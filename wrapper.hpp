#pragma once
#include <algorithm>

namespace lubee {
	template <class T>
	struct Wrapper {
		using value_t = T;
		value_t		_value;

		template <class Ar>
		value_t save_minimal(const Ar&) const { return _value; }
		template <class Ar>
		void load_minimal(const Ar&, const value_t& v) { _value = v; }

		template <class... Args>
		Wrapper(Args&&... args):
			_value(std::forward<Args>(args)...)
		{}
		Wrapper& operator = (const value_t& t) {
			_value = t;
			return *this;
		}
		Wrapper& operator = (value_t&& t) {
			_value = std::move(t);
			return *this;
		}
		operator const T& () const noexcept {
			return _value;
		}
		operator T& () noexcept {
			return _value;
		}
		bool operator == (const value_t& t) const noexcept {
			return _value == t;
		}
		bool operator != (const value_t& t) const noexcept {
			return _value != t;
		}
	};
	template <class T>
	inline bool operator == (const T& t0, const Wrapper<T>& t1) noexcept {
		return t0 == t1._value;
	}
	template <class T>
	inline bool operator != (const T& t0, const Wrapper<T>& t1) noexcept {
		return t0 != t1._value;
	}

	template <class T>
	std::false_type is_wrapper(T*);
	template <class T>
	std::true_type is_wrapper(Wrapper<T>*);
	template <class T>
	using is_wrapper_t = decltype(is_wrapper((T*)nullptr));

	template <class T>
	inline decltype(auto) wrapper_value(const T& v, std::true_type) noexcept {
		return v._value;
	}
	template <class T>
	inline decltype(auto) wrapper_value(const T& v, std::false_type) noexcept {
		return v;
	}
	template <class T>
	inline decltype(auto) wrapper_value(const T& v) noexcept {
		return wrapper_value(v, is_wrapper_t<T>());
	}
}
