#pragma once
#include <type_traits>
#include <limits>
#include <cmath>
#include "meta/enable_if.hpp"

namespace lubee {
	//! 浮動少数点数の値がNaNになっているか
	template <class T, ENABLE_IF((std::is_floating_point<T>{}))>
	bool IsNaN(const T& val) {
		return std::isnan(val);
	}
	//! 浮動少数点数の値がNaN又は無限大になっているか
	template <class T, ENABLE_IF((std::is_floating_point<T>{}))>
	bool IsOutstanding(const T& val) {
		auto valA = std::abs(val);
		return valA==std::numeric_limits<T>::infinity() || IsNaN(valA);
	}
	//! 値飽和
	template <class T>
	T Saturate(const T& val, const T& minV, const T& maxV) {
		if(val > maxV)
			return maxV;
		if(val < minV)
			return minV;
		return val;
	}
	template <class T>
	T Saturate(const T& val, const T& range) {
		return Saturate(val, -range, range);
	}
	//! 値の範囲判定
	template <class T>
	bool IsInRange(const T& val, const T& vMin, const T& vMax) {
		return val>=vMin && val<=vMax;
	}
	template <class T>
	bool IsInRange(const T& val, const T& vMin, const T& vMax, const T& vEps) {
		return IsInRange(val, vMin-vEps, vMax+vEps);
	}
}
