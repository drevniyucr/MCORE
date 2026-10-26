/* From goz-shuttle fw/core (commit 200592d): transport-agnostic MQTT 3.1.1 client,
 * C11, no heap. Keep in sync with the shuttle copy; only the include path differs. */
#include "net/mqtt.h"

#include <string.h>

#define CONNACK_TIMEOUT_MS 10000u

/* ---- encoding ---------------------------------------------------------------- */

typedef struct {
    uint8_t *p;
    size_t   n, cap;
    bool     err;
} wr;

static void put(wr *w, const void *d, size_t n)
{
    if (w->err || w->n + n > w->cap) { w->err = true; return; }
    memcpy(w->p + w->n, d, n);
    w->n += n;
}
static void put8(wr *w, uint8_t b) { put(w, &b, 1); }
static void put16(wr *w, uint16_t v) { put8(w, (uint8_t)(v >> 8)); put8(w, (uint8_t)v); }
static void put_str(wr *w, const char *s, size_t n) { put16(w, (uint16_t)n); put(w, s, n); }

static void put_varint(wr *w, size_t v)
{
    do {
        uint8_t d = (uint8_t)(v % 128);
        v /= 128;
        if (v) d |= 0x80;
        put8(w, d);
    } while (v);
}

static size_t finish(const wr *w) { return w->err ? 0 : w->n; }

size_t sh_mqtt_build_connect(uint8_t *buf, size_t cap, const sh_mqtt_connect_opts *o)
{
    bool user = o->user && o->user[0];
    bool pass = user && o->pass && o->pass[0];
    bool will = o->will_topic && o->will_topic[0];
    size_t id_n = strlen(o->client_id);
    size_t body = 10 + 2 + id_n;
    if (will) body += 2 + strlen(o->will_topic) + 2 + o->will_len;
    if (user) body += 2 + strlen(o->user);
    if (pass) body += 2 + strlen(o->pass);
    if (body > 268435455u || id_n > 0xFFFF || (will && o->will_len > 0xFFFF)) return 0;

    uint8_t flags = 0x02;                               /* clean session */
    if (will) flags |= 0x04 | (uint8_t)((o->will_qos & 3) << 3) | (o->will_retain ? 0x20 : 0);
    if (user) flags |= 0x80;
    if (pass) flags |= 0x40;

    wr w = { buf, 0, cap, false };
    put8(&w, SH_MQTT_CONNECT << 4);
    put_varint(&w, body);
    put_str(&w, "MQTT", 4);
    put8(&w, 4);                                        /* protocol level 3.1.1 */
    put8(&w, flags);
    put16(&w, o->keep_alive_s);
    put_str(&w, o->client_id, id_n);
    if (will) {
        put_str(&w, o->will_topic, strlen(o->will_topic));
        put16(&w, (uint16_t)o->will_len);
        put(&w, o->will_msg, o->will_len);
    }
    if (user) put_str(&w, o->user, strlen(o->user));
    if (pass) put_str(&w, o->pass, strlen(o->pass));
    return finish(&w);
}

size_t sh_mqtt_build_subscribe(uint8_t *buf, size_t cap, uint16_t id, const char *topic, uint8_t qos)
{
    size_t tn = strlen(topic);
    if (tn > 0xFFFF) return 0;
    wr w = { buf, 0, cap, false };
    put8(&w, (SH_MQTT_SUBSCRIBE << 4) | 0x02);
    put_varint(&w, 2 + 2 + tn + 1);
    put16(&w, id);
    put_str(&w, topic, tn);
    put8(&w, qos & 1);
    return finish(&w);
}

size_t sh_mqtt_build_publish_head(uint8_t *buf, size_t cap, const char *topic, size_t payload_len,
                                  uint8_t qos, bool retain, uint16_t id)
{
    size_t tn = strlen(topic);
    size_t body = 2 + tn + (qos ? 2 : 0) + payload_len;
    if (tn > 0xFFFF || body > 268435455u) return 0;
    wr w = { buf, 0, cap, false };
    put8(&w, (uint8_t)((SH_MQTT_PUBLISH << 4) | ((qos & 1) << 1) | (retain ? 1 : 0)));
    put_varint(&w, body);
    put_str(&w, topic, tn);
    if (qos) put16(&w, id);
    return finish(&w);
}

