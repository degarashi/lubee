#pragma once
#include <gtest/gtest.h>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>

namespace lubee {
	// ---- for serialization check ----
	template <class OA, class IA, class Init>
	void _CheckSerialization(Init&& init) {
		using T = std::remove_pointer_t<decltype(init())>;
		using Up = std::unique_ptr<T>;
		Up ptr0(init()),
		   ptr1;
		std::stringstream buffer;
		{
			OA oa(buffer);
			oa(cereal::make_nvp("obj", ptr0));
		}
		{
			IA ia(buffer);
			ia(cereal::make_nvp("obj", ptr1));
		}
		ASSERT_EQ(*ptr0, *ptr1);
	}
	template <class Make>
	void CheckSerializationBin(Make&& m) {
		_CheckSerialization<cereal::BinaryOutputArchive,
							cereal::BinaryInputArchive>(m);
	}
	template <class Make>
	void CheckSerializationJSON(Make&& m) {
		_CheckSerialization<cereal::JSONOutputArchive,
							cereal::JSONInputArchive>(m);
	}
	template <class Make>
	void CheckSerializationWithMake(Make&& m) {
		CheckSerializationBin(m);
		CheckSerializationJSON(m);
	}
	template <class T>
	void CheckSerialization(const T& src) {
		CheckSerializationWithMake([&](){ return new T(src); });
	}
}
