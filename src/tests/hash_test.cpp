#include "test.hpp"
#include "../hash_combine.hpp"
#include "../random/string.hpp"

namespace lubee::test {
	struct TestValue_Num {
		uint32_t	value0;
		float		value1;
		double		value2;

		bool operator == (const TestValue_Num& num) const noexcept {
			return value0 == num.value0 &&
				value1 == num.value1 &&
				value2 == num.value2;
		}
		std::size_t simpleHash() const noexcept {
			return std::hash<decltype(value0)>()(value0) +
					std::hash<decltype(value1)>()(value1) +
					std::hash<decltype(value2)>()(value2);
		}
		std::size_t combinedHash() const noexcept {
			return hash_combine_implicit(value0, value1, value2);
		}
		template <class RD>
		static TestValue_Num Random(RD&& rd) {
			return {
				.value0 = rd.template getUniform<decltype(value0)>(),
				.value1 = rd.template getUniform<decltype(value1)>(),
				.value2 = rd.template getUniform<decltype(value2)>()
			};
		}
	};
	struct TestValue_String {
		using array_t = std::array<std::string, 4>;
		array_t		str;
		bool operator == (const TestValue_String& s) const noexcept {
			return str == s.str;
		}
		std::size_t simpleHash() const noexcept {
			std::size_t seed = 0;
			for(auto& s : str)
				seed += std::hash<std::string>()(s);
			return seed;
		}
		std::size_t combinedHash() const noexcept {
			return hash_combine_range_implicit(str.begin(), str.end());
		}
		template <class RD>
		static TestValue_String Random(RD& rd) {
			const auto rdI = rd.template getUniformF<std::size_t>();
			array_t ar;
			for(auto& s : ar)
				s = random::GenAlphabetString(rdI, 32);
			return {std::move(ar)};
		}
	};

	template <class T>
	class Hash : public Random {
		protected:
			using key_t = T;
			using value_t = uint32_t;
			struct SimpleHash {
				std::size_t operator()(const T& t) const noexcept {
					return t.simpleHash();
				}
			};
			struct CombinedHash {
				std::size_t operator()(const T& t) const noexcept {
					return t.combinedHash();
				}
			};
			using map_t = std::unordered_map<key_t, value_t, CombinedHash>;
			using smap_t = std::unordered_map<key_t, value_t, SimpleHash>;
	};

	using Types = ::testing::Types<TestValue_Num, TestValue_String>;
	TYPED_TEST_SUITE(Hash, Types);

	TYPED_TEST(Hash, General) {
		USING(key_t);
		USING(value_t);
		USING(map_t);
		USING(smap_t);

		map_t		map;
		smap_t		smap;
		auto& mt = this->mt();
		std::size_t n = mt.template getUniform<std::size_t>({16, 128});
		while(n != 0) {
			const auto key = key_t::Random(mt);
			const auto val = mt.template getUniform<value_t>();
			const bool b0 = map.emplace(key, val).second,
					b1 = smap.emplace(key, val).second;
			ASSERT_EQ(b0, b1);
			ASSERT_EQ(map.size(), smap.size());
			--n;
		}

		for(auto& e : smap) {
			const auto itr = map.find(e.first);
			ASSERT_NE(itr, map.end());
			ASSERT_EQ(itr->second, e.second);
		}
	}
}
