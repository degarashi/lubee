#include <gtest/gtest.h>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>

namespace lubee {
	// ---- for serialization check ----
	template <class OA, class IA, class T>
	void _CheckSerialization(const T& src) {
		std::stringstream buffer;
		{
			OA oa(buffer);
			oa(CEREAL_NVP(src));
		}
		T loaded;
		{
			IA ia(buffer);
			ia(cereal::make_nvp("src", loaded));
		}
		ASSERT_EQ(src, loaded);
	}
	template <class T>
	void CheckSerializationBin(const T& src) {
		_CheckSerialization<cereal::BinaryOutputArchive,
							cereal::BinaryInputArchive>(src);
	}
	template <class T>
	void CheckSerializationJSON(const T& src) {
		_CheckSerialization<cereal::JSONOutputArchive,
							cereal::JSONInputArchive>(src);
	}
	template <class T>
	void CheckSerialization(const T& src) {
		CheckSerializationBin(src);
		CheckSerializationJSON(src);
	}
}
