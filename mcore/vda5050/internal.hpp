#pragma once
/*
 * internal.hpp — helpers shared by the vda5050 implementation files.
 * Not part of the public interface.
 */
#include <cstring>

#include "vda5050/json.hpp"
#include "vda5050/vda5050.hpp"

namespace vda {
namespace detail {

template <size_t N>
inline void copy_str(char (&dst)[N], const char *src) {
	strncpy(dst, src != nullptr ? src : "", N - 1);
	dst[N - 1] = '\0';
}

inline bool is_final(ActionStatus s) {
	return s == ActionStatus::Finished || s == ActionStatus::Failed;
}
inline bool is_blocking_drive(Blocking b) {
	return b == Blocking::Soft || b == Blocking::Hard;
}
inline bool is_exclusive(Blocking b) {
	return b == Blocking::Single || b == Blocking::Hard;
}

bool parse_blocking(const char *s, Blocking &b);
const char *param(const ActionParams &a, const char *key);
bool param_float(const ActionParams &a, const char *key, float &out);

// Reads one action object (order, instant or zone action). Zone actions
// have no actionId (require_id = false).
struct ParseErr {
	bool too_long = false;
	char detail[ID_LEN] = "";
};
// params may be null: the parameters are skipped
bool parse_action(json::JsonReader &r, Action &a, int32_t owner_seq, bool require_id, ParseErr &err,
                  ActionParams *params = nullptr);

// Order/zone action types the robot can execute, by scope (Table 4)
bool allowed_on_node(ActionKind k);
bool allowed_on_edge(ActionKind k);
bool allowed_in_zone(ActionKind k);
bool is_instant_kind(ActionKind k);

// ISO 8601 "YYYY-MM-DDTHH:mm:ss(.fff)Z" -> Unix ms (0 on error)
uint64_t parse_iso8601(const char *s);

void u32_to_str(uint32_t v, char *out);   // out >= 11 bytes

} // namespace detail
} // namespace vda
