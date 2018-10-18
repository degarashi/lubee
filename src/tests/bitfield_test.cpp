#include "test.hpp"
#include "../meta/constant_t.hpp"
#include "../bitfield.hpp"

namespace lubee {
	namespace test {
		namespace {
			struct Action {
				enum e {
					Set,			// どれかのエントリに値をセット
					Get,			// どれかのエントリから値を取得
					Compare,		// ビット配列を比較
					_Num
				};
			};

			template <class Word, int Cur, int Remain, int W, class... Ts>
			struct _DefineBF {
				using type = typename _DefineBF<
					Word,
					Cur+W,
					Remain-1,
					W,
					Ts...,
					::lubee::BitF<Cur,W>
				>::type;
			};
			template <class Word, int Cur, int W, class... Ts>
			struct _DefineBF<Word, Cur, -1, W, Ts...> {
				using type = ::lubee::BitDef<Word, Ts...>;
			};
			template <class Word, int N, int W>
			using DefineBF = typename _DefineBF<Word, 0, N-1, W>::type;
		}

		template <class Value, class BF, class Idx>
		struct EntryS;
		template <class Value, class BF, std::size_t... Idx>
		struct EntryS<Value, BF, std::index_sequence<Idx...>> {
			struct Entry {
				int		offset;
				Value	bottommask,
						mask;

				template <class BF2>
				constexpr Entry(const BF2&):
					offset(BF2::offset),
					bottommask((1<<BF2::length)-1),
					mask(bottommask << BF2::offset)
				{}
			};
			constexpr static Entry entry[BF::size] = {{typename BF::template At<Idx>()}...};
		};
		template <class Value, class BF, std::size_t... Idx>
		constexpr typename EntryS<Value,BF,std::index_sequence<Idx...>>::Entry
			EntryS<Value,BF,std::index_sequence<Idx...>>::entry[BF::size];

		template <class BF>
		class BitFieldRef {
			public:
				using Value = uint64_t;
			private:
				using BDef = BF;
				Value		_value;
				constexpr static Value mask_1{Value(1<<(BF::template MaxBit<>-1))-1},
										mask{Value(mask_1 | mask_1<<1)};
				using ES = EntryS<Value, BDef, std::decay_t<decltype(std::make_index_sequence<BDef::size>())>>;

				using BF_t = ::lubee::BitField<BF>;
				using ChkFunc = void (*)(const BF_t&, const BitFieldRef&);
				using SetFunc = void (*)(BF_t&, BitFieldRef&, Value);

			public:
				static ChkFunc s_chkfunc[BF::size];
				static SetFunc s_setfunc[BF::size];
			private:
				template <int Idx>
				static void _CheckAt(const BF_t& bf, const BitFieldRef& bfr) {
					ASSERT_EQ(bfr.get<Idx>(), static_cast<Value>(bf.template at<Idx>()));
					// Mask()のテスト
					auto tmp = bf;
					tmp.value() &= BF_t::template Mask<Idx>();
					ASSERT_EQ(bf.template at<Idx>(), tmp.template at<Idx>());
				}
				template <int Idx>
				static void _SetAt(BF_t& bf, BitFieldRef& bfr, const Value val) {
					bf.template at<Idx>() = val;
					bfr.set<Idx>(val);
				}

				static void _Init(IConst<BF::size>) {}
				template <int Cur>
				static void _Init(IConst<Cur>) {
					s_chkfunc[Cur] = &_CheckAt<Cur>;
					s_setfunc[Cur] = &_SetAt<Cur>;
					_Init(IConst<Cur+1>());
				}
				static void _Init() {
					if(!s_chkfunc[0])
						_Init(IConst<0>());
				}

