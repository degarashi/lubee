#pragma once
#include "meta/countof.hpp"
#include "error.hpp"
#include <vector>
#include <cstdint>

namespace lubee {
	inline double VanDerCorput(uint32_t i, uint32_t base) {
		double h = 0,
			   f = 1.0 / base,
			   factor = f;

		while(i > 0) {
			h += (i%base) * factor;
			i /= base;
			factor *= f;
		}
		return h;
	}
	namespace detail {
		const uint32_t c_primes[] = {2, 3, 5, 7, 9, 11, 13, 17, 19, 23};
		constexpr std::size_t N_Primes = countof(c_primes);

		inline void CheckDim(const std::size_t dim) {
			D_Assert(dim <= N_Primes, "too big dimension");
		}
	}
	inline void Halton(double* dst, const uint32_t i, const std::size_t dim) {
		detail::CheckDim(dim);

		for(std::size_t j=0; j<dim ; j++)
			*dst++ = VanDerCorput(i, detail::c_primes[j]);
	}
	using Double_V = std::vector<double>;
	inline Double_V Hammersley(const std::size_t n, const std::size_t dim) {
		detail::CheckDim(dim);

		Double_V res(n*dim);
		auto* res_p = res.data();
		for(std::size_t i=0; i<n ; i++) {
			*res_p++ = double(i) / n;
			for(std::size_t j=1; j<dim ; j++)
				*res_p++ = VanDerCorput(i, detail::c_primes[j-1]);
		}
		D_Assert0(res_p == res.data() + res.size());
		return res;
	}
}
