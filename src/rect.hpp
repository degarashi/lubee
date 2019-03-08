#pragma once
#include "size.hpp"
#include "error.hpp"
#include "point.hpp"
#include "compare.hpp"
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
			using size_value_t = std::conditional_t<
									std::is_integral_v<value_t>,
									std::make_unsigned_t<value_t>,
									value_t
								>;
			using size_type = Size<size_value_t>;
			union {
				struct {
					value_t	x0, x1, y0, y1;
				};
				value_t		ar[4];
			};

			Rect() = default;
			Rect(const Rect& r) = default;
			template <class T2>
			Rect(const Rect<T2>& s):
				x0(s.x0),
				x1(s.x1),
				y0(s.y0),
				y1(s.y1)
			{}
			Rect(const value_t& x_0, const value_t& x_1, const value_t& y_0, const value_t& y_1):
				x0(x_0), x1(x_1), y0(y_0), y1(y_1)
			{
				D_Assert0(_checkValidness());
			}
			Rect(const value_t& x_0, const value_t& y_0, const size_type& size):
				x0(x_0),
				x1(x_0 + size.width),
				y0(y_0),
				y1(y_0 + size.height)
			{}
			Rect(const Point<T>& pos, const size_type& size):
				Rect(pos.x, pos.y, size)
			{}
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
			void setWidth(const value_t& w) {
				x1 = x0 + w;
			}
			void setHeight(const value_t& h) {
				y1 = y0 + h;
			}
			void setOffset(const Point<T> &pos) {
				const auto w = width(),
							h = height();
				x0 = pos.x;
				y0 = pos.y;
				setWidth(w);
				setHeight(h);
			}
			void setSize(const size_type& s) {
				x1 = x0 + s.width;
				y1 = y0 + s.height;
			}
			Point<value_t> offset() const {
				return Point<value_t>(x0, y0);
			}
			Point<value_t> topLeft() const {
				return offset();
			}
			Point<value_t> topRight() const {
				return {x1, y0};
			}
			Point<value_t> bottomLeft() const {
				return {x0, y1};
			}
			Point<value_t> bottomRight() const {
				return {x1, y1};
			}
			size_type size() const {
				return {width(), height()};
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
					return *this op size_type{v.x, v.y}; \
				} \
				Rect operator op (const size_type& s) const { \
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
			Rect clip(const Rect& r) const {
				return {
					Saturate(x0, r.x0, r.x1),
					Saturate(x1, r.x0, r.x1),
					Saturate(y0, r.y0, r.y1),
					Saturate(y1, r.y0, r.y1)
				};
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
			template <class Ar, class T2>
			friend void serialize(Ar&, Rect<T2>&);
	};
	using RectI = Rect<int32_t>;
	using RectF = Rect<float>;
	template <class T>
	std::ostream& operator << (std::ostream& os, const ::lubee::Rect<T>& r) {
		return os << "Rect {x0=" << r.x0 << ", x1=" << r.x1 <<
				", y0=" << r.y0 << ", y1=" << r.y1 << "}";
	}
}
