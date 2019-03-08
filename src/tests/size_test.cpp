#include "test.hpp"
#include "../size.hpp"

namespace lubee::test {
	template <class T>
	struct TSize : Random {
		using value_t = std::tuple_element_t<0, T>;
		using other_t = std::tuple_element_t<1, T>;
		using Sz = ::lubee::Size<value_t>;
		Sz	s0, s1;

		template <class V>
		static auto Abs(const V& v) {
			if constexpr (std::is_signed_v<V>) {
				return std::abs(v);
			}
			return v;
		}
		template <class V=value_t>
		auto makeNonZeroValue() {
			V v;
			do {
				v = this->makePositive<V>();
			} while(Abs(v) <= V(1e-4));
			return v;
		}
		template <class V=value_t>
		auto makePositive() {
			return this->mt().template getUniform<V>(
				{0, 1024}
			);
		}
		template <class V=value_t>
		::lubee::Size<V> makeSz() {
			return { makePositive<V>(), makePositive<V>() };
		}
		void SetUp() override {
			// ランダムに2つ、Sizeを生成
			s0 = makeSz();
			s1 = makeSz();
		}
	};
	namespace {
		using TTypes = ::testing::Types<
			std::tuple<SizeI::value_t, SizeI::value_t>,
			std::tuple<SizeI::value_t, SizeF::value_t>,
			std::tuple<SizeF::value_t, SizeI::value_t>,
			std::tuple<SizeF::value_t, SizeF::value_t>
		>;
	}
	TYPED_TEST_CASE(TSize, TTypes);

	TYPED_TEST(TSize, Constructor) {
		USING(Sz);
		Sz sz;
		sz = this->s0;
		// Size(Size)
		ASSERT_EQ(this->s0, sz);

		// Size(w, h)
		sz.width = this->makePositive();
		sz.height = this->makePositive();
		ASSERT_EQ(sz, Sz(sz.width, sz.height));

		// Size(Size<other_t>)
		USING(value_t);
		USING(other_t);
		const auto other = this->template makeSz<other_t>();
		sz.width = static_cast<value_t>(other.width);
		sz.height = static_cast<value_t>(other.height);
		ASSERT_EQ(sz, Sz(other));
	}
	TYPED_TEST(TSize, Compare) {
		// operator ==
		ASSERT_EQ(this->s0, this->s0);
		// operator !=
		ASSERT_EQ(
			(this->s0.width != this->s1.width
				|| this->s0.height != this->s1.height),
			this->s0 != this->s1
		);
		// operator ==, !=
		ASSERT_NE(
			this->s0 == this->s1,
			this->s0 != this->s1
		);
		// operator <=
		ASSERT_EQ(
			this->s0.width <= this->s1.width && this->s0.height <= this->s1.height,
			this->s0 <= this->s1
		);

		USING(other_t);
		USING(Sz);
		const auto v = this->template makeNonZeroValue<other_t>();
		// operator *
		ASSERT_EQ(
			Sz(this->s0.width * v, this->s0.height * v),
			this->s0 * v
		);
		// operator /
		ASSERT_EQ(
			Sz(this->s0.width / v, this->s0.height / v),
			this->s0 / v
		);
	}
	TYPED_TEST(TSize, ToSize) {
		// toSize<other_t>で変換した物と、最初からSize<other_t>で構築した物を比較
		USING(other_t);
		ASSERT_EQ(
			::lubee::Size<other_t>(this->s0.width, this->s0.height),
			this->s0.template toSize<other_t>()
		);
	}

	template <class T>
	struct Size : TSize<std::tuple<T,T>> {};
	namespace {
		using Types = ::testing::Types<
			SizeI::value_t,
			SizeF::value_t
		>;
	}
	TYPED_TEST_CASE(Size, Types);

