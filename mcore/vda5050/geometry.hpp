#pragma once
/*
 * geometry.hpp — 2-D helpers for VDA 5050: NURBS trajectories (7.3),
 * zone polygons (7.6) and the clearance checks used before a path segment is
 * handed to the vehicle.
 */
#include <cstddef>
#include <cstdint>

namespace vda {
namespace geo {

struct Vec2 {
	float x, y;
};

constexpr size_t MAX_CONTROL_POINTS = 16;
constexpr size_t MAX_KNOTS = MAX_CONTROL_POINTS + 8;   // degree <= 7

struct Nurbs {
	uint8_t degree;
	uint8_t ncp;               // control points
	uint8_t nknots;            // 0 = default (clamped, uniform)
	Vec2 cp[MAX_CONTROL_POINTS];
	float w[MAX_CONTROL_POINTS];
	float knots[MAX_KNOTS];
};

// Checks the rules of 7.3 (degree >= 1, ncp >= degree + 1, knot count,
// clamped ends, non-decreasing knots in [0, 1], positive weights) and fills
// the default knot vector if none was given.
bool nurbs_prepare(Nurbs &n);

// Point on the curve for u in [0, 1] (rational de Boor).
Vec2 nurbs_eval(const Nurbs &n, float u);

// Samples the curve so that consecutive points are at most `spacing` apart.
// Writes up to max points (first = start, last = end) and returns the count.
size_t nurbs_sample(const Nurbs &n, float spacing, Vec2 *out, size_t max);

// Straight segment a->b sampled the same way (a excluded, b included).
size_t line_sample(Vec2 a, Vec2 b, float spacing, Vec2 *out, size_t max);

float dist(Vec2 a, Vec2 b);
float dist_point_segment(Vec2 p, Vec2 a, Vec2 b);

// Polygon (closed, simple, any orientation)
bool point_in_polygon(Vec2 p, const Vec2 *v, size_t n);
float dist_point_polygon_edges(Vec2 p, const Vec2 *v, size_t n);
// Does a disc of radius r around p touch the polygon (inside or within r of an edge)?
bool disc_touches_polygon(Vec2 p, float r, const Vec2 *v, size_t n);
// Does the capsule swept by a disc of radius r along a->b touch the polygon?
bool capsule_touches_polygon(Vec2 a, Vec2 b, float r, const Vec2 *v, size_t n);

} // namespace geo
} // namespace vda
