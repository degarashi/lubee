#pragma once
#include "../range.hpp"
#include <cereal/access.hpp>

namespace lubee {
	template <class Ar, class T>
	void serialize(Ar& ar, Range<T>& r) {
		ar(CEREAL_NVP(r.from), CEREAL_NVP(r.to));
	}
}
