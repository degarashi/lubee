#pragma once
#include <cstdio>
#include <stdexcept>
#include <sstream>
#include "output.hpp"
#include "meta/enable_if.hpp"

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
	namespace err {
		struct AssertionFailed : std::runtime_error {
			using std::runtime_error::runtime_error;
		};
		//! アサート失敗時の挙動: ログメッセージ
		struct Warn {
			Warn(const std::string& msg, const SourcePos& pos) noexcept {
				try {
					::lubee::log::Log::Output(::lubee::log::Type::Error, pos, msg);
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
	#define DEF_CATCH(typ)	catch(typ t) { cb(t); }
	// 現在投げられている例外をすべてキャッチしてコールバック関数の引数とする
	template <class T, class CB>
	void CatchAll(CB&& cb) {
		try {
			throw;
		} catch(const T& t) {
			cb(t);
		}
		DEF_CATCH(uint64_t)
		DEF_CATCH(uint32_t)
		DEF_CATCH(uint16_t)
		DEF_CATCH(uint8_t)
		DEF_CATCH(int64_t)
		DEF_CATCH(int32_t)
		DEF_CATCH(int16_t)
		DEF_CATCH(int8_t)
		DEF_CATCH(float)
		DEF_CATCH(double)
		DEF_CATCH(bool)
		DEF_CATCH(void*)
		catch(...) { cb("(unknown exception)"); }
	}
	// std::exceptionベースの引数ならwhat()を印字、そうでなければそのままstd::ostreamへ渡す
	template <class T, ENABLE_IF(!(std::is_base_of<std::exception, T>::value))>
	void PrintException(std::ostream& os, const T& t) {
		os << "(unknown type): " << t;
	}
	template <class T, ENABLE_IF((std::is_base_of<std::exception, T>::value))>
	void PrintException(std::ostream& os, const T& t) {
		os << "(std::exception based): " << t.what();
	}
	#undef DEF_CATCH
	class ExceptionScope {
		private:
			const char*	name;
			SourcePos	pos;
		public:
			ExceptionScope(const char* name, const SourcePos& pos):
				name(name),
				pos(pos)
			{}
			void log() const {
				std::stringstream ss;
				ss << "exception pass through(" << name << "): at\n";
				ss << pos << std::endl;
				CatchAll<std::exception>([&ss](const auto& e){
					PrintException(ss, e);
				});
				ss << std::endl;
				Log(Info, ss.str().c_str());
			}
	};
}
#if defined(DEBUG)
	#define D_Scope(name) { ::lubee::ExceptionScope _exc_scope(#name, SOURCEPOS); try{
	#define D_ScopeEnd() } catch(...) { _exc_scope.log(); throw; } }
	// 一番目の関数を呼んだ時の例外を全てキャッチしてコールバック関数の引数にする
	namespace lubee {
		template <class Target, class CB, class CBE>
		void TryAndCatchAll(CB&& cb, CBE&& cbe) {
			try {
				cb();
			} catch(...) {
				CatchAll<Target>(cbe);
			}
		}
	}
#else
	#define D_Scope(name) {
	#define D_ScopeEnd() }
	namespace lubee {
		template <class Target, class CB, class CBE>
		void TryAndCatchAll(std::ostream&, CB&& cb, CBE&& /*cbe*/) {
			cb();
		}
	}
#endif
