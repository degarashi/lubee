#pragma once
#include "enable_if.hpp"
#include "constant_t.hpp"
#include <type_traits>
#include <utility>

namespace lubee {
	//! コンパイル時定数で数値のN*p乗を計算
	template <int N, class T, ENABLE_IF((N==0))>
	constexpr T ConstantPow(T /*p*/, const T value=1, IConst<N>* =nullptr) {
		return value;
	}
	template <int N, class T, ENABLE_IF((N<0))>
	constexpr T ConstantPow(const T p, const T value=1, IConst<N>* =nullptr) {
		return ConstantPow(p, value/p, (IConst<N+1>*)nullptr);
	}
	template <int N, class T, ENABLE_IF((N>0))>
	constexpr T ConstantPow(const T p, const T value=1, IConst<N>* =nullptr) {
		return ConstantPow(p, value*p, (IConst<N-1>*)nullptr);
	}
}
