#pragma once
#include "enable_if.hpp"
#include "constant_t.hpp"
#include <type_traits>
#include <utility>

namespace lubee {
	namespace {
		namespace _pow {
			using Value = int64_t;
			template <Value N>
			using Const = std::integral_constant<Value, N>;
		}
	}
	//! コンパイル時定数で数値のN*p乗を計算
	template <_pow::Value N, ENABLE_IF((N==_pow::Value(0)))>
	constexpr _pow::Value ConstantPow(_pow::Value /*p*/, const _pow::Value value=1, _pow::Const<N>* =nullptr) {
		return value;
	}
	template <_pow::Value N, ENABLE_IF((N<_pow::Value(0)))>
	constexpr _pow::Value ConstantPow(const _pow::Value p, const _pow::Value value=1, _pow::Const<N>* =nullptr) {
		return ConstantPow(p, value/p, (_pow::Const<N+1>*)nullptr);
	}
	template <_pow::Value N, ENABLE_IF((N>_pow::Value(0)))>
	constexpr _pow::Value ConstantPow(const _pow::Value p, const _pow::Value value=1, _pow::Const<N>* =nullptr) {
		return ConstantPow(p, value*p, (_pow::Const<N-1>*)nullptr);
	}
	namespace _pow {}
}
