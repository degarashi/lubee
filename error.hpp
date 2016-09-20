#pragma once
#include <iostream>
#include <cstdio>
#include <stdexcept>

#define SOURCEPOS ::lubee::SourcePos{__FILE__, __PRETTY_FUNCTION__, __func__, __LINE__}
#define AssertBase(typ, exp, ...) \
	if(!(exp)) { \
		::lubee::err::typ{::lubee::MakeAssertMessage(#exp, __VA_ARGS__), SOURCEPOS}; \
	}
#ifdef DEBUG
	#ifdef THROW_ON_FAILURE
		// アサート失敗時にブレークせず、代わりに例外を投げる
		#define ON_FAIL		Throw
	#else
		#define ON_FAIL		Trap
	#endif

	#define D_Assert0(exp)		Assert0(exp)
	#define D_Assert(...)		Assert(__VA_ARGS__)
	#define D_Expect0(exp)		Expect0(exp)
	#define D_Expect(...)		Expect(__VA_ARGS__)
#else
	#define ON_FAIL			Trap
	#define D_Assert0(...) {}
	#define D_Assert(...) {}
	#define D_Expect0(...) {}
	#define D_Expect(...) {}
#endif
#define AssertF0()				Assert0(false)
#define Assert0(exp)			AssertBase(ON_FAIL, exp, "(no message)")
#define Assert(...)				AssertBase(ON_FAIL, __VA_ARGS__)
#define ExpectF0()				Expect0(false)
#define Expect0(exp)			AssertBase(Warn, exp, "(no message)")
#define Expect(...)				AssertBase(Warn, __VA_ARGS__)

namespace lubee {
	//! ソースコード上の位置を表す情報
	struct SourcePos {
		const char	*filename,
					*funcname,
					*funcname_short;
		int			line;
	};
	inline std::ostream& operator << (std::ostream& s, const SourcePos& p) {
		using std::endl;
		return s
			<< "at file:\t" << p.filename << endl
			<< "at function:\t" << p.funcname << endl
			<< "on line:\t" << p.line << endl;
	}
	namespace err {
		struct AssertionFailed : std::runtime_error {
			using std::runtime_error::runtime_error;
		};

		//! アサート失敗時の挙動: ログメッセージ
		struct Warn {
			Warn(const std::string& msg, const SourcePos& pos) {
				std::cerr << msg << std::endl << pos << std::endl;
			}
		};
		//! アサート失敗時の挙動: 例外を送出
		struct Throw : Warn {
			Throw(const std::string& msg, const SourcePos& pos): Warn(msg, pos) {
				throw AssertionFailed(msg);
			}
		};
		//! アサート失敗時の挙動: デバッガをブリーク
		struct Trap : Warn {
			Trap(const std::string& msg, const SourcePos& pos): Warn(msg, pos) {
				__builtin_trap();
			}
		};
	}
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wformat-security"
	template <class... Ts>
	std::string MakeAssertMessage(const char* expr, const char* fmt, Ts&&... ts) {
		constexpr std::size_t BuffSize = 2048;
		char buff[BuffSize];
		std::snprintf(buff, BuffSize, fmt, std::forward<Ts>(ts)...);
		std::string ret("---<assertion failed!>---\nexpression: ");
		ret.append(expr);
		ret.append("\n");
		ret.append(buff);
		return ret;
	}
	#pragma GCC diagnostic pop
}
