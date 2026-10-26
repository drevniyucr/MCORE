/* From goz-shuttle fw/core (commit 200592d), see mqtt.c. */
/* MQTT 3.1.1 client: packet building, an incremental parser for the
 * incoming byte stream and the keep-alive rules. No transport and no clock
 * of its own: the platform passes byte streams in and out (plain TCP or TLS)
 * and the time in milliseconds, so everything here runs on the host tests.
 *
 * Scope is what a VDA 5050 vehicle needs: CONNECT with last will and
 * optional user/password, SUBSCRIBE, PUBLISH QoS 0/1 out (QoS 1 is not
 * retried: the connection message is re-sent on every connect anyway),
 * PUBLISH QoS 0/1 in (PUBACK sent), PINGREQ, DISCONNECT. No QoS 2, no
 * persistent session (clean session is always set). */
#ifndef SH_MQTT_H
#define SH_MQTT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    SH_MQTT_CONNECT = 1, SH_MQTT_CONNACK = 2, SH_MQTT_PUBLISH = 3, SH_MQTT_PUBACK = 4,
    SH_MQTT_SUBSCRIBE = 8, SH_MQTT_SUBACK = 9, SH_MQTT_PINGREQ = 12, SH_MQTT_PINGRESP = 13,
    SH_MQTT_DISCONNECT = 14,
};

typedef struct {
    const char    *client_id;
    const char    *user;          /* NULL or "" = none */
    const char    *pass;          /* only with a user */
    uint16_t       keep_alive_s;
    const char    *will_topic;    /* NULL = no last will */
    const uint8_t *will_msg;
    size_t         will_len;
    uint8_t        will_qos;
    bool           will_retain;
} sh_mqtt_connect_opts;

/* ---- packet building: return the length written, 0 if `cap` is too small --- */
size_t sh_mqtt_build_connect(uint8_t *buf, size_t cap, const sh_mqtt_connect_opts *o);
size_t sh_mqtt_build_subscribe(uint8_t *buf, size_t cap, uint16_t packet_id, const char *topic, uint8_t qos);
/* Fixed header, topic and packet id of a PUBLISH whose payload of
 * `payload_len` bytes is sent right after (it is not copied). */
size_t sh_mqtt_build_publish_head(uint8_t *buf, size_t cap, const char *topic, size_t payload_len,
                                  uint8_t qos, bool retain, uint16_t packet_id);
size_t sh_mqtt_build_puback(uint8_t *buf, size_t cap, uint16_t packet_id);
size_t sh_mqtt_build_pingreq(uint8_t *buf, size_t cap);
size_t sh_mqtt_build_disconnect(uint8_t *buf, size_t cap);

/* ---- client ------------------------------------------------------------------ */

/* Send `n` bytes completely; false = the link is broken. */
typedef bool (*sh_mqtt_send_fn)(void *io, const uint8_t *data, size_t n);
/* An application message arrived (topic NUL-terminated, payload too: one
 * extra byte is kept after it so JSON parsers can use it as a C string). */
typedef void (*sh_mqtt_msg_fn)(void *user, const char *topic, const char *payload, size_t len);

typedef enum {
    SH_MQTT_DOWN = 0,
    SH_MQTT_WAIT_CONNACK,
    SH_MQTT_UP,
} sh_mqtt_state;

typedef struct {
    sh_mqtt_send_fn send;
    void           *io;
    sh_mqtt_msg_fn  on_msg;
    void           *user;

    uint8_t *rx;               /* assembly buffer of one incoming packet */
    size_t   rx_cap;

    /* parser */
    uint8_t  phase;            /* 0 fixed header byte, 1 remaining length, 2 body */
    uint8_t  hdr;              /* first byte of the packet being read */
    uint32_t remaining;        /* remaining length */
    uint32_t got;              /* body bytes received so far */
    uint8_t  len_bytes;        /* remaining-length bytes read */
    uint32_t len_mul;
    bool     skipping;         /* packet longer than rx_cap - 1: dropped */

    sh_mqtt_state state;
    uint8_t  connack_code;     /* return code of the last CONNACK, 0 = accepted */
    uint16_t keep_alive_s;
    uint32_t started_ms;       /* CONNECT sent */
    uint32_t last_ping_ms;     /* PINGREQ sent */
    uint32_t last_rx_ms;
    bool     ping_outstanding;
    uint16_t next_id;

    uint32_t dropped;          /* incoming messages too long for rx */
    uint32_t protocol_errors;
} sh_mqtt;

void sh_mqtt_init(sh_mqtt *m, uint8_t *rx, size_t rx_cap,
                  sh_mqtt_send_fn send, void *io, sh_mqtt_msg_fn on_msg, void *user);

/* Send CONNECT on a fresh transport; the state becomes WAIT_CONNACK. */
bool sh_mqtt_start(sh_mqtt *m, const sh_mqtt_connect_opts *o, uint32_t now_ms);

/* Feed bytes from the transport. False on a protocol violation or a refused
 * connection (connack_code says why): close the transport then. */
bool sh_mqtt_feed(sh_mqtt *m, const uint8_t *data, size_t n, uint32_t now_ms);

/* Keep-alive: a PINGREQ every half keep-alive period; false when the
 * previous one got no PINGRESP by then, when CONNACK does not come within
 * 10 s, or when a send failed (the link is dead). Call at least every second. */
bool sh_mqtt_poll(sh_mqtt *m, uint32_t now_ms);

bool sh_mqtt_subscribe(sh_mqtt *m, const char *topic, uint8_t qos, uint32_t now_ms);
bool sh_mqtt_publish(sh_mqtt *m, const char *topic, const void *payload, size_t len,
                     uint8_t qos, bool retain, uint32_t now_ms);
/* DISCONNECT (the broker then drops the last will). */
void sh_mqtt_disconnect(sh_mqtt *m);

/* The transport went away: back to DOWN, parser reset. */
void sh_mqtt_reset(sh_mqtt *m);

static inline bool sh_mqtt_up(const sh_mqtt *m) { return m->state == SH_MQTT_UP; }

#ifdef __cplusplus
}
#endif
#endif /* SH_MQTT_H */
