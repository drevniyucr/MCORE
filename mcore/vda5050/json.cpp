/*
 * json.cpp — see json.hpp.
 */
#include "vda5050/json.hpp"

#include <cstdlib>

namespace json {

// ===========================================================================
// Reader
// ===========================================================================
void JsonReader::ws() {
	while (p_ < end_ && (*p_ == ' ' || *p_ == '\t' || *p_ == '\n' || *p_ == '\r')) {
		++p_;
	}
}

char JsonReader::peek() {
	ws();
	return (ok_ && p_ < end_) ? *p_ : '\0';
}

bool JsonReader::expect(char c) {
	ws();
	if (!ok_ || p_ >= end_ || *p_ != c) {
		ok_ = false;
		return false;
	}
	++p_;
	return true;
}

// Values consume their trailing comma, so containers only look for the
// closing bracket. (Lenient: a trailing comma before '}' / ']' is accepted.)
bool JsonReader::after_value() {
	ws();
	if (p_ < end_ && *p_ == ',') {
		++p_;
	}
	return ok_;
}

bool JsonReader::enter_object() {
	return expect('{');
}

bool JsonReader::next_key(Span &key) {
	ws();
	if (!ok_ || p_ >= end_) {
		ok_ = false;
		return false;
	}
	if (*p_ == '}') {
		++p_;
		after_value();
		return false;
	}
	if (*p_ != '"') {
		ok_ = false;
		return false;
	}
	const char *start = ++p_;
	while (p_ < end_ && *p_ != '"') {
		if (*p_ == '\\') {
			++p_;                       // keys with escapes are compared raw
		}
		++p_;
	}
	if (p_ >= end_) {
		ok_ = false;
		return false;
	}
	key.p = start;
	key.n = static_cast<size_t>(p_ - start);
	++p_;
	return expect(':');
}

bool JsonReader::enter_array() {
	return expect('[');
}

bool JsonReader::next_element() {
	ws();
	if (!ok_ || p_ >= end_) {
		ok_ = false;
		return false;
	}
	if (*p_ == ']') {
		++p_;
		after_value();
		return false;
	}
	return true;
}

namespace {
int hex(char c) {
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'a' && c <= 'f') return c - 'a' + 10;
	if (c >= 'A' && c <= 'F') return c - 'A' + 10;
	return -1;
}
} // namespace

