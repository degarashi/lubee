#pragma once
#include <iostream>
#include <cstdio>
#include <stdexcept>
#include <sstream>

#define SOURCEPOS ::lubee::SourcePos{__FILE__, __PRETTY_FUNCTION__, __func__, __LINE__}
#define AssertBase(typ, exp, ...) \
	if(!(exp)) { \
		::lubee::err::typ{::lubee::MakeAssertMessage(#exp, __VA_ARGS__), SOURCEPOS}; \
	}
#ifdef DEBUG
	#ifdef THROW_ON_FAILURE
		// アサート失敗時にブレークせず、代わりに例外を投げる
		#define ON_FAIL		Throw<::lubee::err::AssertionFailed>
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
#define AssertF(...)			{ Assert(false, __VA_ARGS__); throw 0; }
#define ExpectF0()				Expect0(false)
#define Expect0(exp)			AssertBase(Warn, exp, "(no message)")
#define Expect(...)				AssertBase(Warn, __VA_ARGS__)
#define ExpectF(...)			{ Expect(false, __VA_ARGS__); throw 0; }

#ifdef DEBUG
	#define NOEXCEPT_IF_RELEASE
#else
	#define NOEXCEPT_IF_RELEASE noexcept
#endif

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
			<< "on line:\t" << p.line;
	}
	namespace err {
		struct AssertionFailed : std::runtime_error {
			using std::runtime_error::runtime_error;
		};

		//! アサート失敗時の挙動: ログメッセージ
		struct Warn {
			Warn(const std::string& msg, const SourcePos& pos) noexcept {
				try {
					std::cerr << msg << std::endl << pos << std::endl;
				} catch(...) {
					// 例外を外へ出さない
				}
			}
		};
		//! アサート失敗時の挙動: 例外を送出
		template <class E>
		struct Throw : Warn {
			Throw(const std::string& msg, const SourcePos& pos): Warn(msg, pos) {
				throw E(msg);
			}
		};
		//! アサート失敗時の挙動: デバッガをブリーク
		struct Trap : Warn {
			Trap(const std::string& msg, const SourcePos& pos) noexcept: Warn(msg, pos) {
				__builtin_trap();
			}
		};
	}
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wformat-security"
	//! デバッグメッセージの生成
	/*! 何かエラーが発生した時にコールされるので例外は送出しない */
	template <class... Ts>
	std::string MakeAssertMessage(const char* expr, const char* fmt, Ts&&... ts) noexcept {
		try {
			constexpr std::size_t BuffSize = 2048;
			char buff[BuffSize];
			std::snprintf(buff, BuffSize, fmt, std::forward<Ts>(ts)...);
			std::string ret("---<assertion failed!>---\nexpression: ");
			ret.append(expr);
			ret.append("\n");
			ret.append(buff);
			return ret;
		} catch(...) {
			// 何か例外が発生したら空のメッセージを返す
			return std::string();
		}
	}
	#pragma GCC diagnostic pop

	//! lubeeのアサートマクロが失敗した際に投げられる
	class AssertionFailed : public std::runtime_error {
		private:
			SourcePos				_pos;
			int						_lineTo;
			mutable std::string		_buff;
		public:
			AssertionFailed(const char* msg, const SourcePos& posAt):
				std::runtime_error(msg),
				_pos(posAt),
				_lineTo(0)
			{}
			AssertionFailed(const char* msg, const SourcePos& posFrom, const SourcePos& posTo):
				std::runtime_error(msg),
				_pos(posFrom),
				_lineTo(posTo.line)
			{}
			const char* what() const noexcept override {
				try {
					std::stringstream ss;
					ss << std::runtime_error::what() << std::endl;
					ss << _pos;
					if(_lineTo > 0)
						ss << " to " << _lineTo;
					ss << std::endl;
					_buff = ss.str();
					return _buff.c_str();
				} catch(...) {
					return "(error while exception handling)";
				}
			}
	};
	template <class T=void>
	struct ScopeHolder {
		struct Scope {
			const char*	name;
			SourcePos	pos;
		};
		thread_local static Scope tls_scope;
	};
	template <class T=void>
	constexpr SourcePos TopLevelSPos{"[no filename]", "[no function]", "[no function]", 0};
	template <class T>
	thread_local typename ScopeHolder<T>::Scope ScopeHolder<T>::tls_scope{"[top level]", TopLevelSPos<>};

	inline void PrintException(std::ostream& os, const int indent=0) {
		os << std::string(indent*4, ' ') << "(unknown exception)" << std::endl;
	}
	inline void PrintException(std::ostream& os, const std::exception& e, const int indent=0) {
		os << std::string(indent*4, ' ') << "std::exception: " << e.what() << std::endl;
	}
	inline void PrintNestedException(std::ostream& os, const std::exception& e, const int indent=0) {
		os << std::string(indent*4, ' ') << "exception: " << e.what() << std::endl;
		try {
			std::rethrow_if_nested(e);
		} catch(const std::exception& e) {
			PrintNestedException(os, e, indent+1);
		} catch(...) {
			PrintException(os, indent+1);
		}
	}
	class ExceptionScope {
		private:
			using SH = ScopeHolder<>;
			using Scope = typename SH::Scope;
			Scope			_prevScope;
		public:
			ExceptionScope(const char* msg, const SourcePos& pos):
				_prevScope(SH::tls_scope)
			{
				SH::tls_scope = Scope{msg, pos};
			}
			~ExceptionScope() noexcept(false) {
				if(!std::uncaught_exception())
					SH::tls_scope = _prevScope;
			}
	};
}
#if defined(DEBUG) && defined(THROW_ON_FAILURE)
	#define D_Scope(name) try { ::lubee::ExceptionScope scope_##__LINE__(name, SOURCEPOS);
	#define D_ScopeEnd() } catch(const ::lubee::AssertionFailed& e) { \
		const auto& s = ::lubee::ScopeHolder<>::tls_scope; \
		std::throw_with_nested(::lubee::AssertionFailed(s.name, s.pos, SOURCEPOS)); \
	}
	namespace lubee {
		template <class CB, class... Ts>
		void CallMain(std::ostream& os, CB&& cb, Ts&&... ts) {
			try {
				cb(std::forward<Ts>(ts)...);
			} catch(...) {
				try {
					std::rethrow_exception(std::current_exception());
				} catch(const std::exception& e) {
					PrintNestedException(os, e);
				} catch(...) {
					PrintException(os);
				}
			}
		}
	}
#else
	#define D_Scope(name) {
	#define D_ScopeEnd() }
	namespace lubee {
		template <class CB, class... Ts>
		void CallMain(std::ostream&, CB&& cb, Ts&&... ts) {
			cb(std::forward<Ts>(ts)...);
		}
	}
#endif
