#pragma once
#include "meta/enable_if.hpp"

namespace lubee {
	namespace bit {
		//! 入力値の一番左のビットだけを残す
		template <class T, ENABLE_IF(std::is_unsigned<T>{})>
		inline constexpr T LowClear(T x) noexcept {
			T ts = 1;
			while(ts < T(sizeof(T)*8/2)) {
				x = x | (x >> ts);
				ts <<= 1;
			}
			return x & ~(x>>1);
		}
		//! most significant bit を取得
		/*! もし入力値が0の場合は0を返す */
		inline constexpr uint32_t MSB(uint32_t x) noexcept {
			const char NLRZ_TABLE[33] = "\x1f\x00\x01\x0e\x1c\x02\x16\x0f\x1d\x1a\x03\x05\x0b\x17"
										"\x07\x10\x1e\x0d\x1b\x15\x19\x04\x0a\x06\x0c\x14\x18\x09"
										"\x13\x08\x12\x11";
			x |= 0x01;
			return NLRZ_TABLE[0x0aec7cd2U * LowClear(x) >> 27];
		}
		//! ビットが幾つ立っているか数える
		template <class T, ENABLE_IF(std::is_unsigned<T>{} && (sizeof(T)<=4))>
		inline constexpr int Count(T v) noexcept {
			T tmp = v & 0xaaaaaaaa;
			v &= 0x55555555;
			v = v + (tmp >> 1);
			tmp = v & 0xcccccccc;
			v &= 0x33333333;
			v = v + (tmp >> 2);
			tmp = v & 0xf0f0f0f0;
			v &= 0x0f0f0f0f;
			v = v + (tmp >> 4);
			tmp = v & 0xff00ff00;
			v &= 0x00ff00ff;
			v = v + (tmp >> 8);
			tmp = v & 0xffff0000;
			v &= 0x0000ffff;
			v = v + (tmp >> 16);
			return v;
		}
		//! ビットカウントの64bitバージョン
		template <class T, ENABLE_IF(std::is_unsigned<T>{} && (sizeof(T)==8))>
		inline constexpr int Count(T v) noexcept {
			return Count(uint32_t(v)) + Count(uint32_t(v>>32));
		}
	}
}
