#include "test.hpp"
#include "../sort.hpp"

namespace lubee {
	namespace test {
		template <class T>
		struct Sort : Random {
			using value_t = T;
		};
		using SortT = ::testing::Types<int, double>;
		TYPED_TEST_CASE(Sort, SortT);
		TYPED_TEST(Sort, Insertion) {
			auto& mt = this->mt();
			// 適当にランダム配列を生成
			std::vector<TypeParam> v[2];
			const int n = mt.template getUniform<int>({0, 100});
			for(int i=0 ; i<n ; i++) {
				v[0].push_back(mt.template getUniform<TypeParam>());
			}
			// もう片方にコピー
			v[1] = v[0];
			// 一方はstdのソート
			std::sort(v[0].begin(), v[0].end());
			// もう片方はlubee::insertion_sort
			insertion_sort(
				v[1].begin(),
				v[1].end(),
				[](const auto& v0, const auto& v1){ return v0 < v1; }
			);
			// 結果は同じになる
			ASSERT_EQ(v[0], v[1]);
		}
		TYPED_TEST(Sort, Stable) {
			auto& mt = this->mt();
			// 適当にランダム配列を生成
			std::vector<std::pair<int, TypeParam>> v[2];
			const int n = mt.template getUniform<int>({2, 100});
			const auto val = mt.template getUniform<TypeParam>();
			for(int i=0 ; i<n ; i++) {
				v[0].emplace_back(val, mt.template getUniform<int>());
			}
			// もう片方にコピー
			v[1] = v[0];
			lubee::insertion_sort(
				v[1].begin(),
				v[1].end(),
				[](const auto& v0, const auto& v1){
					return v0.first < v1.first;
				}
			);
			// ソート操作をしても順番は変わらない
			ASSERT_EQ(v[0], v[1]);
		}
	}
}
