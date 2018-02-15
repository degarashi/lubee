#pragma once
#include "sourcepos.hpp"
#include "niftycounter.hpp"
#include <memory>
#include <iostream>
#include <sstream>

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
				Verbose
			};
		};
		struct Output {
			virtual void print(Type::e type, const std::string& msg) = 0;
			virtual void print(Type::e type, const SourcePos& pos, const std::string& msg) = 0;
			virtual ~Output() {}
		};
		struct DefaultOutput : Output {
			void print(const Type::e type, const std::string& msg) override {
				switch(type) {
					case Type::Error:
						#ifdef ANDROID
							LOGE(msg.c_str());
						#else
							std::cerr << "<Error> " << msg << std::endl;
						#endif
						break;
					case Type::Info:
						#ifdef ANDROID
							LOGI(msg.c_str());
						#else
							std::cerr << "<Info> " << msg << std::endl;
						#endif
						break;
					case Type::Verbose:
						#ifdef ANDROID
							LOGV(msg.c_str());
						#elif not defined(NDEBUG)
							std::cout << "<Verbose> " << msg << std::endl;
						#endif
						break;
				}
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