size_t sh_mqtt_build_puback(uint8_t *buf, size_t cap, uint16_t id)
{
    wr w = { buf, 0, cap, false };
    put8(&w, SH_MQTT_PUBACK << 4);
    put8(&w, 2);
    put16(&w, id);
    return finish(&w);
}

size_t sh_mqtt_build_pingreq(uint8_t *buf, size_t cap)
{
    wr w = { buf, 0, cap, false };
    put8(&w, SH_MQTT_PINGREQ << 4);
    put8(&w, 0);
    return finish(&w);
}

size_t sh_mqtt_build_disconnect(uint8_t *buf, size_t cap)
{
    wr w = { buf, 0, cap, false };
    put8(&w, SH_MQTT_DISCONNECT << 4);
    put8(&w, 0);
    return finish(&w);
}

/* ---- client -------------------------------------------------------------------- */

void sh_mqtt_init(sh_mqtt *m, uint8_t *rx, size_t rx_cap,
                  sh_mqtt_send_fn send, void *io, sh_mqtt_msg_fn on_msg, void *user)
{
    memset(m, 0, sizeof *m);
    m->rx = rx;
    m->rx_cap = rx_cap;
    m->send = send;
    m->io = io;
    m->on_msg = on_msg;
    m->user = user;
    m->next_id = 1;
}

void sh_mqtt_reset(sh_mqtt *m)
{
    m->state = SH_MQTT_DOWN;
    m->phase = 0;
    m->skipping = false;
    m->ping_outstanding = false;
}

static bool send_all(sh_mqtt *m, const void *d, size_t n)
{
    if (n == 0 || !m->send(m->io, (const uint8_t *)d, n)) {
        sh_mqtt_reset(m);
        return false;
    }
    return true;
}

static uint16_t take_id(sh_mqtt *m)
{
    uint16_t id = m->next_id++;
    if (m->next_id == 0) m->next_id = 1;
    return id;
}

bool sh_mqtt_start(sh_mqtt *m, const sh_mqtt_connect_opts *o, uint32_t now_ms)
{
    sh_mqtt_reset(m);
    size_t n = sh_mqtt_build_connect(m->rx, m->rx_cap, o);   /* rx is free before CONNACK */
    m->keep_alive_s = o->keep_alive_s;
    m->started_ms = m->last_ping_ms = m->last_rx_ms = now_ms;
    m->connack_code = 0xFF;
    if (!send_all(m, m->rx, n)) return false;
    m->state = SH_MQTT_WAIT_CONNACK;
    return true;
}

/* One complete packet in rx[0..remaining). */
static bool handle(sh_mqtt *m)
{
    uint8_t type = m->hdr >> 4;
    uint8_t *b = m->rx;
    uint32_t n = m->remaining;

    if (m->state == SH_MQTT_WAIT_CONNACK && type != SH_MQTT_CONNACK) {
        m->protocol_errors++;
        return false;
    }
    switch (type) {
    case SH_MQTT_CONNACK:
        if (n != 2 || m->state != SH_MQTT_WAIT_CONNACK) { m->protocol_errors++; return false; }
        m->connack_code = b[1];
        if (b[1] != 0) return false;
        m->state = SH_MQTT_UP;
        return true;
    case SH_MQTT_PUBLISH: {
        uint8_t qos = (m->hdr >> 1) & 3;
        if (n < 2 || qos > 1) { m->protocol_errors++; return false; }
        uint32_t tn = ((uint32_t)b[0] << 8) | b[1];
        uint32_t p = 2 + tn + (qos ? 2 : 0);
        if (p > n) { m->protocol_errors++; return false; }
        uint16_t id = qos ? (uint16_t)((b[2 + tn] << 8) | b[3 + tn]) : 0;
        memmove(b, b + 2, tn);                            /* topic to the front, NUL after it */
        b[tn] = 0;
        b[n] = 0;                                         /* rx has one spare byte */
        if (m->on_msg) m->on_msg(m->user, (const char *)b, (const char *)b + p, n - p);
        if (qos) {
            uint8_t ack[4];
            return send_all(m, ack, sh_mqtt_build_puback(ack, sizeof ack, id));
        }
        return true;
    }
    case SH_MQTT_SUBACK:
        if (n < 3) { m->protocol_errors++; return false; }
        if (b[2] == 0x80) m->protocol_errors++;           /* subscription refused */
        return true;
    case SH_MQTT_PINGRESP:
        m->ping_outstanding = false;
        return true;
    case SH_MQTT_PUBACK:
        return true;
    default:
        m->protocol_errors++;
        return false;                                     /* a client never receives the others */
    }
}

