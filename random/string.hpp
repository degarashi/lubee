#pragma once
#include <string>

namespace lubee {
	namespace random {
		//! 任意の文字列候補からなるランダム文字列を生成
		/*!
			\param[in] rd		ランダム生成器(整数)
			\param[in] len		生成する文字列数
			\param[in] src		文字候補配列
		*/
		template <class RD, class C>
		auto GenString(RD&& rd, const std::size_t len, const std::basic_string<C>& src) {
			std::basic_string<C> result;
			result.resize(len);
			const auto srcLen = src.size();
			for(std::size_t i=0 ; i<len ; i++) {
				const int cn = rd({0, srcLen-1});
				result[i] = src[cn];
			}
			return result;
		}
		//! ランダムなアルファベット列(utf-8)
		template <class RD>
		auto GenAlphabetString(const RD& rd, const std::size_t len) {
			//! ランダム文字列生成に使うアルファベット配列
			const static std::string c_alphabet("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
			return GenString(rd, len, c_alphabet);
		}
	}
}
