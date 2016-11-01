//! classes of floating-point 16bit
#pragma once
#include "ieee754.hpp"
#include "bit.hpp"

namespace lubee {
	struct Fp32;
	//! 16bitの浮動小数点数
	/*!
		GPUに16bitFPを渡す時等に使用
		何か計算する時は一旦floatに戻す
	*/
	struct Fp16 {
		using IE = IEEE754<Half>;
		unsigned short	fract : IE::FracBits,
						exp : IE::ExpBits,
						sign : 1;
		Fp16() = default;
		Fp16(const float v) noexcept {
			setFloat(v);
		}
		Fp16(Fp32 fp) noexcept;
		float asFloat() const noexcept;
		void setFloat(const float v) noexcept;
	};

	//! floatのラッパークラス
	struct Fp32 {
		using IE = IEEE754<float>;
		unsigned int 	fract : IE::FracBits,
						exp : IE::ExpBits,
						sign : 1;

		Fp32() = default;
		Fp32(const float v) noexcept {
			setFloat(v);
		}
		Fp32(const Fp16 fp) noexcept {
			using IE16 = IEEE754<Half>;
			if(fp.exp == (1<<IE16::ExpBits)-1) {
				sign = fp.sign;
				exp = (1<<IE::ExpBits)-1;
				if(fp.fract == 0) {
					// Inf
					fract = 0;
				} else {
					// NaN
					fract = 1<<(IE::FracBits-1);
				}
				return;
			}
			int32_t t_fract = int32_t(fp.fract) << (IE::FracBits-IE16::FracBits);
			int32_t t_exp = int32_t(fp.exp);
			const int32_t mask = bit::ZeroOrFull(t_exp);								// 指数が0の時に0, それ以外は全部1
			const int32_t mask2 = bit::ZeroOrFull(t_exp ^ ((1<<IE16::FracBits)-1));		// 指数が15の時に0
			const int32_t mask3 = mask|mask2;											// 指数が上限または下限の時に0
			t_fract &= mask3;
			t_exp = t_exp - IE16::ExpZero + IE::ExpZero;
			t_exp &= mask;								// 指数のマイナス飽和
			t_exp |= ~mask2 & ((1<<IE::ExpBits)-1);		// 指数のプラス飽和

			exp = t_exp;
			fract = t_fract;
			sign = fp.sign;
		}

		float asFloat() const noexcept {
			return *reinterpret_cast<const float*>(this);
		}
		void setFloat(const float v) noexcept {
			*this = *reinterpret_cast<const Fp32*>(&v);
		}
	};

	Fp16::Fp16(const Fp32 fp) noexcept {
		using IE32 = IEEE754<float>;
		int32_t t_exp = int32_t(fp.exp) - IE32::ExpZero + IE::ExpZero;
		// 負数ならビットが全部1になり、それ以外は全部0
		int32_t mask = (t_exp >> (sizeof(t_exp)*8-1));
		// NaN
		if(fp.exp == (1<<IE32::ExpBits)-1) {
			sign = fp.sign;
			exp = (1<<IE::ExpBits)-1;
			if(fp.fract == 0) {
				// Inf
				fract = 0;
			} else {
				// NaN
				fract = 1<<(IE::FracBits-1);
			}
			return;
		}
		// 指数のマイナス飽和
		t_exp &= ~mask;
		// 指数のプラス飽和
		const int32_t mask2 = bit::ZeroOrFull(t_exp & ~((1<<IE::ExpBits)-1));
		t_exp |= mask2;
		t_exp &= (1<<IE::ExpBits)-1;
		// 指数がどちらかに飽和したら仮数部は0とする
		mask |= mask2;
		int32_t t_fract = fp.fract >> (IE32::FracBits - IE::FracBits);
		t_fract &= ~(mask|mask2);

		exp = t_exp;
		sign = fp.sign;
		fract = t_fract;
	}
	float Fp16::asFloat() const noexcept {
		const Fp32 ret(*this);
		return *reinterpret_cast<const float*>(&ret);
	}
	void Fp16::setFloat(const float v) noexcept {
		const Fp32 fp32(v);
		*this = Fp16(fp32);
	}
}
