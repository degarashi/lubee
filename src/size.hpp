#pragma once
#include "meta/enable_if.hpp"
#include "bit.hpp"
#include "error.hpp"
#include <cmath>
#include <ostream>

namespace lubee {
	//! 任意の型の縦横サイズ
	template <class T>
	struct Size {
		using value_t = T;
		value_t		width,
					height;

		constexpr static bool ExCmp = noexcept(std::declval<value_t>() < std::declval<value_t>(),
											std::declval<value_t>() == std::declval<value_t>()),
								ExEq = noexcept(noexcept(std::declval<value_t&>()=std::declval<value_t>()) &&
												noexcept(std::declval<value_t&>()+=std::declval<value_t>())),
								is_integral = std::is_integral<T>{};
		Size() = default;
		Size(const T& s) noexcept(ExEq):
			width(s),
			height(s)
		{
			D_Assert0(_checkValidness());
		}
		Size(const T& w, const T& h) noexcept(ExEq):
			width(w),
			height(h)
		{
			D_Assert0(_checkValidness());
		}
		template <class T2>
		Size(const Size<T2>& s) noexcept(ExEq):
			width(static_cast<value_t>(s.width)),
			height(static_cast<value_t>(s.height))
		{
			D_Assert0(_checkValidness());
		}
		bool _checkValidness() const noexcept {
			return width>=0 && height>=0;
		}
		template <class T2>
		Size& operator *= (const T2& s) {
			return *this = *this * s;
		}
		template <class T2>
		Size& operator /= (const T2& s) {
			return *this = *this / s;
		}
		template <class T2>
		Size operator / (const T2& s) const {
			return Size(width/s, height/s);
		}
		template <class T2>
		Size operator * (const T2& s) const {
			return Size(width*s, height*s);
		}
		void shiftR(const size_t n) noexcept(ExEq) {
			if constexpr (is_integral) {
				width >>= n;
				height >>= n;
			} else {
				*this /= std::pow(2, n);
			}
		}
		Size& operator >>= (const size_t n) noexcept(ExEq) {
			shiftR(n);
			return *this;
		}
		//! 指定したビット数分、右シフトしてもし値がゼロになったら1をセットする
		void shiftR_one(const size_t n) noexcept(ExEq) {
			if constexpr (is_integral) {
				width >>= n;
				width |= ~bit::ZeroOrFull(width) & 0x01;
				height >>= n;
				height |= ~bit::ZeroOrFull(height) & 0x01;
			} else {
				// 効率は微妙だが、とりあえず
				shiftR(n);
				width = std::max<value_t>(1, width);
				height = std::max<value_t>(1, height);
			}
		}
		//! 1右ビットシフトした値を2の累乗に合わせる
		void shiftR_2pow() noexcept(ExEq) {
			if constexpr (is_integral) {
				const auto fn = [](const value_t& t) -> value_t {
					using UT = std::make_unsigned_t<T>;
					const UT ut(t);
					const auto v = bit::LowClear(ut);
					if(v != ut)
						return v;
					return v >> 1;
				};
				width = fn(width);
				height = fn(height);
			} else {
				// 一旦, 整数に直してから演算
				Size<uint64_t> tmp(
					std::floor(width),
					std::floor(height)
				);
				tmp.shiftR_2pow();

				// 値をthisに戻す
				width = value_t(tmp.width);
				height = value_t(tmp.height);
			}
		}
		bool operator == (const Size& s) const noexcept(ExCmp) {
			return width == s.width &&
					height == s.height;
		}
		bool operator != (const Size& s) const noexcept(ExCmp) {
			return !(this->operator == (s));
		}
		bool operator <= (const Size& s) const noexcept(ExCmp) {
			return width <= s.width &&
					height <= s.height;
		}
		//! 領域の拡大縮小
		/*!
			負数で領域の縮小
			\return		縮小によって面積が0になった場合はtrue, それ以外はfalse
		*/
		bool expand(const value_t& w, const value_t& h) noexcept(ExEq) {
			bool ret = false;
			width += w;
			height += h;
			if(width <= 0) {
				width = 0;
				ret = true;
			}
			if(height <= 0) {
				height = 0;
				ret = true;
			}
			return ret;
		}
		bool expand(const value_t& w) noexcept(ExEq) {
			return expand(w,w);
		}
		template <class V>
		auto toSize() const noexcept(ExEq) {
			return Size<V>(width, height);
		}
		auto area() const noexcept(ExEq) {
			return width * height;
		}
	};
	template <class T>
	std::ostream& operator << (std::ostream& os, const ::lubee::Size<T>& s) {
		return os << "Size {width=" << s.width << ", height=" << s.height << "}";
	}
	using SizeI = Size<int32_t>;
	using SizeF = Size<float>;
}
