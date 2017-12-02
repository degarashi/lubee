#pragma once
#include <algorithm>
#include "meta/enable_if.hpp"

namespace lubee {
	template <class T>
	struct Wrapper;
	template <class T>
	std::false_type is_wrapper(T*);
	template <class T>
	std::true_type is_wrapper(Wrapper<T>*);
	template <class T>
	using is_wrapper_t = decltype(is_wrapper((T*)nullptr));

	template <class T>
	struct Wrapper {
		using value_t = T;
		value_t		_value;

		template <class Ar>
		value_t save_minimal(const Ar&) const { return _value; }
		template <class Ar>
		void load_minimal(const Ar&, const value_t& v) { _value = v; }

		Wrapper() = default;
		template <class A0, class... Args>
		explicit Wrapper(A0&& a0, Args&&... args):
			_value(std::forward<A0>(a0), std::forward<Args>(args)...)
		{}
		template <class A, ENABLE_IF(!is_wrapper_t<A>{})>
		explicit Wrapper(A&& arg):
			_value(std::forward<A>(arg))
		{}
		template <
			class A,
			 ENABLE_IF((
				std::is_assignable<value_t, A>{}
			))
		>
		Wrapper& operator = (A&& a) {
			_value = std::forward<A>(a);
			return *this;
		}
		Wrapper& operator = (const value_t& a) {
			_value = a;
			return *this;
		}
		Wrapper& operator = (value_t&& a) {
			_value = std::move(a);
			return *this;
		}
		operator const value_t& () const noexcept {
			return _value;
		}
		operator value_t& () noexcept {
			return _value;
		}
		value_t* operator -> () noexcept {
			return &_value;
		}
		const value_t* operator -> () const noexcept {
			return &_value;
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
