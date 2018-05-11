#pragma once
#include <functional>

// 複数のハッシュ値を結合
namespace lubee {
	template <class T>
	void hash_combine_single(std::size_t& seed, const T& v) noexcept {
		// -------------- from boost::combine_hash() --------------
		seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	inline void hash_combine(std::size_t&) noexcept {}
	template <class T0, class... Ts>
	void hash_combine(std::size_t& seed, const T0& t, const Ts&... ts) noexcept {
		hash_combine_single(seed, t);
		hash_combine(seed, ts...);
	}

	template <class... Ts>
	std::size_t hash_combine_implicit(const Ts&... ts) noexcept {
		std::size_t seed = 0;
		hash_combine(seed, ts...);
		return seed;
	}

	template <class Itr>
	void hash_combine_range(std::size_t& seed, Itr itr, const Itr itrE) noexcept {
		while(itr != itrE) {
			hash_combine(seed, *itr);
			++itr;
		}
	}
	template <class Itr>
	std::size_t hash_combine_range_implicit(const Itr itr, const Itr itrE) noexcept {
		std::size_t seed = 0;
		hash_combine_range(seed, itr, itrE);
		return seed;
	}
}
