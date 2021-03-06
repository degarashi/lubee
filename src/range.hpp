#pragma once
#include "error.hpp"

namespace lubee {
	template <class T>
	struct Range {
		using value_t = T;
		value_t	from,
				to;

		constexpr static bool ExCmp = noexcept(value_t()>value_t(),
											value_t()==value_t()),
								ExEq = noexcept(from=to,
												from+=to),
								is_integral = static_cast<bool>(std::is_integral<T>{});
		Range() = default;
		template <class T2>
		constexpr Range(const Range<T2>& r) noexcept(ExEq):
			Range(static_cast<T>(r.from), static_cast<T>(r.to)) {}
		explicit constexpr Range(const value_t& f) noexcept(ExEq):
			Range(-f, f)
		{
			D_Expect(f>=0, "invalid range");
		}
		constexpr Range(const value_t& f, const value_t& t) noexcept(ExEq):
			from(f),
			to(t)
		{
			D_Expect(from <= to, "invalid range");
		}
		//! 値の範囲チェック
		bool hit(const value_t& v) const noexcept(ExCmp) {
			return v >= from
				&& v <= to;
		}
		//! 範囲が重なっているか
		bool hit(const Range& r) const noexcept(ExCmp) {
			return !(to < r.from || from > r.to);
		}
		bool operator == (const Range& r) const noexcept(ExCmp) {
			return from == r.from
				&& to == r.to;
		}
		bool operator != (const Range& r) const noexcept(ExCmp) {
			return !(this->operator == (r));
		}
		#define DEF_OP(op) \
			Range& operator op##= (const value_t& t) noexcept(ExEq) { \
				from op##= t; \
				to op##= t; \
				return *this; \
			} \
			Range operator op (const value_t& t) const noexcept(ExEq) { \
				return Range(from op t, to op t); \
			}
		DEF_OP(+)
		DEF_OP(-)
		DEF_OP(*)
		DEF_OP(/)
		#undef DEF_OP

		template <class Ar, class T2>
		friend void serialize(Ar&, Range<T2>&);
	};
	using RangeI = Range<int_fast32_t>;
	using RangeF = Range<float>;
}
