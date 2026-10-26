/* From goz-shuttle fw/tests (commit 200592d). */
#include "net/mqtt.h"
#include "minitest.h"

/* ---- fake transport ---------------------------------------------------------- */

static uint8_t out[4096];
static size_t  out_len;
static bool    link_ok = true;

static bool fake_send(void *io, const uint8_t *d, size_t n)
{
    (void)io;
    if (!link_ok) return false;
    if (out_len + n <= sizeof out) memcpy(out + out_len, d, n);
    out_len += n;
    return true;
}

static char   got_topic[64];
static char   got_payload[256];
static size_t got_len;
static int    got_count;
static bool   got_terminated;

static void on_msg(void *u, const char *topic, const char *payload, size_t len)
{
    (void)u;
    strncpy(got_topic, topic, sizeof got_topic - 1);
    got_len = len;
    if (len < sizeof got_payload) memcpy(got_payload, payload, len);
    got_terminated = payload[len] == 0;
    ++got_count;
}

static uint8_t rx[64];
static sh_mqtt m;

static void client(void)
{
    sh_mqtt_init(&m, rx, sizeof rx, fake_send, NULL, on_msg, NULL);
    out_len = 0;
    link_ok = true;
    got_count = 0;
}

static const sh_mqtt_connect_opts plain = { "abc", NULL, NULL, 30, NULL, NULL, 0, 0, false };

static void up(void)
{
    client();
    CHECK(sh_mqtt_start(&m, &plain, 0));
    static const uint8_t ack[] = { 0x20, 0x02, 0x00, 0x00 };
    CHECK(sh_mqtt_feed(&m, ack, sizeof ack, 10));
    CHECK(sh_mqtt_up(&m));
    out_len = 0;
}

/* ---- packets ------------------------------------------------------------------ */

static void test_connect_packet(void)
{
    uint8_t b[128];
    static const uint8_t want[] = { 0x10, 0x0F, 0, 4, 'M', 'Q', 'T', 'T', 4, 0x02, 0, 30, 0, 3, 'a', 'b', 'c' };
    CHECK_EQ_INT(sh_mqtt_build_connect(b, sizeof b, &plain), sizeof want);
    CHECK(memcmp(b, want, sizeof want) == 0);

    /* will (QoS 1, retain), user and password */
    sh_mqtt_connect_opts o = { "id", "u", "pw", 60, "t/c", (const uint8_t *)"off", 3, 1, true };
    size_t n = sh_mqtt_build_connect(b, sizeof b, &o);
    static const uint8_t want2[] = { 0x10, 10 + 4 + 5 + 5 + 3 + 4,
                                     0, 4, 'M', 'Q', 'T', 'T', 4, 0xEE, 0, 60,
                                     0, 2, 'i', 'd', 0, 3, 't', '/', 'c', 0, 3, 'o', 'f', 'f',
                                     0, 1, 'u', 0, 2, 'p', 'w' };
    CHECK_EQ_INT(n, sizeof want2);
    CHECK(memcmp(b, want2, sizeof want2) == 0);

    /* a password without a user is not sent */
    sh_mqtt_connect_opts o3 = { "id", "", "pw", 60, NULL, NULL, 0, 0, false };
    n = sh_mqtt_build_connect(b, sizeof b, &o3);
    CHECK_EQ_INT(b[9], 0x02);
    CHECK(sh_mqtt_build_connect(b, 10, &plain) == 0);  /* too small */
}

static void test_publish_and_subscribe_packets(void)
{
    uint8_t b[16];
    static const uint8_t p0[] = { 0x31, 8, 0, 3, 'a', '/', 'b' };
    CHECK_EQ_INT(sh_mqtt_build_publish_head(b, sizeof b, "a/b", 3, 0, true, 0), sizeof p0);
    CHECK(memcmp(b, p0, sizeof p0) == 0);
    static const uint8_t p1[] = { 0x32, 10, 0, 3, 'a', '/', 'b', 0, 7 };
    CHECK_EQ_INT(sh_mqtt_build_publish_head(b, sizeof b, "a/b", 3, 1, false, 7), sizeof p1);
    CHECK(memcmp(b, p1, sizeof p1) == 0);
    /* two-byte remaining length: 2 + 3 + 200 = 205 = 0xCD 0x01 */
    CHECK_EQ_INT(sh_mqtt_build_publish_head(b, sizeof b, "a/b", 200, 0, false, 0), 8);
    CHECK_EQ_INT(b[1], 0xCD);
    CHECK_EQ_INT(b[2], 0x01);
    /* 16000 bytes: 0x80 0x7D */
    sh_mqtt_build_publish_head(b, sizeof b, "a/b", 16000 - 5, 0, false, 0);
    CHECK_EQ_INT(b[1], 0x80);
    CHECK_EQ_INT(b[2], 0x7D);

    static const uint8_t s[] = { 0x82, 8, 0, 5, 0, 3, 'x', '/', 'y', 0 };
    CHECK_EQ_INT(sh_mqtt_build_subscribe(b, sizeof b, 5, "x/y", 0), sizeof s);
    CHECK(memcmp(b, s, sizeof s) == 0);
}

/* ---- client ------------------------------------------------------------------- */

