#include "arithmetic.hpp"
#include <cstdint>

namespace lubee {
	namespace {
		constexpr int64_t c[] = {
			100,200,300,400,
			0xdeadbeef, 0xc00feeee, 0x12345678,
			0xf0f0c0c0
		};
		static_assert(Add() == 0, "");
		static_assert(Add(c[0]) == c[0], "");
		static_assert(Add(c[0], c[1], c[2], c[3]) == c[0]+c[1]+c[2]+c[3], "");
		static_assert(Add(c[0], -c[1], c[2], -c[3]) == c[0]-c[1]+c[2]-c[3], "");

		static_assert(And_A() == ~0, "");
		static_assert(And_A(c[4]) == c[4], "");
		static_assert(And_A(c[4], c[5], c[6]) == (c[4]&c[5]&c[6]), "");
		static_assert(And_A(c[5], c[6], c[7]) == (c[5]&c[6]&c[7]), "");

		static_assert(Or_A() == 0, "");
		static_assert(Or_A(c[4]) == c[4], "");
		static_assert(Or_A(c[4], c[5], c[6]) == (c[4]|c[5]|c[6]), "");
		static_assert(Or_A(c[5], c[6], c[7]) == (c[5]|c[6]|c[7]), "");

		static_assert(X_OrArgs() == 0, "");
		static_assert(X_OrArgs(c[0], c[1]) == (c[0]^c[1]), "");
		static_assert(X_OrArgs(c[2], c[3], c[4], c[5]) == Or_A(c[2]^c[3], c[4]^c[5]), "");
		static_assert(X_OrArgs(c[2], c[3], c[4], c[5], c[6], c[7]) == Or_A(c[2]^c[3], c[4]^c[5], c[6]^c[7]), "");
	}
}
