#pragma once
#include <cstdint>

namespace lubee {
	// (fpset.hpp)
	struct Fp16;
	struct Fp32;

	// (freelist.hpp)
	template <class T>
	class Freelist;

	// (ieee754.hpp)
	template <class T>
	struct IEEE754;
	struct Half;

	// (random.hpp)
	template <class MT>
	class Random;

	// (range.hpp)
	template <class T>
	struct Range;
	using RangeI = Range<int_fast32_t>;
	using RangeF = Range<float>;

	// (point.hpp)
	template <class T>
	struct Point;
	using PointI = Point<int32_t>;
	using PointF = Point<float>;

	// (size.hpp)
	template <class T>
	class Size;
	using SizeI = Size<uint32_t>;
	using SizeF = Size<float>;

	// (rect.hpp)
	template <class T>
	class Rect;
	using RectI = Rect<int32_t>;
	using RectF = Rect<float>;
	template <class T>
	struct PP_Lower;
	template <class T>
	struct PP_Higher;
	template <class T, template <class> class Policy>
	class PowValue;
	using PowInt = PowValue<uint32_t, PP_Higher>;
	using PowSize = Size<PowInt>;

	// (wrapper.hpp)
	template <class T>
	struct Wrapper;
}