bool sh_mqtt_feed(sh_mqtt *m, const uint8_t *data, size_t n, uint32_t now_ms)
{
    if (m->state == SH_MQTT_DOWN) return false;
    if (n) m->last_rx_ms = now_ms;
    for (size_t i = 0; i < n;) {
        uint8_t c = data[i];
        if (m->phase == 0) {
            m->hdr = c;
            m->remaining = 0;
            m->len_mul = 1;
            m->len_bytes = 0;
            m->phase = 1;
            ++i;
        } else if (m->phase == 1) {
            m->remaining += (uint32_t)(c & 0x7F) * m->len_mul;
            m->len_mul *= 128;
            ++i;
            if (++m->len_bytes > 4) { m->protocol_errors++; sh_mqtt_reset(m); return false; }
            if (!(c & 0x80)) {
                m->got = 0;
                m->skipping = m->remaining + 1 > m->rx_cap;
                if (m->skipping) m->dropped++;
                m->phase = 2;
                if (m->remaining == 0) {
                    m->phase = 0;
                    if (!handle(m)) { sh_mqtt_reset(m); return false; }
                }
            }
        } else {
            size_t take = n - i;
            if (take > m->remaining - m->got) take = m->remaining - m->got;
            if (!m->skipping) memcpy(m->rx + m->got, data + i, take);
            m->got += (uint32_t)take;
            i += take;
            if (m->got == m->remaining) {
                m->phase = 0;
                if (m->skipping) {
                    m->skipping = false;
                } else if (!handle(m)) {
                    sh_mqtt_reset(m);
                    return false;
                }
            }
        }
    }
    return true;
}

bool sh_mqtt_poll(sh_mqtt *m, uint32_t now_ms)
{
    if (m->state == SH_MQTT_DOWN) return false;
    if (m->state == SH_MQTT_WAIT_CONNACK) {
        if (now_ms - m->started_ms >= CONNACK_TIMEOUT_MS) { sh_mqtt_reset(m); return false; }
        return true;
    }
    if (m->keep_alive_s == 0) return true;
    uint32_t half = (uint32_t)m->keep_alive_s * 500u;
    if (now_ms - m->last_ping_ms >= half) {
        if (m->ping_outstanding) { sh_mqtt_reset(m); return false; }   /* no answer to the last one */
        uint8_t p[2];
        if (!send_all(m, p, sh_mqtt_build_pingreq(p, sizeof p))) return false;
        m->ping_outstanding = true;
        m->last_ping_ms = now_ms;
    }
    return true;
}

bool sh_mqtt_subscribe(sh_mqtt *m, const char *topic, uint8_t qos, uint32_t now_ms)
{
    (void)now_ms;
    if (m->state != SH_MQTT_UP) return false;
    uint8_t b[160];
    return send_all(m, b, sh_mqtt_build_subscribe(b, sizeof b, take_id(m), topic, qos));
}

bool sh_mqtt_publish(sh_mqtt *m, const char *topic, const void *payload, size_t len,
                     uint8_t qos, bool retain, uint32_t now_ms)
{
    (void)now_ms;
    if (m->state != SH_MQTT_UP) return false;
    uint8_t h[160];
    size_t hn = sh_mqtt_build_publish_head(h, sizeof h, topic, len, qos, retain, qos ? take_id(m) : 0);
    if (!send_all(m, h, hn)) return false;
    return len == 0 || send_all(m, payload, len);
}

void sh_mqtt_disconnect(sh_mqtt *m)
{
    if (m->state == SH_MQTT_UP) {
        uint8_t b[2];
        send_all(m, b, sh_mqtt_build_disconnect(b, sizeof b));
    }
    sh_mqtt_reset(m);
}
