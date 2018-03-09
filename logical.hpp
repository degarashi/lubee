#pragma once

namespace lubee {
	inline constexpr bool And_L() {
		return true;
	}
	template <class... Ts>
	inline constexpr bool And_L(const bool b0, const Ts&... ts) {
		return b0 && And_L(ts...);
	}

	inline constexpr bool Or_L() {
		return false;
	}
	template <class... Ts>
	inline constexpr bool Or_L(const bool b0, const Ts&... ts) {
		return b0 || Or_L(ts...);
	}
}