			public:
				BitFieldRef() {
					_Init();
				}
				template <int Idx>
				Value get() const {
					static_assert(Idx < int(BF::size), "out of index");
					constexpr auto& e = ES::entry[Idx];
					return (_value >> e.offset) & e.bottommask;
				}
				template <int Idx>
				void set(const Value val) {
					static_assert(Idx < int(BF::size), "out of index");
					constexpr auto& e = ES::entry[Idx];
					_value &= ~e.mask;
					_value |= (val & e.bottommask) << e.offset;
				}
				void clear() {
					value() = 0;
				}
				auto& value() {
					return _value;
				}
				const auto& value() const {
					return _value;
				}
				auto cleanedValue() const {
					return _value & mask;
				}
				bool operator == (const BitFieldRef& b) const {
					return cleanedValue() == b.cleanedValue();
				}
				bool operator != (const BitFieldRef& b) const {
					return cleanedValue() != b.cleanedValue();
				}
				bool operator < (const BitFieldRef& b) const {
					return cleanedValue() < b.cleanedValue();
				}
		};
		template <class BF>
		typename BitFieldRef<BF>::ChkFunc BitFieldRef<BF>::s_chkfunc[BF::size];
		template <class BF>
		typename BitFieldRef<BF>::SetFunc BitFieldRef<BF>::s_setfunc[BF::size];

		template <class T>
		struct Bitfield : Random {
			using Word = std::tuple_element_t<0,T>;
			constexpr static int N = std::tuple_element_t<1,T>::value,
								Width = std::tuple_element_t<2,T>::value;
			using BDef = DefineBF<Word, N, Width>;
			using BF = ::lubee::BitField<BDef>;
			using BFR = BitFieldRef<BDef>;
			constexpr static int NEnt = BDef::size;
		};

		using Types = ::testing::Types<std::tuple<uint32_t, IConst<3>, IConst<9>>>;
		TYPED_TEST_CASE(Bitfield, Types);

		TYPED_TEST(Bitfield, General) {
			USING(BF);
			USING(BFR);
			BF bf;
			bf.clear();
			BFR bfr;
			bfr.clear();
			auto& mt = this->mt();
			const auto mtf = mt.template getUniformF<int>();
			const auto mtu = mt.template getUniformF<typename BFR::Value>();
			constexpr int N = 256;
			const auto rndPos = [&mtf]{
				return mtf({0, TestFixture::NEnt-1});
			};
			for(int i=0 ; i<N ; i++) {
				switch(mtf({0, Action::_Num-1})) {
					case Action::Set:
						// 値をランダムな箇所にセット
						BFR::s_setfunc[rndPos()](bf, bfr, mtu());
						break;
					case Action::Get:
					{
						auto bf2 = bf;
						// ランダムな箇所から値を取得し、比較
						BFR::s_chkfunc[rndPos()](bf, bfr);
						ASSERT_EQ(bf2, bf);
						break;
					}
					case Action::Compare:
						// フラグの生の値を取得し、比較
						ASSERT_EQ(bfr.value(), bf.value());
						ASSERT_EQ(bfr.cleanedValue(), bf.cleanedValue());
						break;
				}
			}
		}
		TYPED_TEST(Bitfield, Compare) {
			USING(BFR);
			USING(BF);
			auto& mt = this->mt();
			const auto mtf = mt.template getUniformF<int>();
			const auto mtu = mt.template getUniformF<typename BFR::Value>();
			const int N = mtf({4, 64});
			std::vector<BFR> v_bfr(N);
			std::vector<BF> v_bf(N);
			for(int i=0 ; i<N ; i++) {
				v_bf[i].value() = v_bfr[i].value() = mtu();
			}
			std::sort(v_bf.begin(), v_bf.end(), [](const auto& v0, const auto& v1){
				return v0 < v1; });
			std::sort(v_bfr.begin(), v_bfr.end(), [](const auto& v0, const auto& v1){
				return v0 < v1; });

			for(int i=0 ; i<N ; i++) {
				const auto& cur = v_bf[i];
				ASSERT_EQ(v_bfr[i].cleanedValue(), cur.cleanedValue());
				ASSERT_FALSE(v_bfr[i].cleanedValue() != cur.cleanedValue());
				if(i > 0) {
					const auto& prev = v_bf[i-1];
					ASSERT_TRUE(prev<cur|| cur==prev);
				}
				if(i < N-1) {
					const auto& next = v_bf[i+1];
					ASSERT_TRUE(cur<next|| cur==next);
				}
			}
		}
	}
}
