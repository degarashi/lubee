#pragma once

namespace lubee {
	//! 引数のペアを全てXORしたものをORする
	inline constexpr int X_OrArgs() {
		return 0;
	}
	template <class T, class... Args>
	inline constexpr T X_OrArgs(const T& t0, const T& t1, Args... args) {
		return (t0^t1) | X_OrArgs(args...);
	}
}
