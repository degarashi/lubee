#pragma once
#include "hash_combine.hpp"

namespace lubee {
	template <class T>
	struct Point {
		using value_t = T;
		value_t		x, y;

		Point() = default;
		Point(const value_t& x, const value_t& y):
			x(x),
			y(y)
		{}

		bool operator == (const Point& p) const noexcept {
			return x == p.x &&
					y == p.y;
		}
		bool operator != (const Point& p) const noexcept {
			return !(this->operator == (p));
		}
		#define DEF_OP(op) \
			Point operator op (const Point& p) const { \
				return {x op p.x, y op p.y}; } \
			Point& operator op##= (const Point& p) { \
				x op##= p.x; \
				y op##= p.y; \
				return *this; }
		DEF_OP(+)
		DEF_OP(-)
		#undef DEF_OP
		#define DEF_OP(op) \
			Point operator op (const value_t t) const { \
				return {x op t, y op t}; } \
			Point& operator op##= (const value_t t) { \
				x op##= t; \
				y op##= t; \
				return *this; }
		DEF_OP(*)
		DEF_OP(/)
		#undef DEF_OP
	};
	using PointI = Point<int32_t>;
	using PointF = Point<float>;
}
namespace std {
	template <class T>
	struct hash<lubee::Point<T>> {
		std::size_t operator()(const lubee::Point<T>& p) const noexcept {
			return lubee::hash_combine_implicit(p.x, p.y);
		}
	};
}
