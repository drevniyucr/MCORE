/*
 * geometry.cpp — see geometry.hpp.
 */
#include <cmath>

#include "vda5050/geometry.hpp"

namespace vda {
namespace geo {

float dist(Vec2 a, Vec2 b) {
	const float dx = a.x - b.x, dy = a.y - b.y;
	return std::sqrt(dx * dx + dy * dy);
}

float dist_point_segment(Vec2 p, Vec2 a, Vec2 b) {
	const float dx = b.x - a.x, dy = b.y - a.y;
	const float l2 = dx * dx + dy * dy;
	float t = l2 > 0.0F ? ((p.x - a.x) * dx + (p.y - a.y) * dy) / l2 : 0.0F;
	t = t < 0.0F ? 0.0F : (t > 1.0F ? 1.0F : t);
	return dist(p, Vec2{ a.x + t * dx, a.y + t * dy });
}

// --- NURBS ---------------------------------------------------------------------
bool nurbs_prepare(Nurbs &n) {
	if (n.degree < 1 || n.degree > 7 || n.ncp < n.degree + 1U || n.ncp > MAX_CONTROL_POINTS) {
		return false;
	}
	for (uint8_t i = 0; i < n.ncp; ++i) {
		if (!(n.w[i] > 0.0F)) return false;
	}
	const uint8_t want = static_cast<uint8_t>(n.ncp + n.degree + 1U);
	if (n.nknots == 0) {
		// clamped, uniform interior knots
		const uint8_t interior = static_cast<uint8_t>(n.ncp - n.degree - 1U);
		for (uint8_t i = 0; i < want; ++i) {
			if (i <= n.degree) n.knots[i] = 0.0F;
			else if (i >= want - n.degree - 1U) n.knots[i] = 1.0F;
			else n.knots[i] = static_cast<float>(i - n.degree) / static_cast<float>(interior + 1U);
		}
		n.nknots = want;
		return true;
	}
	if (n.nknots != want) return false;
	for (uint8_t i = 0; i < n.nknots; ++i) {
		if (n.knots[i] < 0.0F || n.knots[i] > 1.0F) return false;
		if (i > 0 && n.knots[i] < n.knots[i - 1]) return false;
	}
	// clamped: first and last knot repeated degree + 1 times
	for (uint8_t i = 1; i <= n.degree; ++i) {
		if (n.knots[i] != n.knots[0] || n.knots[n.nknots - 1U - i] != n.knots[n.nknots - 1U]) return false;
	}
	return n.knots[n.nknots - 1U] > n.knots[0];
}

Vec2 nurbs_eval(const Nurbs &n, float u) {
	const uint8_t p = n.degree;
	const float u0 = n.knots[p];
	const float u1 = n.knots[n.ncp];
	u = u0 + (u1 - u0) * (u < 0.0F ? 0.0F : (u > 1.0F ? 1.0F : u));
	// knot span k with knots[k] <= u < knots[k+1], k in [p, ncp-1]
	uint8_t k = p;
	while (k + 1U < n.ncp && u >= n.knots[k + 1U]) ++k;
	// de Boor on homogeneous coordinates
	float hx[8], hy[8], hw[8];
	for (uint8_t j = 0; j <= p; ++j) {
		const uint8_t i = static_cast<uint8_t>(k - p + j);
		hw[j] = n.w[i];
		hx[j] = n.cp[i].x * n.w[i];
		hy[j] = n.cp[i].y * n.w[i];
	}
	for (uint8_t r = 1; r <= p; ++r) {
		for (uint8_t j = p; j >= r; --j) {
			const uint8_t i = static_cast<uint8_t>(k - p + j);
			const float den = n.knots[i + p + 1U - r] - n.knots[i];
			const float a = den > 0.0F ? (u - n.knots[i]) / den : 0.0F;
			hx[j] = (1.0F - a) * hx[j - 1] + a * hx[j];
			hy[j] = (1.0F - a) * hy[j - 1] + a * hy[j];
			hw[j] = (1.0F - a) * hw[j - 1] + a * hw[j];
		}
	}
	return Vec2{ hx[p] / hw[p], hy[p] / hw[p] };
}

size_t nurbs_sample(const Nurbs &n, float spacing, Vec2 *out, size_t max) {
	if (max < 2) return 0;
	// Evaluate finely, keep a point whenever we are `spacing` away from the
	// last kept one (overshoot <= one fine step).
	constexpr int FINE = 400;
	size_t count = 0;
	out[count++] = nurbs_eval(n, 0.0F);
	for (int i = 1; i < FINE && count + 1 < max; ++i) {
		const Vec2 q = nurbs_eval(n, static_cast<float>(i) / FINE);
		if (dist(out[count - 1], q) >= spacing) {
			out[count++] = q;
		}
	}
	const Vec2 end = nurbs_eval(n, 1.0F);
	if (count > 1 && dist(out[count - 1], end) < 0.3F * spacing) {
		out[count - 1] = end;       // avoid a tiny last step
	} else {
		out[count++] = end;
	}
	return count;
}

size_t line_sample(Vec2 a, Vec2 b, float spacing, Vec2 *out, size_t max) {
	const float d = dist(a, b);
	size_t steps = static_cast<size_t>(std::ceil(d / spacing));
	if (steps < 1) steps = 1;
	if (steps > max) steps = max;
	for (size_t i = 1; i <= steps; ++i) {
		const float t = static_cast<float>(i) / static_cast<float>(steps);
		out[i - 1] = Vec2{ a.x + t * (b.x - a.x), a.y + t * (b.y - a.y) };
	}
	return steps;
}

// --- polygons ----------------------------------------------------------------------
bool point_in_polygon(Vec2 p, const Vec2 *v, size_t n) {
	bool in = false;
	for (size_t i = 0, j = n - 1; i < n; j = i++) {
		if (((v[i].y > p.y) != (v[j].y > p.y)) &&
		    (p.x < (v[j].x - v[i].x) * (p.y - v[i].y) / (v[j].y - v[i].y) + v[i].x)) {
			in = !in;
		}
	}
	return in;
}

float dist_point_polygon_edges(Vec2 p, const Vec2 *v, size_t n) {
	float best = 1e30F;
	for (size_t i = 0, j = n - 1; i < n; j = i++) {
		const float d = dist_point_segment(p, v[j], v[i]);
		if (d < best) best = d;
	}
	return best;
}

bool disc_touches_polygon(Vec2 p, float r, const Vec2 *v, size_t n) {
	return point_in_polygon(p, v, n) || dist_point_polygon_edges(p, v, n) <= r;
}

namespace {
float cross(Vec2 o, Vec2 a, Vec2 b) {
	return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
}
bool segments_intersect(Vec2 a, Vec2 b, Vec2 c, Vec2 d) {
	const float d1 = cross(c, d, a), d2 = cross(c, d, b);
	const float d3 = cross(a, b, c), d4 = cross(a, b, d);
	return ((d1 > 0) != (d2 > 0)) && ((d3 > 0) != (d4 > 0));
}
} // namespace

bool capsule_touches_polygon(Vec2 a, Vec2 b, float r, const Vec2 *v, size_t n) {
	if (point_in_polygon(a, v, n) || point_in_polygon(b, v, n)) return true;
	for (size_t i = 0, j = n - 1; i < n; j = i++) {
		if (segments_intersect(a, b, v[j], v[i])) return true;
		if (dist_point_segment(v[i], a, b) <= r) return true;
		if (dist_point_segment(a, v[j], v[i]) <= r || dist_point_segment(b, v[j], v[i]) <= r) return true;
	}
	return false;
}

} // namespace geo
} // namespace vda