static void test_connack(void)
{
    client();
    CHECK(sh_mqtt_start(&m, &plain, 0));
    CHECK_EQ_INT(m.state, SH_MQTT_WAIT_CONNACK);
    CHECK_EQ_INT(out[0], 0x10);
    /* refused: not authorised */
    static const uint8_t no[] = { 0x20, 0x02, 0x00, 0x05 };
    CHECK(!sh_mqtt_feed(&m, no, sizeof no, 5));
    CHECK_EQ_INT(m.connack_code, 5);
    CHECK_EQ_INT(m.state, SH_MQTT_DOWN);

    /* anything before CONNACK is a protocol error */
    client();
    sh_mqtt_start(&m, &plain, 0);
    static const uint8_t early[] = { 0xD0, 0x00 };
    CHECK(!sh_mqtt_feed(&m, early, sizeof early, 5));

    /* no CONNACK within 10 s */
    client();
    sh_mqtt_start(&m, &plain, 1000);
    CHECK(sh_mqtt_poll(&m, 10999));
    CHECK(!sh_mqtt_poll(&m, 11000));
    CHECK_EQ_INT(m.state, SH_MQTT_DOWN);
}

static void test_incoming_publish(void)
{
    up();
    /* QoS 1 message, fed one byte at a time, together with a PINGRESP */
    static const uint8_t pkt[] = { 0x32, 13, 0, 3, 'o', '/', 'x', 0, 7, '{', '"', 'a', '"', ':', '1', 0xD0, 0x00 };
    for (size_t i = 0; i < sizeof pkt; ++i) CHECK(sh_mqtt_feed(&m, pkt + i, 1, 20));
    CHECK_EQ_INT(got_count, 1);
    CHECK_STR(got_topic, "o/x");
    CHECK_EQ_INT(got_len, 6);
    CHECK(memcmp(got_payload, "{\"a\":1", 6) == 0);
    CHECK(got_terminated);
    static const uint8_t ack[] = { 0x40, 0x02, 0, 7 };
    CHECK_EQ_INT(out_len, sizeof ack);
    CHECK(memcmp(out, ack, sizeof ack) == 0);

    /* two QoS 0 messages in one chunk */
    static const uint8_t two[] = { 0x30, 6, 0, 1, 'a', 'x', 'y', 'z', 0x30, 4, 0, 1, 'b', 'q' };
    CHECK(sh_mqtt_feed(&m, two, sizeof two, 30));
    CHECK_EQ_INT(got_count, 3);
    CHECK_STR(got_topic, "b");
    CHECK_EQ_INT(got_len, 1);

    /* longer than the buffer: dropped and counted, the stream stays in step */
    uint8_t big[2 + 2 + 1 + 100];
    big[0] = 0x30; big[1] = 103; big[2] = 0; big[3] = 1; big[4] = 'c';
    memset(big + 5, 'z', 100);
    CHECK(sh_mqtt_feed(&m, big, 2 + 103, 40));
    CHECK_EQ_INT(m.dropped, 1);
    CHECK_EQ_INT(got_count, 3);
    CHECK(sh_mqtt_feed(&m, two, sizeof two, 50));
    CHECK_EQ_INT(got_count, 5);

    /* topic length beyond the packet */
    static const uint8_t bad[] = { 0x30, 3, 0, 9, 'x' };
    CHECK(!sh_mqtt_feed(&m, bad, sizeof bad, 60));
    CHECK_EQ_INT(m.state, SH_MQTT_DOWN);
}

static void test_keep_alive(void)
{
    up();                                            /* keep-alive 30 s, connected at t = 0 */
    CHECK(sh_mqtt_poll(&m, 14999));
    CHECK_EQ_INT(out_len, 0);
    CHECK(sh_mqtt_poll(&m, 15000));
    CHECK_EQ_INT(out_len, 2);
    CHECK_EQ_INT(out[0], 0xC0);
    static const uint8_t resp[] = { 0xD0, 0x00 };
    CHECK(sh_mqtt_feed(&m, resp, sizeof resp, 15010));
    CHECK(sh_mqtt_poll(&m, 30000));                  /* answered: next ping */
    CHECK_EQ_INT(out_len, 4);
    CHECK(!sh_mqtt_poll(&m, 45000));                 /* this one was not */
    CHECK_EQ_INT(m.state, SH_MQTT_DOWN);
}

static void test_publish_and_broken_link(void)
{
    up();
    CHECK(sh_mqtt_publish(&m, "a/b", "xyz", 3, 0, false, 100));
    static const uint8_t want[] = { 0x30, 8, 0, 3, 'a', '/', 'b', 'x', 'y', 'z' };
    CHECK_EQ_INT(out_len, sizeof want);
    CHECK(memcmp(out, want, sizeof want) == 0);
    out_len = 0;
    CHECK(sh_mqtt_publish(&m, "a/b", "xyz", 3, 1, true, 100));
    CHECK_EQ_INT(out[0], 0x33);
    CHECK_EQ_INT(out[8], 1);                         /* first packet id */
    CHECK(sh_mqtt_subscribe(&m, "o/x", 0, 100));

    link_ok = false;
    CHECK(!sh_mqtt_publish(&m, "a/b", "xyz", 3, 0, false, 200));
    CHECK_EQ_INT(m.state, SH_MQTT_DOWN);
    CHECK(!sh_mqtt_publish(&m, "a/b", "xyz", 3, 0, false, 200));   /* stays down */

    up();
    sh_mqtt_disconnect(&m);
    CHECK_EQ_INT(out_len, 2);
    CHECK_EQ_INT(out[0], 0xE0);
    CHECK_EQ_INT(m.state, SH_MQTT_DOWN);
}

MT_MAIN({
    RUN(test_connect_packet);
    RUN(test_publish_and_subscribe_packets);
    RUN(test_connack);
    RUN(test_incoming_publish);
    RUN(test_keep_alive);
    RUN(test_publish_and_broken_link);
})
