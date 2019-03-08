#pragma once
#include "size.hpp"
#include "operators.hpp"

namespace lubee {
	//! 値の低い方に補正
	template <class T>
	struct PP_Lower {
		static T proc(const T& t) {
			T ret = bit::LowClear(t);
			return std::max(T(1), ret);
		}
	};
	//! 値の高い方に補正
	template <class T>
	struct PP_Higher {
		static T proc(const T& t) {
			T ret = bit::LowClear(t);
			if(t & ~ret)
				ret <<= 1;
			return std::max(T(1), ret);
		}
	};

	//! 2の乗数しか代入できない型
	/*! 0も許可されない */
	template <class T, template <class> class Policy>
	class PowValue : public op::Cmp<PowValue<T,Policy>>,
					public op::Arithmetic<PowValue<T,Policy>>
	{
		public:
			using value_t = T;
		private:
			using P = Policy<value_t>;
			value_t		_value;

		public:
			template <class Ar, class T2, template <class> class P2>
			friend void serialize(Ar&, PowValue<T2, P2>&);
			//TODO: テンプレートによる静的な値補正
			PowValue(const value_t& t):
				_value(P::proc(t))
			{}
			PowValue(const PowValue& p) = default;

			PowValue& operator = (const PowValue& p) = default;
			PowValue& operator = (const value_t& t) {
				// 2の乗数に補正 (動的なチェック)
				_value = P::proc(t);
				return *this;
			}
			bool operator == (const PowValue& v) const noexcept {
				return _value == v._value;
			}
			bool operator < (const PowValue& v) const noexcept {
				return _value < v._value;
			}
			#define DEF_OP(op) \
				PowValue operator op (const PowValue& v) const noexcept { \
					return _value op v._value; }
			DEF_OP(+)
			DEF_OP(-)
			DEF_OP(*)
			DEF_OP(/)
			#undef DEF_OP
			const value_t& get() const {
				return _value;
			}
			operator const value_t& () const {
				return _value;
			}
	};
	using PowInt = PowValue<uint32_t, PP_Higher>;
	using PowSize = Size<PowInt>;
	template <class... Ts>
	std::ostream& operator << (std::ostream& os, const ::lubee::PowValue<Ts...>& p) {
		return os << p.get();
	}
}
