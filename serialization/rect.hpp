#pragma once
#include "../rect.hpp"
#include <cereal/access.hpp>

namespace lubee {
	template <class Ar, class T>
	void serialize(Ar& ar, Rect<T>& r) {
		ar(
			cereal::make_nvp("x0", r.x0),
			cereal::make_nvp("x1", r.x1),
			cereal::make_nvp("y0", r.y0),
			cereal::make_nvp("y1", r.y1)
		);
	}

	template <class Ar, class T, template <class> class P>
	void serialize(Ar& ar, PowValue<T,P>& p) {
		ar(cereal::make_nvp("value", p._value));
	}
}
