#include "../logical.hpp"

namespace lubee {
	namespace {
		static_assert(And_L(), "");
		static_assert(And_L(true, true, true, true, true), "");
		static_assert(!And_L(false, true, true, true, true), "");
		static_assert(!And_L(true, true, false, true, true), "");
		static_assert(!And_L(true, true, false, true, false), "");

		static_assert(!Or_L(), "");
		static_assert(!Or_L(false), "");
		static_assert(Or_L(true, false, false, false, false), "");
		static_assert(Or_L(false, false, false, false, true), "");
		static_assert(Or_L(false, false, true, false, true), "");
		static_assert(Or_L(false, true, false, false, false), "");
		static_assert(Or_L(true, true, true, true, false), "");
	}
}
