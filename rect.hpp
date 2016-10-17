#pragma once
#include "size.hpp"
#include "meta/check_macro.hpp"
#include "meta/countof.hpp"

namespace lubee {
	DEF_HASMETHOD(x)
	DEF_HASMETHOD(y)
	//! 任意の型の2D矩形
	template <class T>
	class Rect {
		private:
			bool _checkValidness() const noexcept {
				return x1>=x0 && y1>=y0;
			}
		public:
			using value_t = T;
			union {
				struct {
					value_t	x0, x1, y0, y1;
				};
				value_t		ar[4];
			};
	
			Rect() = default;
			Rect(const Rect& r) = default;
			Rect(const value_t& x_0, const value_t& x_1, const value_t& y_0, const value_t& y_1):
				x0(x_0), x1(x_1), y0(y_0), y1(y_1)
			{
				D_Assert0(_checkValidness());
			}
			static Rect FromSize(const value_t& x0, const value_t& y0, const value_t& w, const value_t& h) {
				return Rect(x0, x0+w, y0, y0+h);
			}
			value_t width() const {
				D_Assert0(_checkValidness());
				return x1-x0;
			}
			value_t height() const {
				D_Assert0(_checkValidness());
				return y1-y0;
			}
			void checkValidness() const {
				Assert0(_checkValidness());
			}
			void addOffset(const value_t& x, const value_t& y) {
				D_Assert0(_checkValidness());
				x0 += x;
				x1 += x;
				y0 += y;
				y1 += y;
			}
			bool hit(const Rect& r) const {
				D_Assert0(_checkValidness());
				return !(
					r.x1 <= x0 ||
					r.x0 >= x1 ||
					r.y1 <= y0 ||
					r.y0 >= y1
				);
			}
			//! 面積
			value_t area() const {
				return width() * height();
			}
			Rect move(const value_t& dx, const value_t& dy) const {
				return
					Rect(
						x0 + dx,
						x1 + dx,
						y0 + dy,
						y1 + dy
					);
			}
			void shrinkRight(const value_t& s) {
				x1 = std::max(x0, x1-s);
			}
			void shrinkBottom(const value_t& s) {
				y1 = std::max(y0, y1-s);
			}
			void shrinkLeft(const value_t& s) {
				x0 = std::min(x1, x0+s);
			}
			void shrinkTop(const value_t& s) {
				y0 = std::min(y1, y0+s);
			}
			Size<value_t> size() const {
				return Size<value_t>(width(), height());
			}
			#define DEF_OP(op) \
				template <class V> \
				Rect& operator op##= (const V& t) { \
					return *this = *this op t; } \
				Rect operator op (const value_t& t) const { \
					Rect r; \
					for(int i=0 ; i<int(countof(ar)) ; i++) \
						r.ar[i] = ar[i] op t; \
					return r; \
				} \
				template <class V, ENABLE_IF((HasMethod_x_t<V>{} && HasMethod_y_t<V>{}))> \
				Rect operator op (const V& v) const { \
					return *this op Size<value_t>{v.x, v.y}; \
				} \
				template <class V> \
				Rect operator op (const Size<V>& s) const { \
					return {x0 op s.width, x1 op s.width, \
							y0 op s.height, y1 op s.height}; \
				}
			DEF_OP(+)
			DEF_OP(-)
			DEF_OP(*)
			DEF_OP(/)
			#undef DEF_OP
	
			//! 領域の拡大縮小
			/*!
				負数で領域の縮小
				\return		縮小によって面積が0になった場合はtrue, それ以外はfalse
			*/
			bool expand(const value_t& w, const value_t& h) {
				bool ret = false;
				x0 -= w;
				x1 += w;
				y0 -= h;
				y1 += h;
				if(x0 >= x1) {
					auto d = x0 - x1;
					x1 += d/2;
					x0 = x1;
					ret = true;
				}
				if(y0 >= y1) {
					auto d = y0 - y1;
					y1 += d/2;
					y0 = y1;
					ret = true;
				}
				return ret;
			}
			bool expand(const value_t& w) {
				return expand(w,w);
			}
			template <class V>
			auto toRect() const {
				return Rect<V>(x0, x1, y0,y1);
			}
			bool operator == (const Rect& r) const {
				return x0 == r.x0 &&
						x1 == r.x1 &&
						y0 == r.y0 &&
						y1 == r.y1;
			}
			bool operator != (const Rect& r) const {
				return !(operator == (r));
			}
			//! 重なっている領域を算出
			std::pair<bool, Rect> cross(const Rect& r) const {
				if(hit(r)) {
					return std::make_pair(true,
							Rect(
								std::max(x0, r.x0),
								std::min(x1, r.x1),
								std::max(y0, r.y0),
								std::min(y1, r.y1)
							));
				}
				return std::make_pair(false, Rect());
			}
			template <class Ar>
			void serialize(Ar& ar) {
				ar(
					CEREAL_NVP(x0),
					CEREAL_NVP(x1),
					CEREAL_NVP(y0),
					CEREAL_NVP(y1)
				);
			}
	};
	using RectI = Rect<int32_t>;
	using RectF = Rect<float>;
	template <class T>
	std::ostream& operator << (std::ostream& os, const ::lubee::Rect<T>& r) {
		return os << "Rect {x0=" << r.x0 << ", x1=" << r.x1 <<
				", y0=" << r.y0 << ", y1=" << r.y1 << "}";
	}

	//! 値の低い方に補正
	template <class T>
	struct PP_Lower {
		static T proc(const T& t) {
			T ret = bit::LowClear(t);
			return std::max(T(1), ret);
		}
	};
	//! 値の高い方に補正
	template <class T>
	struct PP_Higher {
		static T proc(const T& t) {
			T ret = bit::LowClear(t);
			if(t & ~ret)
				ret <<= 1;
			return std::max(T(1), ret);
		}
	};

	//! 2の乗数しか代入できない型
	/*! 0も許可されない */
	template <class T, template <class> class Policy>
	class PowValue {
		public:
			using value_t = T;
		private:
			using P = Policy<value_t>;
			value_t		_value;

		public:
			template <class Ar>
			void serialize(Ar& ar) {
				ar(CEREAL_NVP(_value));
			}
			//TODO: テンプレートによる静的な値補正
			PowValue(const value_t& t):
				_value(P::proc(t))
			{}
			PowValue(const PowValue& p) = default;

			PowValue& operator = (const PowValue& p) = default;
			PowValue& operator = (const value_t& t) {
				// 2の乗数に補正 (動的なチェック)
				_value = P::proc(t);
				return *this;
			}
			const value_t& get() const {
				return _value;
			}
			operator const value_t& () const {
				return _value;
			}
	};
	using PowInt = PowValue<uint32_t, PP_Higher>;
	using PowSize = Size<PowInt>;
	template <class... Ts>
	std::ostream& operator << (std::ostream& os, const ::lubee::PowValue<Ts...>& p) {
		return os << p.get();
	}
}
