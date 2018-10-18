#pragma once
#include "meta/check_macro.hpp"

DEF_HASMETHOD(deepCmp)
namespace lubee {
	class DeepCmp {
		private:
			template <class T>
			bool _cmp(const T& t0, const T& t1, std::true_type) const noexcept {
				return t0.deepCmp(t1);
			}
			template <class T>
			bool _cmp(const T& t0, const T& t1, std::false_type) const noexcept {
				return t0 == t1;
			}
		public:
			template <class T>
			bool operator()(const T& t0, const T& t1) const noexcept {
				return _cmp(t0, t1, HasMethod_deepCmp_t<T>());
			}
	};
}
