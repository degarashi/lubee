#include "test.hpp"
#include "../freelist.hpp"
#include "../serialization/freelist.hpp"

namespace lubee {
	namespace test {
		template <class T>
		class Freelist : public Random {
			public:
				using value_t = T;
				using fl_t = ::lubee::Freelist<value_t>;
		};
		using Types = ::testing::Types<uint32_t, int32_t, uint64_t>;
		TYPED_TEST_SUITE(Freelist, Types);

		struct Op {
			enum e {
				Get,
				Put,
				_Num
			};
		};
		template <class F, class RI>
		void RandomOperation(F& f, const RI& ri, const int n) {
			std::vector<typename F::value_t> v;
			for(int i=0 ; i<n ; i++) {
				switch(ri({0, Op::_Num-1})) {
					case Op::Get:
					{
						auto ret = f.get();
						ASSERT_EQ(std::find(v.begin(), v.end(), ret), v.end());
						v.emplace_back(ret);
						break;
					}
					case Op::Put:
					{
						if(v.empty())
							continue;
						const int idx = ri({0, int(v.size()-1)});
						const auto itr = v.begin() + idx;
						f.put(*itr);
						v.erase(itr);
						break;
					}
				}
			}
		}
		TYPED_TEST(Freelist, General) {
			USING(value_t);
			USING(fl_t);
			auto& mt = this->mt();
			using Lm = std::numeric_limits<value_t>;
			fl_t f(Lm::max(), value_t(0));
			RandomOperation(
				f,
				mt.template getUniformF<int>(),
				mt.template getUniform<int>({0,128})
			);
			::lubee::CheckSerialization(f);
		}
	}
}
