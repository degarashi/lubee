#pragma once
#include "sourcepos.hpp"
#include "niftycounter.hpp"
#include <memory>
#include <iostream>
#include <sstream>
#include <functional>

#ifdef ANDROID
	#include <android/log.h>
	#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "threaded_app", __VA_ARGS__))
	#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "threaded_app", __VA_ARGS__))
	/* For debug builds, always enable the debug traces in this library */
	#ifndef NDEBUG
	#  define LOGV(...)  ((void)__android_log_print(ANDROID_LOG_VERBOSE, "threaded_app", __VA_ARGS__))
	#else
	#  define LOGV(...)  ((void)0)
	#endif
#endif
namespace lubee {
	namespace log {
		struct Type {
			enum e {
				Error,
				Info,
				Verbose,
				_Num
			};
		};
		struct Output {
			virtual void print(Type::e type, const std::string& msg) = 0;
			virtual void print(Type::e type, const SourcePos& pos, const std::string& msg) = 0;
			virtual ~Output() {}
		};
		class DefaultOutput : public Output {
			protected:
				using ProcLog = std::function<void (const std::string&)>;
				ProcLog		_proc[Type::_Num] =
				{
					#ifdef ANDROID
						[](const auto& str){ LOGE(msg.c_str()); },
						[](const auto& str){ LOGI(msg.c_str()); },
						[](const auto& str){ LOGV(msg.c_str()); }
					#else
						[](const auto& str){ std::cerr << "<Error> " << str << std::endl; },
						[](const auto& str){ std::cerr << "<Info> " << str << std::endl; },
						[](const auto& str){ std::cout << "<Verbose> " << str << std::endl; }
					#endif
				};
			public:
				void print(const Type::e type, const std::string& msg) override {
					_proc[type](msg);
				}
				void print(const Type::e type, const SourcePos& pos, const std::string& msg) override {
					std::stringstream ss;
					ss << pos << std::endl << msg;
					print(type, ss.str());
				}
		};
		class Log {
			private:
				template <int Dummy=0>
				struct Out {
					using Out_UP = std::unique_ptr<Output>;
					static Out_UP	s_out;
				};

			public:
				static void Initialize() {
					Out<>::s_out = std::make_unique<DefaultOutput>();
				}
				static void Terminate() {}
				static void Output(const Type::e type, const std::string& s) {
					Out<>::s_out->print(type, s);
				}
				static void Output(const Type::e type, const SourcePos& pos, const std::string& msg) {
					Out<>::s_out->print(type, pos, msg);
				}
				static auto& GetOutput() noexcept {
					return Out<>::s_out;
				}
		};
		DEF_NIFTY_INITIALIZER(Log);

		template <int Dummy>
		typename Log::Out<Dummy>::Out_UP Log::Out<Dummy>::s_out;

		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wformat-security"
		template <class... Ts>
		std::string MakeMessage(const char* fmt, Ts&&... ts) noexcept {
			try {
				constexpr std::size_t BuffSize = 2048;
				char buff[BuffSize];
				std::snprintf(buff, BuffSize, fmt, std::forward<Ts>(ts)...);
				return std::string(buff);
			} catch(...) {
				// 何か例外が発生したら空のメッセージを返す
				return std::string();
			}
		}
		#pragma GCC diagnostic pop
	}
}
#define Log(type, ...)			::lubee::log::Log::Output(::lubee::log::Type::type, ::lubee::log::MakeMessage(__VA_ARGS__))
#define LogHere(type, ...)		::lubee::log::Log::Output(::lubee::log::Type::type, SOURCEPOS, ::lubee::log::MakeMessage(__VA_ARGS__))
