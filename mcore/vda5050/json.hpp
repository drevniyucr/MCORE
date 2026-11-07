#pragma once
/*
 * json.hpp — allocation-free JSON reader (pull parser) and writer.
 *
 * JsonReader walks the text directly: no token array, so a 16 KB VDA 5050
 * order costs no RAM beyond the structures it is parsed into. Typical use:
 *
 *   JsonReader r(text, len);
 *   if (r.enter_object()) {
 *       Span key;
 *       while (r.next_key(key)) {
 *           if (key == "orderId") r.read_string(buf, sizeof(buf));
 *           else r.skip();
 *       }
 *   }
 *   if (!r.ok()) ...   // any syntax error latches ok() == false
 *
 * JsonWriter appends to a caller buffer and inserts commas itself; numbers
 * are formatted without printf (newlib-nano has no %f).
 */
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace json {

struct Span {
	const char *p = nullptr;
	size_t n = 0;
	bool operator==(const char *s) const { return strlen(s) == n && memcmp(p, s, n) == 0; }
};

class JsonReader {
public:
	JsonReader(const char *text, size_t len) : p_(text), end_(text + len) {}

	bool ok() const { return ok_; }
	const char *pos() const { return p_; }

	// '{' ... next_key() returns false at the closing '}' (consumed)
	bool enter_object();
	bool next_key(Span &key);
	// '[' ... next_element() returns false at the closing ']' (consumed)
	bool enter_array();
	bool next_element();

	// Value readers. All return false (and latch !ok()) on a type mismatch.
	// read_string unescapes into out (NUL-terminated); too long = error.
	bool read_string(char *out, size_t cap);
	bool read_number(double &v);
	bool read_int(int64_t &v);        // rejects fractions
	bool read_bool(bool &v);
	bool peek_null();                 // true and consumes it if the value is null
	bool skip();                      // any value
	char peek();                      // next significant character, 0 at end

	void fail() { ok_ = false; }

private:
	void ws();
	bool expect(char c);
	bool after_value();               // eats ',' or leaves ']' / '}'
	bool skip_string();

	const char *p_;
	const char *end_;
	bool ok_ = true;
};

class JsonWriter {
public:
	JsonWriter(char *buf, size_t cap) : buf_(buf), cap_(cap) { if (cap_) buf_[0] = '\0'; }

	JsonWriter &begin_object();
	JsonWriter &end_object();
	JsonWriter &begin_array();
	JsonWriter &end_array();
	JsonWriter &key(const char *k);

	JsonWriter &str(const char *s);
	JsonWriter &str(const char *s, size_t n);
	JsonWriter &num(double v, uint8_t decimals = 3);
	JsonWriter &integer(int64_t v);
	JsonWriter &boolean(bool v);
	JsonWriter &null();

	// "key": value shortcuts
	JsonWriter &kv(const char *k, const char *v) { return key(k).str(v); }
	JsonWriter &kv(const char *k, double v, uint8_t d = 3) { return key(k).num(v, d); }
	JsonWriter &kv_int(const char *k, int64_t v) { return key(k).integer(v); }
	JsonWriter &kv_bool(const char *k, bool v) { return key(k).boolean(v); }

	bool ok() const { return ok_ && depth_ == 0; }   // complete and not truncated
	bool overflow() const { return !ok_; }
	size_t size() const { return len_; }
	const char *data() const { return buf_; }

private:
	void raw(const char *s, size_t n);
	void put(char c) {
		if (ok_ && len_ + 2 <= cap_) {
			buf_[len_++] = c;
			buf_[len_] = '\0';
		} else {
			ok_ = false;
		}
	}
	void value_prefix();
	void quoted(const char *s, size_t n);

	char *buf_;
	size_t cap_;
	size_t len_ = 0;
	bool ok_ = true;
	uint8_t depth_ = 0;
	bool first_[16] = {};
	bool after_key_ = false;
};

// ISO 8601 UTC "YYYY-MM-DDTHH:mm:ss.fffZ" (VDA 5050 timestamp) into out[25].
void format_iso8601(uint64_t unix_ms, char out[25]);

} // namespace json
