#include "test.hpp"
#include "assoc.hpp"
#include "../serialization/assoc.hpp"

namespace lubee {
	namespace test {
		template <class A, class RD>
		void TestAssoc(RD& rd) {
			A	asv;
			const auto rdF = [&rd](){
				return rd.template getUniform<int>();
			};
			const auto rdR = [&rd](const int from, const int to){
				return rd.template getUniform<int>({from,to});
			};

			// 適当な数の要素を追加(最低1回)
			::lubee::test::
			AddRandom((rdF() & 0xf00) + 1, rdF, asv);
			// 順番確認
			ASSERT_NO_FATAL_FAILURE(ChkSequence(asv, nullptr));
			// 適当なインデックスの要素を削除
			RemRandom(rdR(0, asv.size()-1), rdR, asv);
			ASSERT_NO_FATAL_FAILURE(ChkSequence(asv, nullptr));
		}

		template <class T>
		struct AssocVec : Random {
			template <class T2, class P>
			using A = ::lubee::AssocVec<T2,P>;
			template <class K, class T2, class P>
			using AK = ::lubee::AssocVecK<K,T2,P>;
		};
		using AsvT = ::testing::Types<std::less<>, std::greater<>>;
		TYPED_TEST_SUITE(AssocVec, AsvT);
		TYPED_TEST(AssocVec, WithoutKey) {
			auto& mt = this->mt();
			TestAssoc<typename TestFixture::template A<int,TypeParam>>(mt);
		}
		TYPED_TEST(AssocVec, WithKey) {
			auto& mt = this->mt();
			TestAssoc<typename TestFixture::template AK<int,int,TypeParam>>(mt);
		}

		TYPED_TEST(AssocVec, Modify_WithoutKey) {
			auto& mt = this->mt();
			const auto rdF = [&mt](){
				return mt.template getUniformMin<int>(0);
			};
			typename TestFixture::template A<int, TypeParam> asv;
			// 適当な数の要素を追加
			AddRandom(rdF() % 0xf00, rdF, asv);
			// 何箇所か適当に値を改変
			int n = rdF() % 0x10;
			auto* ptr = const_cast<int*>(&asv.front());
			while(n-- != 0) {
				const int pos = rdF() % asv.size();
				ptr[pos] ^= 0xf0f0f0f0;
			}
			// 再度ソートをかける
			asv.re_sort();
			// 順番確認
			ASSERT_NO_FATAL_FAILURE(ChkSequence(asv, nullptr));
		}

		struct AssocVecSerialize : Random {};
		TEST_F(AssocVecSerialize, WithoutKey) {
			// ランダムなデータ列を作る
			auto& mt = this->mt();
			const auto rdF = [&mt](){
				return mt.getUniform<int>();
			};
			::lubee::AssocVec<int, std::less<>> data;
			// 適当な数の要素を追加(最低1回)
			AddRandom((rdF() & 0xf00) + 1, rdF, data);
			::lubee::CheckSerialization(data);
		}
		TEST_F(AssocVecSerialize, WithKey) {
			// ランダムなデータ列を作る
			auto& mt = this->mt();
			const auto rdF = [&mt](){
				return mt.getUniform<int>();
			};
			::lubee::AssocVecK<int, int, std::less<>> data;
			// 適当な数の要素を追加(最低1回)
			AddRandom((rdF() & 0xf00) + 1, rdF, data);
			::lubee::CheckSerialization(data);
		}
	}
}

