#pragma once
#include "../random.hpp"
#include "../check_serialization.hpp"

namespace lubee {
	namespace test {
		class Random : public ::testing::Test {
			private:
				RandomMT	_mt;
			public:
				Random(): _mt(RandomMT::Make<4>()) {}
				auto& mt() noexcept {
					return _mt;
				}
		};
		#define USING(t) using t = typename TestFixture::t
	}
}
