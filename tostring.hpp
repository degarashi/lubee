#pragma once
#include <string>
#include <sstream>

namespace lubee {
	template <class T>
	std::string ToString(const T& t) {
		std::stringstream ss;
		ss << t;
		return ss.str();
	}
}
