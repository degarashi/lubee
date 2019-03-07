#pragma once

namespace lubee {
	// ラムダ式で再帰を記述するための補助クラス
	// Recursive([](auto&& f, int n) -> int {
	//		if(n == 0)
	//			return 1;
	//		return f(n-1) + n;
	// })(40);
	// [0]=1, [1]=2, [2]=4, [3]=7, [4]=11, ...  といった数が得られる
	template <class F>
	struct Recursive : F {
		explicit constexpr Recursive(F&& f) noexcept:
			F(std::forward<F>(f))
		{}

		template <class... Args>
		constexpr decltype(auto) operator()(Args&&... args) const {
			return F::operator()(*this, std::forward<Args>(args)...);
		}
	};
}
