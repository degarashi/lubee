#pragma once
#include <cstring>
#include <algorithm>

namespace lubee {
	// 値の読み替え & コピー
	// (ポインタをreinterpret_castするのはstrict aliasingルールで違反となる為)
	template <class To, class From>
	void Reinterpret(To& to, const From& from) {
		// std::memcpyはコンパイラの最適化を期待
		std::memcpy(&to, &from, std::min(sizeof(from), sizeof(to)));
	}
}
