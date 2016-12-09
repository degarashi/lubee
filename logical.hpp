#pragma once

namespace lubee {
	template <class T>
	inline constexpr T And_A() {
		return T(0);
	}
	template <class T, class... Ts>
	inline constexpr auto And_A(const T& t0, const Ts&... ts) {
		return t0 & And_A<T>(ts...);
	}
	inline constexpr bool And_L() {
		return true;
	}
	template <class... Ts>
	inline constexpr bool And_L(const bool b0, const Ts&... ts) {
		return b0 && And_L(ts...);
	}

	template <class T>
	inline constexpr auto Or() {
		return T(0);
	}
	template <class T0, class... Ts>
	inline constexpr auto Or(const T0& t0, const Ts&... ts) {
		return t0 | Or(ts...);
	}
	inline constexpr auto Or_L() {
		return false;
	}
	template <class... Ts>
	inline constexpr auto Or_L(const bool b0, const Ts&... ts) {
		return b0 || Or_L(ts...);
	}

	//! 引数のペアを全てXORしたものをORする
	inline constexpr int X_OrArgs() {
		return 0;
	}
	template <class T, class... Args>
	inline constexpr T X_OrArgs(const T& t0, const T& t1, Args... args) {
		return (t0^t1) | X_OrArgs(args...);
	}
}
