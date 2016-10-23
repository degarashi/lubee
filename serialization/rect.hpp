#pragma once
#include "../rect.hpp"
#include <cereal/access.hpp>

namespace lubee {
	template <class Ar, class T>
	void serialize(Ar& ar, Rect<T>& r) {
		ar(
			CEREAL_NVP(r.x0),
			CEREAL_NVP(r.x1),
			CEREAL_NVP(r.y0),
			CEREAL_NVP(r.y1)
		);
	}

	template <class Ar, class T, template <class> class P>
	void serialize(Ar& ar, PowValue<T,P>& p) {
		ar(CEREAL_NVP(p._value));
	}
}
