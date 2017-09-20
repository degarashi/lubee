#pragma once
#include "../assoc.hpp"
#include <cereal/types/vector.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/base_class.hpp>

namespace lubee {
	template <class Ar, class T2, class Pred2, class CT2>
	void serialize(Ar& ar, AssocVec<T2,Pred2,CT2>& a) {
		ar(cereal::make_nvp("vec", a._vec));
	}
	template <class Ar, class K2, class T2, class Pred2, class CT2>
	void serialize(Ar& ar, AssocVecK<K2,T2,Pred2,CT2>& a) {
		ar(cereal::base_class<typename std::decay_t<decltype(a)>::ASV>(&a));
	}
}
