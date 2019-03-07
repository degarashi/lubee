#include "test.hpp"
#include "../unionset.hpp"
#include "../recursive_lambda.hpp"

namespace lubee::test {
	struct UnionSet : Random {};
	TEST_F(UnionSet, General) {
		using us_t = ::lubee::UnionSet;
		constexpr size_t N = 8,
						S = N+2;
		constexpr int Empty = 0x00,
						Wall = 0xff;
		using ar_t = std::array<int, S*S>;
		// 二次元のマップをランダムに生成
		// 周囲は1マスの壁(+2)
		ar_t map;
		const auto index = [](const int x, const int y) {
			return y*S + x;
		};
		const auto at = [&map, &index](const int x, const int y) -> decltype(auto) {
			return map[index(x,y)];
		};
		// 周囲の壁の配置
		for(int i=0 ; i<S ; i++) {
			// 上
			at(i, 0) = Wall;
			// 下
			at(i, S-1) = Wall;
			// 左
			at(0, i) = Wall;
			// 右
			at(S-1, i) = Wall;
		}
		// 内部のマスは一定確率(0.25)で壁にする
		{
			auto& mt = this->mt();
			auto& rd = mt.refMt();
			std::bernoulli_distribution bdist(0.25);
			for(int i=1 ; i<S-1 ; i++) {
				for(int j=1 ; j<S-1 ; j++) {
					at(j,i) = bdist(rd) ? Wall : Empty;
				}
			}
		}
		us_t us(S*S);
		// 塗りつぶしアルゴリズムで順番に番号を振っていく
		// 同時にUnionSetも更新
		int num = 0;
		for(int i=1 ; i<S-1 ; i++) {
			for(int j=1 ; j<S-1 ; j++) {
				auto& a = at(j,i);
				if(a == Empty) {
					Recursive([&at, &us, &index](
						auto&& f, const int from_index, const int num,
						const int x, const int y) -> void
						{
							auto& a = at(x,y);
							if(a == Empty) {
								a = num;
								us.merge(from_index, index(x,y));
								f(from_index, num, x-1, y);
								f(from_index, num, x+1, y);
								f(from_index, num, x, y-1);
								f(from_index, num, x, y+1);
							}
						}
					)(index(j,i), ++num, j, i);
				}
			}
		}
		// 全てのマスのペアについてUnionSetで違うペアならば番号も異なる
		for(int i=0 ; i<S ; i++) {
			for(int j=0 ; j<S ; j++) {
				const auto idx0 = index(j, i);
				const auto& a = at(j, i);

				for(int k=0 ; k<S ; k++) {
					for(int l=0 ; l<S ; l++) {
						const auto idx1 = index(l, k);
						if(idx0 == idx1)
							continue;

						const auto& b = at(l, k);
						const bool b0 = us.isSame(idx0, idx1),
									b1 = a == b;
						if(a==Wall && b==Wall) {
							ASSERT_FALSE(b0);
						} else {
							ASSERT_EQ(b0, b1);
						}
					}
				}
			}
		}
	}
}