bool JsonReader::read_string(char *out, size_t cap) {
	if (!expect('"') || cap == 0) {
		ok_ = false;
		return false;
	}
	size_t n = 0;
	auto emit = [&](char c) {
		if (n + 1 >= cap) {
			ok_ = false;
			return;
		}
		out[n++] = c;
	};
	while (ok_ && p_ < end_ && *p_ != '"') {
		char c = *p_++;
		if (static_cast<unsigned char>(c) < 0x20) {
			ok_ = false;                    // raw control character
			break;
		}
		if (c != '\\') {
			emit(c);
			continue;
		}
		if (p_ >= end_) {
			ok_ = false;
			break;
		}
		c = *p_++;
		switch (c) {
		case '"': case '\\': case '/': emit(c); break;
		case 'b': emit('\b'); break;
		case 'f': emit('\f'); break;
		case 'n': emit('\n'); break;
		case 'r': emit('\r'); break;
		case 't': emit('\t'); break;
		case 'u': {
			if (end_ - p_ < 4) { ok_ = false; break; }
			uint32_t cp = 0;
			for (int i = 0; i < 4; ++i) {
				const int h = hex(*p_++);
				if (h < 0) { ok_ = false; break; }
				cp = (cp << 4) | static_cast<uint32_t>(h);
			}
			// Surrogate pair
			if (ok_ && cp >= 0xD800 && cp <= 0xDBFF && end_ - p_ >= 6 && p_[0] == '\\' && p_[1] == 'u') {
				uint32_t lo = 0;
				bool good = true;
				for (int i = 2; i < 6; ++i) {
					const int h = hex(p_[i]);
					if (h < 0) { good = false; break; }
					lo = (lo << 4) | static_cast<uint32_t>(h);
				}
				if (good && lo >= 0xDC00 && lo <= 0xDFFF) {
					cp = 0x10000 + ((cp - 0xD800) << 10) + (lo - 0xDC00);
					p_ += 6;
				}
			}
			// UTF-8
			if (cp < 0x80) {
				emit(static_cast<char>(cp));
			} else if (cp < 0x800) {
				emit(static_cast<char>(0xC0 | (cp >> 6)));
				emit(static_cast<char>(0x80 | (cp & 0x3F)));
			} else if (cp < 0x10000) {
				emit(static_cast<char>(0xE0 | (cp >> 12)));
				emit(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
				emit(static_cast<char>(0x80 | (cp & 0x3F)));
			} else {
				emit(static_cast<char>(0xF0 | (cp >> 18)));
				emit(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
				emit(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
				emit(static_cast<char>(0x80 | (cp & 0x3F)));
			}
			break;
		}
		default:
			ok_ = false;
			break;
		}
	}
	if (!ok_ || p_ >= end_) {
		ok_ = false;
		out[0] = '\0';
		return false;
	}
	++p_;                                   // closing quote
	out[n] = '\0';
	return after_value();
}

bool JsonReader::read_number(double &v) {
	ws();
	char buf[40];
	size_t n = 0;
	while (p_ < end_ && n < sizeof(buf) - 1) {
		const char c = *p_;
		if ((c >= '0' && c <= '9') || c == '-' || c == '+' || c == '.' || c == 'e' || c == 'E') {
			buf[n++] = c;
			++p_;
		} else {
			break;
		}
	}
	buf[n] = '\0';
	char *endp = nullptr;
	v = strtod(buf, &endp);
	if (n == 0 || endp != buf + n || !(buf[0] == '-' || (buf[0] >= '0' && buf[0] <= '9'))) {
		ok_ = false;
		return false;
	}
	return after_value();
}

bool JsonReader::read_int(int64_t &v) {
	double d = 0;
	if (!read_number(d)) {
		return false;
	}
	if (d != static_cast<double>(static_cast<int64_t>(d)) || d > 9.0e15 || d < -9.0e15) {
		ok_ = false;
		return false;
	}
	v = static_cast<int64_t>(d);
	return true;
}

bool JsonReader::read_bool(bool &v) {
	ws();
	if (end_ - p_ >= 4 && memcmp(p_, "true", 4) == 0) {
		v = true;
		p_ += 4;
		return after_value();
	}
	if (end_ - p_ >= 5 && memcmp(p_, "false", 5) == 0) {
		v = false;
		p_ += 5;
		return after_value();
	}
	ok_ = false;
	return false;
}

bool JsonReader::peek_null() {
	ws();
	if (end_ - p_ >= 4 && memcmp(p_, "null", 4) == 0) {
		p_ += 4;
		after_value();
		return true;
	}
	return false;
}

bool JsonReader::skip_string() {
	if (!expect('"')) {
		return false;
	}
	while (p_ < end_ && *p_ != '"') {
		if (*p_ == '\\') {
			++p_;
		}
		++p_;
	}
	if (p_ >= end_) {
		ok_ = false;
		return false;
	}
	++p_;
	return after_value();
}

bool JsonReader::skip() {
	const char c = peek();
	switch (c) {
	case '{': {
		enter_object();
		Span k;
		int guard = 0;
		while (ok_ && next_key(k) && ++guard < 100000) {
			skip();
		}
		return ok_;
	}
	case '[':
		enter_array();
		while (ok_ && next_element()) {
			skip();
		}
		return ok_;
	case '"':
		return skip_string();
	case 't':
	case 'f': {
		bool b;
		return read_bool(b);
	}
	case 'n':
		if (!peek_null()) {
			ok_ = false;
		}
		return ok_;
	default: {
		double d;
		return read_number(d);
	}
	}
}

// ===========================================================================
// Writer
// ===========================================================================
void JsonWriter::raw(const char *s, size_t n) {
	if (!ok_ || len_ + n + 1 > cap_) {
		ok_ = false;
		return;
	}
	memcpy(&buf_[len_], s, n);
	len_ += n;
	buf_[len_] = '\0';
}

void JsonWriter::value_prefix() {
	if (after_key_) {
		after_key_ = false;
		return;
	}
	if (depth_ > 0) {
		if (!first_[depth_]) {
			put(',');
		}
		first_[depth_] = false;
	}
}

JsonWriter &JsonWriter::begin_object() {
	value_prefix();
	put('{');
	if (depth_ + 1 >= static_cast<int>(sizeof(first_))) {
		ok_ = false;
		return *this;
	}
	first_[++depth_] = true;
	return *this;
}

JsonWriter &JsonWriter::end_object() {
	put('}');
	if (depth_ > 0) --depth_;
	return *this;
}

JsonWriter &JsonWriter::begin_array() {
	value_prefix();
	put('[');
	if (depth_ + 1 >= static_cast<int>(sizeof(first_))) {
		ok_ = false;
		return *this;
	}
	first_[++depth_] = true;
	return *this;
}

JsonWriter &JsonWriter::end_array() {
	put(']');
	if (depth_ > 0) --depth_;
	return *this;
}

JsonWriter &JsonWriter::key(const char *k) {
	value_prefix();
	quoted(k, strlen(k));
	put(':');
	after_key_ = true;
	return *this;
}

JsonWriter &JsonWriter::str(const char *s) {
	return str(s, strlen(s));
}

JsonWriter &JsonWriter::str(const char *s, size_t n) {
	value_prefix();
	quoted(s, n);
	return *this;
}

void JsonWriter::quoted(const char *s, size_t n) {
	put('"');
	size_t run = 0;   // start of the pending run of characters that need no escape
	for (size_t i = 0; i < n; ++i) {
		const unsigned char c = static_cast<unsigned char>(s[i]);
		if (c >= 0x20 && c != '"' && c != '\\') {
			continue;
		}
		raw(s + run, i - run);
		run = i + 1;
		switch (c) {
		case '"': raw("\\\"", 2); break;
		case '\\': raw("\\\\", 2); break;
		case '\n': raw("\\n", 2); break;
		case '\r': raw("\\r", 2); break;
		case '\t': raw("\\t", 2); break;
		default:
			if (c < 0x20) {
				static const char hexd[] = "0123456789abcdef";
				const char esc[6] = { '\\', 'u', '0', '0', hexd[c >> 4], hexd[c & 0xF] };
				raw(esc, 6);
			}
		}
	}
	raw(s + run, n - run);
	put('"');
}

JsonWriter &JsonWriter::integer(int64_t v) {
	value_prefix();
	char tmp[24];
	size_t n = 0;
	const bool neg = v < 0;
	uint64_t u = neg ? static_cast<uint64_t>(-(v + 1)) + 1U : static_cast<uint64_t>(v);
	// 64-bit division is a library call on Cortex-M: digits above 2^32 only
	while (u > 0xFFFFFFFFULL) {
		tmp[n++] = static_cast<char>('0' + u % 10U);
		u /= 10U;
	}
	uint32_t u32 = static_cast<uint32_t>(u);
	do {
		tmp[n++] = static_cast<char>('0' + u32 % 10U);
		u32 /= 10U;
	} while (u32 != 0U);
	if (neg) {
		put('-');
	}
	while (n > 0) {
		put(tmp[--n]);
	}
	return *this;
}

JsonWriter &JsonWriter::num(double v, uint8_t decimals) {
	if (!(v == v) || v > 1e15 || v < -1e15) {
		v = 0.0;                            // NaN / huge: JSON has no representation
	}
	if (decimals > 9) {
		decimals = 9;
	}
	uint32_t scale = 1;   // decimals <= 9: fits
	for (uint8_t i = 0; i < decimals; ++i) {
		scale *= 10U;
	}
	const bool neg = v < 0;
	const double a = neg ? -v : v;
	const uint64_t r = static_cast<uint64_t>(a * static_cast<double>(scale) + 0.5);
	uint64_t ip;
	uint32_t fp;
	if (r <= 0xFFFFFFFFULL) {             // the usual case: 32-bit division
		ip = static_cast<uint32_t>(r) / scale;
		fp = static_cast<uint32_t>(r) % scale;
	} else {
		ip = r / scale;
		fp = static_cast<uint32_t>(r % scale);
	}
	if (neg && r != 0U) {
		value_prefix();
		put('-');
		after_key_ = true;                  // the integer below must not add a comma
	}
	integer(static_cast<int64_t>(ip));
	if (fp != 0U) {
		// fixed width, then drop trailing zeros
		char frac[10];
		for (int i = decimals - 1; i >= 0; --i) {
			frac[i] = static_cast<char>('0' + fp % 10U);
			fp /= 10U;
		}
		size_t n = decimals;
		while (n > 0 && frac[n - 1] == '0') {
			--n;
		}
		put('.');
		raw(frac, n);
	}
	return *this;
}

JsonWriter &JsonWriter::boolean(bool v) {
	value_prefix();
	if (v) raw("true", 4); else raw("false", 5);
	return *this;
}

JsonWriter &JsonWriter::null() {
	value_prefix();
	raw("null", 4);
	return *this;
}

// ===========================================================================
// Time
// ===========================================================================
void format_iso8601(uint64_t unix_ms, char out[25]) {
	// One 64-bit division, the rest in 32 bits (valid until 2106)
	const uint32_t secs = static_cast<uint32_t>(unix_ms / 1000U);
	const uint32_t ms = static_cast<uint32_t>(unix_ms - static_cast<uint64_t>(secs) * 1000U);
	uint32_t days = secs / 86400U;
	const uint32_t sod = secs % 86400U;

	// civil_from_days (H. Hinnant), proleptic Gregorian calendar, days >= 0
	days += 719468;
	const uint32_t era = days / 146097;
	const uint32_t doe = days - era * 146097;
	const uint32_t yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
	int64_t y = static_cast<int64_t>(yoe) + static_cast<int64_t>(era) * 400;
	const uint32_t doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
	const uint32_t mp = (5 * doy + 2) / 153;
	const uint32_t d = doy - (153 * mp + 2) / 5 + 1;
	const uint32_t m = mp < 10 ? mp + 3 : mp - 9;
	y += (m <= 2) ? 1 : 0;

	auto put2 = [](char *p, uint32_t v) { p[0] = static_cast<char>('0' + v / 10); p[1] = static_cast<char>('0' + v % 10); };
	const uint32_t yy = static_cast<uint32_t>(y);
	out[0] = static_cast<char>('0' + (yy / 1000) % 10);
	out[1] = static_cast<char>('0' + (yy / 100) % 10);
	out[2] = static_cast<char>('0' + (yy / 10) % 10);
	out[3] = static_cast<char>('0' + yy % 10);
	out[4] = '-';
	put2(&out[5], m);
	out[7] = '-';
	put2(&out[8], d);
	out[10] = 'T';
	put2(&out[11], sod / 3600);
	out[13] = ':';
	put2(&out[14], (sod / 60) % 60);
	out[16] = ':';
	put2(&out[17], sod % 60);
	out[19] = '.';
	out[20] = static_cast<char>('0' + ms / 100);
	out[21] = static_cast<char>('0' + (ms / 10) % 10);
	out[22] = static_cast<char>('0' + ms % 10);
	out[23] = 'Z';
	out[24] = '\0';
}

} // namespace json
