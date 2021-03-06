#pragma once
#include "error.hpp"
#include "meta/enable_if.hpp"

namespace lubee {
	namespace detail {
		// Chk::errorDesck(...)がnullptr以外を返したらエラーと判断し、Act.onErrorを呼ぶ
		template <class Act, class Chk, class... Ts>
		void EChk(Chk&& chk, const SourcePos& pos, Ts&&... ts) {
			const char* msg = chk.errorDesc(std::forward<Ts>(ts)...);
			if(msg)
				Act{MakeAssertMessage(chk.getAPIName(), msg), pos};
		}
		// ------------ エラーチェック & アサート ------------
		// APIがエラーを記憶しているタイプ
		// 戻り値がある場合のエラーチェック
		template <class Act, class Chk, class Func, class... TsA,
					class RT = decltype(std::declval<Func>()(std::declval<TsA>()...)),
					ENABLE_IF((!std::is_same<void,RT>{}))>
		auto EChk_polling(Chk&& chk, const SourcePos& pos, Func&& func, TsA&&... ts) {
			chk.reset();
			auto res = func(std::forward<TsA>(ts)...);
			EChk<Act>(chk, pos);
			return res;
		}
		// 戻り値がない場合のエラーチェック
		template <class Act, class Chk, class Func, class... TsA,
					class RT = decltype(std::declval<Func>()(std::declval<TsA>()...)),
					ENABLE_IF((std::is_same<void,RT>{}))>
		void EChk_polling(Chk&& chk, const SourcePos& pos, Func&& func, TsA&&... ts) {
			chk.reset();
			func(std::forward<TsA>(ts)...);
			EChk<Act>(chk, pos);
		}
		template <class Act, class Chk>
		void EChk_polling(Chk&& chk, const SourcePos& pos) {
			EChk<Act>(chk, pos);
		}
		// APIがエラーコードを返すタイプ
		template <class Act, class Chk, class Func, class... TsA>
		auto EChk_return(Chk&& chk, const SourcePos& pos, Func&& func, TsA&&... ts) {
			const auto code = func(std::forward<TsA>(ts)...);
			EChk<Act>(chk, pos, code);
			return code;
		}
		// 処理はせずに予め取得した結果コードを入力してエラーチェックのみ
		template <class Act, class Chk, class CODE>
		auto EChk_usercode(Chk&& chk, const SourcePos& pos, const CODE& code) {
			EChk<Act>(chk, pos, code);
			return code;
		}
	}
	#define EChk_polling_a		::lubee::detail::EChk_polling
	#define EChk_return_a		::lubee::detail::EChk_return
	#define EChk_usercode_a		::lubee::detail::EChk_usercode
	#define EChk_a				::lubee::detail::EChk
	#ifdef DEBUG
		#define EChk_polling_d		::lubee::detail::EChk_polling
		#define EChk_return_d		::lubee::detail::EChk_return
		#define EChk_usercode_d		::lubee::detail::EChk_usercode
		#define EChk_d				::lubee::detail::EChk
	#else
		#define EChk_d(...)
		namespace detail {
			// ----- エラーチェック無し(非デバッグモード時) -----
			template <class Act, class Chk, class Func, class... Ts>
			auto EChk_polling_pass(Chk&&, const SourcePos&, Func&& func, Ts&&... ts) {
				return func(std::forward<Ts>(ts)...);
			}
			template <class Act, class Chk>
			void EChk_polling_pass(Chk&&, const SourcePos&) {}
			template <class Act, class... Ts>
			auto EChk_return_pass(Ts&&... ts) {
				return EChk_polling_pass<Act>(std::forward<Ts>(ts)...);
			}
			template <class Act, class Chk, class CODE>
			auto EChk_usercode_pass(Chk&&, const SourcePos&, const CODE& code) {
				return code;
			}
		}
		#define EChk_polling_d		::lubee::detail::EChk_polling_pass
		#define EChk_return_d		::lubee::detail::EChk_return_pass
		#define EChk_usercode_d		::lubee::detail::EChk_usercode_pass
	#endif
}
