#include "test.hpp"
#include "../ieee754.hpp"
#include "../fpset.hpp"

namespace lubee {
	namespace test {
		using IE16 = IEEE754<Half>;
		using IE32 = IEEE754<float>;

		float StripFracbits(const float f) {
			auto i = *reinterpret_cast<const uint32_t*>(&f);
			i &= ~((1 << (IE32::FracBits - IE16::FracBits))-1);
			return *reinterpret_cast<const float*>(&i);
		}

		template <class F>
		bool IsZero(const F& fp) {
			return fp.fract == 0 &&
					fp.exp == 0;
		}
		template <class F>
		bool IsNaN(const F& fp) {
			return fp.exp == (1<<F::IE::ExpBits)-1 &&
					fp.fract != 0;
		}
		template <class F>
		bool IsInf(const F& fp) {
			return fp.exp == (1<<F::IE::ExpBits)-1 &&
					fp.fract == 0 &&
					fp.sign == 0;
		}
		template <class F>
		bool IsMInf(const F& fp) {
			return fp.exp == (1<<F::IE::ExpBits)-1 &&
					fp.fract == 0 &&
					fp.sign == 1;
		}

		template <class Check>
		void ValueTest(const float val, const Check& check) {
			// Fp16(float);
			ASSERT_TRUE(check(Fp16(val)));
			// Fp16(Fp32());
			ASSERT_TRUE(check(Fp16(Fp32(val))));
			// Fp32(float);
			ASSERT_TRUE(check(Fp32(val)));
			// Fp32(Fp16());
			ASSERT_TRUE(check(Fp32(Fp16(val))));
		}

		struct Fpset : Random {};
		TEST_F(Fpset, General) {
			auto& mt = this->mt();
			// zero
			ValueTest(0.f, [](const auto& v){ return IsZero(v); });
			using Lim = std::numeric_limits<float>;
			// NaN
			ValueTest(Lim::quiet_NaN(),
					[](const auto& v){ return IsNaN(v); });
			// inf
			ValueTest(Lim::infinity(),
					[](const auto& v){ return IsInf(v); });
			// -inf
			ValueTest(-Lim::infinity(),
					[](const auto& v){ return IsMInf(v); });

			const auto mtf = mt.template getUniformF<float>({-std::pow(2,-14), std::pow(2,15)});
			const float val = mtf();
			{
				// Fp16の範囲にて、Fp16 -> Fp32 -> Fp16
				const Fp16 fp16(val);
				const float val0 = fp16.asFloat();
				const Fp32 fp32(fp16);
				const float val1 = fp32.asFloat();
				const Fp16 fp16_2(fp32);
				ASSERT_EQ(val0, val1);
				ASSERT_EQ(StripFracbits(val), fp16_2.asFloat());
			}
			{
				// (Fp16 & Fp32) setFloatのテスト
				Fp16 fp16;
				Fp32 fp32;
				fp16.setFloat(val);
				fp32.setFloat(val);
				ASSERT_EQ(StripFracbits(val), fp16.asFloat());
				ASSERT_EQ(val, fp32.asFloat());
			}
		}
	}
}
