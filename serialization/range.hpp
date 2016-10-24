#pragma once
#include "../range.hpp"
#include <cereal/access.hpp>

namespace lubee {
	template <class Ar, class T>
	void serialize(Ar& ar, Range<T>& r) {
		ar(
			cereal::make_nvp("from", r.from),
			cereal::make_nvp("to", r.to)
		);
	}
}
