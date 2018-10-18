#pragma once
#include "../freelist.hpp"
#include <cereal/access.hpp>
#include <cereal/types/stack.hpp>
#include <cereal/types/deque.hpp>

namespace lubee {
	template <class Ar, class T>
	void serialize(Ar& ar, Freelist<T>& f) {
		ar(
			cereal::make_nvp("max", f._maxV),
			cereal::make_nvp("start", f._startV),
			cereal::make_nvp("cur", f._curV),
			cereal::make_nvp("stack", f._stk)
		);
	}
	template <class T>
	template <class Ar>
	void Freelist<T>::load_and_construct(Ar& ar, cereal::construct<Freelist>& construct) {
		T max, start;
		ar(
			cereal::make_nvp("max", max),
			cereal::make_nvp("start", start)
		);
		construct(max, start);
		ar(
			cereal::make_nvp("cur", construct->_curV),
			cereal::make_nvp("stack", construct->_stk)
		);
	}
}
