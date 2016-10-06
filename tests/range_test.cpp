#include "test.hpp"
#include "../random/range.hpp"

namespace lubee {
	namespace test {
		template <class T>
		struct Range : Random {
			using value_t = T;
			using Range_t = ::lubee::Range<value_t>;

			auto makeMtf() {
				return mt().template getUniformF<T>();
			}
			Range_t makeRange() {
				return ::lubee::random::GenRange<T>(makeMtf());
			}
		};
		using Types = ::testing::Types<int, double, char>;
		TYPED_TEST_CASE(Range, Types);

		template <class T, class MTF, ENABLE_IF(std::is_floating_point<T>{})>
		T GenValueNZ(MTF& mtf) {
			T ret;
			do {
				ret = mtf();
			} while(std::abs(ret) < 1e-2);
			return ret;
		}
		template <class T, class MTF, ENABLE_IF(std::is_integral<T>{})>
		T GenValueNZ(MTF& mtf) {
			T ret;
			do {
				ret = mtf();
			} while(ret == 0);
			return ret;
		}
		TYPED_TEST(Range, General) {
			USING(Range_t);
			const auto mtf = this->makeMtf();
			{
				// range(v) = range(-v,v)
				const auto v = std::abs(mtf());
				const Range_t rt0(v),
					  rt1(-v, v);
				ASSERT_EQ(rt0, rt1);
			}

			const auto r0 = this->makeRange();
			{
				// hit(value)
				const auto v = mtf();
				const bool b = v>=r0.from && v<=r0.to;
				ASSERT_EQ(b, r0.hit(v));
			}
			{
				// hit(range)
				const auto r1 = this->makeRange();
				const bool b0 = r0.hit(r1),
							b1 = r1.hit(r0);
				ASSERT_EQ(b0, b1);
				const bool b2 =
					(r1.from >= r0.from && r1.from <= r0.to) ||
					(r1.to >= r0.from && r1.to <= r0.to) ||
					(r1.from <= r0.from && r1.to >= r0.to) ||
					(r1.from >= r0.from && r1.to <= r0.to);
				ASSERT_EQ(b0, b2);
			}
			{
				// ==, !=
				Range_t r00(r0.from, r0.to);
				ASSERT_EQ(r00, r0);
				do {
					r00.from = mtf();
				} while(r00.from == r0.from);
				ASSERT_NE(r00, r0);

				r00.from = r0.from;
				do {
					r00.to = mtf();
				} while(r00.to == r0.to);
				ASSERT_NE(r00, r0);
			}
			USING(value_t);
			const auto check = [&mtf, &r0](const auto& op, const auto& ope) {
				const auto v = GenValueNZ<value_t>(mtf);
				const auto r1 = op(r0, v);
				auto r2 = r0;
				ope(r2, v);
				if(r1.from <= r1.to) {
					ASSERT_EQ(r1, r2);
					ASSERT_EQ(Range_t(op(r0.from, v), op(r0.to, v)), r1);
				}
			};
			// +=, + value
			check(std::plus<>(), [](auto& a, const auto& b){ a+=b; });
			// -=, - value
			check(std::minus<>(), [](auto& a, const auto& b){ a-=b; });
			// *=, * value
			check(std::multiplies<>(), [](auto& a, const auto& b){ a*=b; });
			// /=, / value
			check(std::divides<>(), [](auto& a, const auto& b){ a/=b; });
		}
		TYPED_TEST(Range, Serialization) {
			const auto r = this->makeRange();
			::lubee::CheckSerialization(r);
		}
	}
}
