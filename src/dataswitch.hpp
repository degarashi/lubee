#pragma once
#include <cstdint>

namespace lubee {
	template <class T>
	class DataSwitcher {
		private:
			T				_data[2];
			uint_fast8_t	_sw = 0;
		public:
			void clear() {
				for(auto& t : _data)
					t = T();
				_sw = 0;
			}
			T& current() {
				return _data[_sw];
			}
			const T& current() const {
				return _data[_sw];
			}
			T& prev() {
				return _data[_sw^1];
			}
			const T& prev() const {
				return _data[_sw^1];
			}
			//! データフラグを切り替え
			void advance() {
				_sw ^= 1;
			}
			//! フラグ切り替えと同時に内容をデフォルトコンストラクタでクリア
			void advance_clear() {
				advance();
				current() = T();
			}
	};
}
