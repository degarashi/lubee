#include "test.hpp"
#include "../point.hpp"

namespace lubee::test {
	template <class T>
	struct Point : Random {
		using value_t = T;
		using Pt = ::lubee::Point<value_t>;
		Pt	p0, p1;

		value_t makeValue() {
			return this->mt().template getUniform<value_t>(
				{-1024, 1024}
			);
		}
		value_t makeNonZeroValue() {
			value_t v;
			do {
				v = this->makeValue();
			} while(std::abs(v) <= value_t(1e-4));
			return v;
		}
		Pt makePt() {
			return { makeValue(), makeValue() };
		}
		void SetUp() override {
			// ランダムに2つ、点を生成
			p0 = makePt();
			p1 = makePt();
		}
	};
	namespace {
		using Types = ::testing::Types<PointI::value_t, PointF::value_t>;
	}
	TYPED_TEST_CASE(Point, Types);

	TYPED_TEST(Point, Constructor) {
		const auto v0 = this->makeValue(),
				  v1 = this->makeValue();
		USING(Pt);
		Pt p;
		p.x = v0;
		p.y = v1;
		ASSERT_EQ((Pt{v0, v1}), p);
	}
	TYPED_TEST(Point, Compare) {
		// operator ==
		ASSERT_EQ(
			this->p0.x==this->p1.x && this->p0.y==this->p1.y,
			this->p0 == this->p1
		);
		// operator !=
		ASSERT_EQ(
			this->p0.x!=this->p1.x || this->p0.y!=this->p1.y,
			this->p0 != this->p1
		);
		// operator ==, !=
		ASSERT_NE((this->p0==this->p1), (this->p0!=this->p1));
	}
	TYPED_TEST(Point, Operator) {
		USING(Pt);
		// 要素を個別に評価したのと各種オペレータの結果を比較
		// operator +
		ASSERT_EQ(
			(Pt{ this->p0.x + this->p1.x, this->p0.y + this->p1.y }),
			this->p0 + this->p1
		);
		// operator -
		ASSERT_EQ(
			(Pt{ this->p0.x - this->p1.x, this->p0.y - this->p1.y }),
			this->p0 - this->p1
		);

		const auto t = this->makeNonZeroValue();
		// operator *
		ASSERT_EQ(
			(Pt{ this->p0.x * t, this->p0.y * t }),
			this->p0 * t
		);
		// operator /
		ASSERT_EQ(
			(Pt{ this->p0.x / t, this->p0.y / t }),
			this->p0 / t
		);
	}
}
