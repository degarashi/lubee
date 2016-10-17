#pragma once

namespace lubee {
	//! static変数の初期化を制御する
	template <class T>
	class NiftyCounterIdiom : T {
		private:
			static int s_niftyCounter;
		public:
			NiftyCounterIdiom() {
				if(s_niftyCounter++ == 0) {
					T::Initialize();
				}
			}
			~NiftyCounterIdiom() {
				if(--s_niftyCounter == 0) {
					T::Terminate();
				}
			}
	};
	template <class T>
	int NiftyCounterIdiom<T>::s_niftyCounter = 0;

	#define DEF_NIFTY_INITIALIZER(name)	static class name##_initializer : public ::lubee::NiftyCounterIdiom<name> {} name##_initializer_obj
}
