#pragma once
#include "meta/enable_if.hpp"
#include "error.hpp"
#include <stack>

namespace cereal {
	template <class T>
	class construct;
}
namespace lubee {
	//! 配列などのインデックスをフリーリストで管理
	template <class T>
	class Freelist {
		public:
			using value_t = T;
			template <class Ar>
			static void load_and_construct(Ar&, cereal::construct<Freelist>&);
		private:
			using Check = ENABLE_IF_I(std::is_integral<value_t>{});
			using IDStk = std::stack<value_t, std::deque<value_t>>;
			value_t		_maxV,
						_startV,
						_curV;
			IDStk		_stk;

			template <class Ar, class T2>
			friend void serialize(Ar&, Freelist<T2>&);
		public:
			Freelist(value_t maxV, value_t startV):
				_maxV(maxV),
				_startV(startV),
				_curV(startV)
			{}
			value_t get() {
				if(_stk.empty()) {
					value_t ret = _curV++;
					Assert0(_curV != _maxV);
					return ret;
				}
				value_t val = _stk.top();
				_stk.pop();
				return val;
			}
			void put(value_t val) {
				_stk.push(val);
			}
			void swap(Freelist& f) noexcept {
				std::swap(_maxV, f._maxV);
				std::swap(_curV, f._curV);
				std::swap(_stk, f._stk);
			}
			bool operator == (const Freelist& f) const noexcept {
				return _maxV == f._maxV &&
						_startV == f._startV &&
						_curV == f._curV &&
						_stk == f._stk;
			}
			bool operator != (const Freelist& f) const noexcept {
				return !(this->operator == (f));
			}
	};
}
