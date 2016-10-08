#include "test.hpp"
#include "../alignedalloc.hpp"

namespace lubee {
	namespace test {
		template <class T>
		class AlignedAlloc : public Random {
			public:
				using value_t = T;
		};

		template <std::size_t A, class T>
		struct alignas(A) AObj : CheckAlign<T> {
			AObj() = default;
			T	value;

			AObj(const int idx):
				value(idx)
			{}
			bool operator == (const AObj& a) const noexcept {
				return value == a.value;
			}
			bool operator == (const T& a) const noexcept {
				return value == a;
			}
		};
		using Types = ::testing::Types<
							uint8_t,
							uint64_t,
							AObj<1, uint8_t>,
							AObj<4, uint8_t>,
							AObj<32, uint64_t>,
							AObj<64, uint64_t>
						>;
		TYPED_TEST_CASE(AlignedAlloc, Types);

		template <class MT, class CBInit, class CBCheck, class CBFree>
		void Test(MT& mt, const CBInit& cbInit, const CBCheck& cbCheck, const CBFree& cbFree) {
			using Value = decltype(cbInit(0));
			const int N = 64;
			std::vector<std::pair<Value, int>> ar;
			for(int i=0 ; i<N ; i++)
				ar.emplace_back(std::make_pair(cbInit(i), i));
			std::shuffle(ar.begin(), ar.end(), mt);
			// ランダムな順序で解放
			for(auto& a : ar) {
				cbCheck(a.first, a.second);
				cbFree(a.first);
			}
		}
		// 単独でのAlignedAlloc
		TYPED_TEST(AlignedAlloc, Single) {
			USING(value_t);
			using AA = AAllocator<value_t>;
			auto& mt = this->mt().refMt();
			const auto check = [](const auto& p, auto idx){ ASSERT_EQ(*p, idx); };
			const auto noop = [](const auto&){};
			// 生ポインタによるテスト
			ASSERT_NO_FATAL_FAILURE(
				Test(mt, [](auto idx){ return AAlloc<value_t>(alignof(value_t), idx); },
					check, [](const auto& p){ AFree<value_t>(p); })
			);
			// unique_ptr(object)による解放
			ASSERT_NO_FATAL_FAILURE(
				Test(mt, [](auto idx){ return AA::NewU(idx); }, check, noop)
			);
			// unique_ptr(function)による解放
			ASSERT_NO_FATAL_FAILURE(
				Test(mt, [](auto idx){ return AA::NewUF(idx); }, check, noop)
			);
			// shared_ptr(object)による解放
			ASSERT_NO_FATAL_FAILURE(
				Test(mt, [](auto idx){ return AA::NewS(idx); }, check, noop)
			);
		}
		template <class MT, class CBInit, class CBCheck, class CBFree>
		void TestArray(MT& mt, const CBInit& cbInit, const CBCheck& cbCheck, const CBFree& cbFree) {
			using Value = decltype(cbInit(0,0));
			const int N = 64;
			struct Entry {
				Value	value;
				int		index;
				int		n;

				Entry(Value v, const int idx, const int n):
					value(std::move(v)),
					index(idx),
					n(n)
				{}
			};
			std::vector<Entry> ar;
			int idx = 0;
			for(int i=0 ; i<N ; i++) {
				const int n = mt.template getUniform<int>({0, 256});
				ar.emplace_back(cbInit(n, idx), idx, n);
				idx += n;
			}
			std::shuffle(ar.begin(), ar.end(), mt.refMt());
			// ランダムな順序で解放
			for(auto& a : ar) {
				cbCheck(a.value, a.n, a.index);
				cbFree(a.value);
			}
		}
		// 配列でのAlignedAlloc
		TYPED_TEST(AlignedAlloc, Array) {
			USING(value_t);
			using AA = AAllocator<value_t>;
			auto& mt = this->mt();
			const auto check = [](const auto& p, auto n, auto idx){
									using Value = std::decay_t<decltype(p[0])>;
									for(int i=0 ; i<int(n) ; i++) {
										ASSERT_EQ(p[i], Value(idx++));
									}
								};
			const auto noop = [](const auto&){};
			// 生ポインタによるテスト
			ASSERT_NO_FATAL_FAILURE(
				TestArray(mt,
					[](auto n, auto idx){
						auto* ret = AArray<value_t>(alignof(value_t), n);
						for(int i=0 ; i<int(n) ; i++)
							ret[i] = idx++;
						return ret;
					}, check, [](const auto& p){ AArrayFree<value_t>(p); })
			);
			// unique_ptr(object)による解放
			ASSERT_NO_FATAL_FAILURE(
				TestArray(mt,
					[](auto n, auto idx){
						auto ret = AA::ArrayU(n);
						for(int i=0 ; i<int(n) ; i++)
							ret[i] = idx++;
						return std::move(ret);
					}, check, noop
				)
			);
			// unique_ptr(function)による解放
			ASSERT_NO_FATAL_FAILURE(
				TestArray(mt,
					[](auto n, auto idx){
						auto ret = AA::ArrayUF(n);
						for(int i=0 ; i<int(n) ; i++)
							ret[i] = idx++;
						return std::move(ret);
					}, check, noop
				)
			);
			// shared_ptr(object)による解放
			ASSERT_NO_FATAL_FAILURE(
				TestArray(mt,
					[](auto n, auto idx){
						auto ret = AA::ArrayS(n);
						for(int i=0 ; i<int(n) ; i++)
							ret.get()[i] = idx++;
						return std::move(ret);
					},
					[](const auto& p, auto n, auto idx){
						using Value = std::decay_t<decltype(p.get()[0])>;
						for(int i=0 ; i<int(n) ; i++) {
							ASSERT_EQ(p.get()[i], Value(idx++));
						}
					},
					noop
				)
			);
		}
		TYPED_TEST(AlignedAlloc, Allocator) {
			USING(value_t);
			using Vec = std::vector<std::pair<value_t, int>, AlignedPool<value_t>>;
			auto& mt = this->mt();
			Vec v;
			const int N = mt.template getUniform<int>({1, 64});
			for(int i=0 ; i<N ; i++) {
				v.emplace_back(value_t(i), i);
			}
			std::shuffle(v.begin(), v.end(), mt.refMt());
			for(auto& vt : v) {
				ASSERT_EQ(vt.first, vt.second);
			}
		}
	}
}