	TYPED_TEST(Size, Area) {
		// area
		ASSERT_EQ(
			this->s0.width * this->s0.height,
			this->s0.area()
		);

		// 各辺を2倍したら面積は4倍
		const auto s = this->s0 * 2;
		ASSERT_EQ(
			this->s0.area() * 4,
			s.area()
		);
	}
	TYPED_TEST(Size, Shift) {
		USING(value_t);
		constexpr size_t BWidth = sizeof(value_t)*8;
		auto& mt = this->mt();
		// shiftR
		{
			const auto n = mt.template getUniform<size_t>({0, BWidth-1});
			auto tmp = this->s0;
			tmp.shiftR(n);
			if(n == 0) {
				// 元の数と等しい
				ASSERT_EQ(this->s0, tmp);
			} else {
				if constexpr (std::is_integral_v<value_t>) {
					// 元の数にシフトした分のビットマスクをかけた値と等しい
					const auto makeMask = [](const size_t n){
						assert(n > 0);
						size_t ret(1);
						for(size_t i=0 ; i<n-1 ; i++)
							ret |= ret << 1;
						return ret;
					};
					ASSERT_EQ(
						this->s0.width & ~makeMask(n),
						tmp.width << n
					);
				} else {
					const value_t div(size_t(1) << n);
					ASSERT_EQ(tmp.width, this->s0.width / div);
					ASSERT_EQ(tmp.height, this->s0.height / div);
				}
			}

			// 内部値のビット数分以上、右シフトしたらゼロになる
			const auto n2 = mt.template getUniform<size_t>({BWidth, 128});
			tmp = this->s0;
			tmp.shiftR(n2);
			ASSERT_EQ(0, tmp.width);
			ASSERT_EQ(0, tmp.height);
		}
		// operator >>=
		{
			// shiftRと同じ結果になる
			const auto n = mt.template getUniform<size_t>({0, BWidth*2});

			auto tmp0 = this->s0;
			auto tmp1 = tmp0;
			tmp0.shiftR(n);
			tmp1 >>= n;
			ASSERT_EQ(tmp0, tmp1);
		}
		// shiftR_one
		{
			auto tmp = this->s0;
			size_t accum = 0;
			do {
				const auto n = mt.template getUniform<size_t>({0, BWidth-1});
				tmp.shiftR_one(n);
				accum += n;
				// 0にはならない
				ASSERT_GT(tmp.width, value_t(0));
				ASSERT_GT(tmp.height, value_t(0));
			} while(accum < BWidth);
			// 値は両方共1になっている
			ASSERT_EQ(value_t(1), tmp.width);
			ASSERT_EQ(value_t(1), tmp.height);
		}
		// shiftR_2pow
		{
			auto tmp = this->s0;
			tmp.shiftR_2pow();
			if constexpr (std::is_integral_v<value_t>) {
				// 2の累乗数または0になっている
				if(tmp.width != 0)
					ASSERT_EQ(1, bit::Count(uint64_t(tmp.width)));
				if(tmp.height != 0)
					ASSERT_EQ(1, bit::Count(uint64_t(tmp.height)));
			} else {
				const auto w = std::floor(tmp.width),
							h = std::floor(tmp.height);
				// 仮数部は0
				ASSERT_EQ(tmp.width, w);
				ASSERT_EQ(tmp.height, h);
				// 2の累乗数または0になっている
				if(w != 0)
					ASSERT_EQ(1, bit::Count(uint64_t(w)));
				if(h != 0)
					ASSERT_EQ(1, bit::Count(uint64_t(h)));
			}
		}
	}
	TYPED_TEST(Size, Expand) {
		USING(value_t);
		auto& mt = this->mt();
		const auto makeVal = [&mt](){
			if constexpr (std::is_integral_v<value_t>) {
				return mt.template getUniform<std::make_signed_t<value_t>>({-1024, 1024});
			} else {
				return mt.template getUniform<value_t>({-1024, 1024});
			}
		};
		// expand(w,h)
		{
			const auto dw = makeVal(),
						dh = makeVal();
			auto tmp = this->s0;
			if(tmp.expand(dw, dh)) {
				// dw, dhの少なくとも片方が負数
				ASSERT_TRUE((dw < 0) || (dh < 0));
				// 面積も0
				ASSERT_EQ(0, tmp.area());
				// width, heightの少なくとも片方が0
				ASSERT_TRUE((tmp.width == 0) || (tmp.height == 0));
			} else {
				// width, height共に0より大きい
				ASSERT_TRUE((tmp.width > 0) && (tmp.height > 0));
				// 面積も1以上
				ASSERT_GT(tmp.area(), 0);
			}
		}
		// expand(s)
		{
			// expand(s,s)と同じ結果になる
			const auto d = makeVal();
			auto tmp0 = this->s0,
				 tmp1 = tmp0;
			const bool b0 = tmp0.expand(d),
						b1 = tmp1.expand(d, d);
			ASSERT_EQ(tmp0, tmp1);
			ASSERT_EQ(b0, b1);
		}
	}
}
