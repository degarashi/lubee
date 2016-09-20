#pragma once
#include <type_traits>

namespace lubee {
	template <class T>
	using is_number = std::integral_constant<bool, std::is_integral<T>{} || std::is_floating_point<T>{}>;
}
