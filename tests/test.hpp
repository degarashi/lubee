#pragma once
#include "../random.hpp"
#include "../check_serialization.hpp"
#include "../output.hpp"
#include <fstream>
#include <chrono>

namespace lubee {
	namespace test {
		class Random : public ::testing::Test {
			private:
				using seed_t = RandomMT::result_type;
				seed_t		_seed;
				RandomMT	_mt;
			public:
				Random():
					_mt(0)
				{
					{
						bool bLoaded = false;
						// シードファイルがあったら読み込む(とりあえずファイル名は決め打ち)
						std::ifstream f("/tmp/debug_seed");
						if(f.is_open()) {
							f >> _seed;
							bLoaded = f.good();
						}
						if(bLoaded)
							Log(Verbose, "Loaded Random-seed: %d", _seed);
						else {
							_seed = std::chrono::system_clock::now().time_since_epoch().count();
							Log(Verbose, "Generated Random-seed: %d", _seed);
						}
					}
					RandomMT mt(_seed);
					std::swap(mt, _mt);
				}
				auto& mt() noexcept {
					return _mt;
				}
		};
		#define USING(t) using t = typename TestFixture::t
	}
}
