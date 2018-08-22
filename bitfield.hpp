#pragma once
#include <limits>
#include "meta/typelist.hpp"

namespace lubee {
	//! ビットフィールド定義用
	template <int Ofs, int Len>
	struct BitF {
		constexpr static int offset = Ofs,
							length = Len;
	};
	//! ワードに対するビット操作
	/*!
		特定のビットを対象として値を取得 & 代入
		\param Word	値を保持する型(unsigned)
		\param Ofs	下位からのビットオフセット
		\param Len	処理対象ビット幅
	*/
	template <class Word, int Ofs, int Len>
	class BitSub {
		private:
			const static Word MaskLow = (Word(1 << Len) - 1),
								Mask = MaskLow << Ofs;
			Word&	_word;

			// Tが非数値型だったりsignedな型ならコンパイルエラーを起こす
			using Limits = std::numeric_limits<Word>;
			constexpr static int Dummy[
				(
					Limits::is_specialized &&
					Limits::is_integer &&
					!Limits::is_signed
				) ? 1 : -1
			] = {};

		public:
			BitSub(Word& w):
				_word(w) {}

			template <class V>
			BitSub& operator = (const V v) {
				// ビットクリア
				_word &= ~Mask;
				_word |= (Word(v)<<Ofs & Mask);
				return *this;
			}
			operator Word() const {
				return get(_word);
			}
			static Word get(const Word& w) {
				return (w >> Ofs) & MaskLow;
			}
			//! 違う基本型やビット位置でも交換可能だが、サイズ違いは駄目
			template <class T_Word, int T_Ofs>
			void swap(BitSub<T_Word, T_Ofs, Len>& bs) noexcept {
				Word val0(*this);
				(*this) = Word(bs);
				bs = val0;
			}
	};
	//! テンプレートにてビットフィールドを定義
	/*!
		\tparam T		値を保持する型
		\tparam Args	ビットフィールドを定義するBitSubクラス
	*/
	template <class T, class... Ts>
	struct BitDef : Types<Ts...> {
		// Tが非数値型だったりsignedな型ならコンパイルエラーを起こす
		using Limits = std::numeric_limits<T>;
		constexpr static int UnsignedCheck[
			(
				Limits::is_specialized &&
				Limits::is_integer &&
				!Limits::is_signed
			) ? 1 : -1
		] = {};

		using Word = T;
	};
	//! ビットフィールドテンプレートクラス
	/*!
		\example
			struct MyDef : BitDef<uint32_t, BitF<0,14>, BitF<14,6>, BitF<20,12>> {	<br>
					enum { VAL0, VAL1, VAL2 }; };									<br>
			using Value = BitField<MyDef>;											<br>
			Value value;															<br>
			value.at<Value::VAL0>() = 128;											<br>
			int toGet = value.at<Value::VAL0>();
		\tparam BF BitDefクラス
	*/
	template <class BDef>
	class BitField : public BDef {
		public:
			// BDef(Types)から総サイズとオフセット計算
			constexpr static int maxbit = BDef::template MaxBit<>,
								buffsize = ((maxbit-1)/8)+1;
			using Word = typename BDef::Word;
		private:
			// 表現するビット列のサイズがWordより大きかったらコンパイルエラー
			constexpr static int WordCheck[buffsize > sizeof(Word) ? -1 : 1] = {};

			template <int N>
			using BFAt = typename BDef::template At<N>;
			template <int N>
			using BitSubT = BitSub<Word, BFAt<N>::offset, BFAt<N>::length>;

			Word _word;

		public:
			BitField() = default;
			BitField(const BitField& bf):
				_word(bf.value()) {}
			BitField(const Word& w):
				_word(w) {}
			void clear() {
				value() = 0;
			}
			Word& value() {
				return _word;
			}
			const Word& value() const {
				return _word;
			}

			//! ビット領域参照
			template <int N>
			BitSubT<N> at() {
				return BitSubT<N>(_word);
			}
			//! ビットフィールド値取得
			template <int N>
			Word at() const {
				return BitSubT<N>::get(_word);
			}
			//! ビット幅マスク取得
			template <int N>
			constexpr static Word Length_mask() {
				return (Word(1) << BFAt<N>::length) - 1;
			}
			//! ビットマスク取得
			template <int N>
			constexpr static Word Mask() {
				auto ret = Length_mask<N>();
				return ret << BFAt<N>::offset;
			}

			//! ビットフィールドで使う場所以外をゼロクリアした値を取得
			Word cleanedValue() const {
				constexpr Word msk = (maxbit >= 64) ? ~0 : ((uint64_t(1) << maxbit)-1);
				return value() & msk;
			}
			bool operator == (const BitField& bf) const {
				return cleanedValue() == bf.cleanedValue(); }
			bool operator != (const BitField& bf) const {
				return !(this->operator == (bf)); }
			bool operator < (const BitField& bf) const {
				return cleanedValue() < bf.cleanedValue(); }
	};
}
