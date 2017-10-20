#pragma once
#include <gtest/gtest.h>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>

namespace lubee {
	// ---- for serialization check ----
	template <class OA, class IA, class PTR, class CMP>
	void _CheckSerializationImpl(const PTR& ptr0, PTR& ptr1, const CMP& cmp) {
		std::stringstream buffer;
		{
			OA oa(buffer);
			oa(cereal::make_nvp("obj", ptr0));
		}
		{
			IA ia(buffer);
			ia(cereal::make_nvp("obj", ptr1));
		}
		ASSERT_TRUE(cmp(*ptr0, *ptr1));
	}
	template <class PTR, class CMP>
	void _CheckSerializationBin(const PTR& ptr0, PTR& ptr1, const CMP& cmp) {
		_CheckSerializationImpl<cereal::BinaryOutputArchive,
								cereal::BinaryInputArchive>(ptr0, ptr1, cmp);
	}
	template <class PTR, class CMP>
	void _CheckSerializationJSON(const PTR& ptr0, PTR& ptr1, const CMP& cmp) {
		_CheckSerializationImpl<cereal::JSONOutputArchive,
								cereal::JSONInputArchive>(ptr0, ptr1, cmp);
	}
	template <class PTR, class CMP>
	void _CheckSerialization(const PTR& ptr0, PTR& ptr1, const CMP& cmp) {
		_CheckSerializationBin(ptr0, ptr1, cmp);
		_CheckSerializationJSON(ptr0, ptr1, cmp);
	}
	template <class T, class CMP = std::equal_to<>>
	void CheckSerialization(const T& src, const CMP& cmp = CMP()) {
		using Up = std::unique_ptr<T>;
		Up ptr0(new T(src)),
		   ptr1;
		_CheckSerialization(ptr0, ptr1, cmp);
	}
	template <class T, class CMP = std::equal_to<>>
	void CheckSerialization(const std::shared_ptr<T>& sp, const CMP& cmp = CMP()) {
		std::shared_ptr<T> sp1;
		_CheckSerialization(sp, sp1, cmp);
	}
}
