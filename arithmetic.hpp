#pragma once

namespace lubee {
	inline constexpr auto Add() {
		return 0;
	}
	template <class T, class... Ts>
	inline constexpr auto Add(const T& t0, const Ts&... ts) {
		return t0 + Add(ts...);
	}

	inline constexpr auto And_A() {
		return ~0;
	}
	template <class T, class... Ts>
	inline constexpr auto And_A(const T& t0, const Ts&... ts) {
		return t0 & And_A(ts...);
	}

	inline constexpr auto Or_A() {
		return 0;
	}
	template <class T0, class... Ts>
	inline constexpr auto Or_A(const T0& t0, const Ts&... ts) {
		return t0 | Or_A(ts...);
	}

	//! 引数のペアを全てXORしたものをORする
	inline constexpr auto X_OrArgs() {
		return 0;
	}
	template <class T, class... Args>
	inline constexpr T X_OrArgs(const T& t0, const T& t1, Args... args) {
		return (t0^t1) | X_OrArgs(args...);
	}
}
