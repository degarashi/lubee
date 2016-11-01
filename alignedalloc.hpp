#pragma once
#include "error.hpp"
#include "bit.hpp"
#include "meta/compare.hpp"
#include <memory>

namespace lubee {
	namespace {
		namespace alignedalloc_i {
			using Offset_t = uint16_t;
			#pragma pack(push, 1)
			struct Preamble {
				Offset_t	offset;
			};
			struct PreambleA {
				uint16_t	length;
				Offset_t	offset;
			};
			#pragma pack(pop)
		}
	}
	//! アラインメントされたメモリ領域を確保
	/*!
		\param[in] nAlign	バイト境界 (1byte以上32768byte以下)
		\param[in] size		確保したいバイト数
		\return pair<ユーザーに返すアライン済みメモリ, プリアンブル構造体>
	*/
	template <class Pre>
	auto AlignedAlloc(const std::size_t nAlign, const std::size_t size) {
		D_Assert0(nAlign>=1 && nAlign<=(1<<(sizeof(alignedalloc_i::Offset_t)*8-1)));
		D_Assert0(bit::Count(nAlign)==1);		// nAlignは2の乗数倍

		const std::size_t sz = size + (nAlign-1) + sizeof(Pre);
		const auto ptr = reinterpret_cast<intptr_t>(std::malloc(sz));
		// アラインメントオフセット計算
		const intptr_t aptr = (ptr + sizeof(Pre) +nAlign-1) & ~(nAlign-1);
		// ユーザーに返すメモリ領域の前にずらした距離を記載
		auto *const pre = reinterpret_cast<Pre*>(aptr - sizeof(Pre));
		pre->offset = static_cast<alignedalloc_i::Offset_t>(aptr - ptr);
		struct {
			void*	ptr;
			Pre		pre;
		} ret{reinterpret_cast<void*>(aptr), *pre};
		return ret;
	}
	//! AlignedAllocで確保したメモリの解放
	/*!
		\param[in] ptr	開放したいメモリのポインタ(nullは不可)
	*/
	template <class Pre>
	void AlignedFree(void *const ptr) {
		D_Assert0(ptr);

		const auto iptr = reinterpret_cast<uintptr_t>(ptr);
		const auto *const pre = reinterpret_cast<Pre*>(iptr - sizeof(Pre));
		std::free(reinterpret_cast<void*>(iptr - pre->offset));
	}

	//! バイトアラインメント付きのメモリ確保
	/*! 解放はAFreeで行う */
	template <class T, class... Args>
	T* AAlloc(const std::size_t nAlign, Args&&... args) {
		return new(AlignedAlloc<alignedalloc_i::Preamble>(nAlign, sizeof(T)).ptr) T(std::forward<Args>(args)...);
	}
	//! バイトアラインメント付きの配列メモリ確保
	template <class T>
	T* AArray(const std::size_t nAlign, const int n) {
		// 先頭に要素数カウンタを置く
		const size_t size = (sizeof(T) + nAlign-1) & ~(nAlign-1);
		D_Assert0(size == sizeof(T));

		auto ret = AlignedAlloc<alignedalloc_i::PreambleA>(nAlign, size*n);
		ret.pre.length = n;
		auto ptr = reinterpret_cast<uintptr_t>(ret.ptr);
		for(int i=0 ; i<n ; i++) {
			new(reinterpret_cast<T*>(ptr)) T();
			ptr += size;
		}
		return reinterpret_cast<T*>(ret.ptr);
	}
	template <class T>
	void AFree(T *const ptr) {
		ptr->~T();
		AlignedFree<alignedalloc_i::Preamble>(ptr);
	}
	template <class T>
	void AArrayFree(T *const ptr) {
		using Pre = alignedalloc_i::PreambleA;
		{
			const auto *const pre = reinterpret_cast<Pre*>(reinterpret_cast<uintptr_t>(ptr) - sizeof(Pre));
			auto* uptr = ptr;
			int n = pre->length;
			while(n != 0) {
				uptr->~T();
				++uptr;
				--n;
			}
		}
		AlignedFree<Pre>(ptr);
	}

	//! アラインメントチェッカ
	/*! うっかり普通のnewでメモリを確保した時などのチェック用 */
	template <class T>
	class CheckAlign {
		protected:
			CheckAlign() {
				// アラインメントチェック
				D_Assert0((((uintptr_t)this)&(alignof(T)-1)) == 0);
			}
	};

	template <class T>
	class AAllocator {
		private:
			constexpr static std::size_t NAlign = alignof(T);
			static void AlignedDelete(void* ptr) {
				AFree(reinterpret_cast<T*>(ptr));
			}
			struct AlignedDeleter {
				void operator()(void* ptr) const {
					AFree(reinterpret_cast<T*>(ptr));
				}
			};
			static void ArrayDelete(void* ptr) {
				AArrayFree(reinterpret_cast<T*>(ptr));
			}
			struct ArrayDeleter {
				void operator()(void* ptr) const {
					AArrayFree(reinterpret_cast<T*>(ptr));
				}
			};

			//! 任意の引数によるctor
			template <class... Args>
			static T* _New(Args&&... args) {
				return AAlloc<T>(NAlign, std::forward<Args>(args)...); }
		public:
			//! アラインメント済みのメモリにオブジェクトを確保し、カスタムデリータ付きのunique_ptrとして返す
			template <class... Args>
			static auto NewU(Args&&... args) {
				return std::unique_ptr<T, AlignedDeleter>(_New(std::forward<Args>(args)...)); }
			static auto ArrayU(const std::size_t n) {
				return std::unique_ptr<T[], ArrayDeleter>(AArray<T>(NAlign, n)); }
			//! アラインメント済みのメモリにオブジェクトを確保し、関数型デリータ付きのunique_ptrとして返す
			template <class... Args>
			static auto NewUF(Args&&... args) {
				return std::unique_ptr<T, void(*)(void*)>(_New(std::forward<Args>(args)...), &AlignedDelete); }
			static auto ArrayUF(const std::size_t n) {
				return std::unique_ptr<T[], void(*)(void*)>(AArray<T>(NAlign,n), &ArrayDelete); }
			//! アラインメント済みのメモリにオブジェクトを確保し、shared_ptrとして返す
			template <class... Args>
			static auto NewS(Args&&... args) {
				return std::shared_ptr<T>(_New(std::forward<Args>(args)...), AlignedDeleter()); }
			static auto ArrayS(const std::size_t n) {
				return std::shared_ptr<T>(AArray<T>(NAlign,n), ArrayDeleter()); }
	};

	template <class T>
	class AlignedPool {
		private:
			constexpr static int Align = alignof(T);
			using OffsetType = alignedalloc_i::Offset_t;
			// Alignは2の累乗でなければならない
			using Check =
				std::enable_if_t<
					typename ArithmeticT<
						cbit::LowClear<Align>::result,
						Align
					>::equal{}
				>;
			// AlignはOffsetTypeで表せる範囲でなければならない
			using Check2 =
				std::enable_if_t<
					typename ArithmeticT<
						Align,
						(1 << sizeof(OffsetType)*8)
					>::less{}
				>;
			constexpr static int TSize = Arithmetic<sizeof(T), Align>::great;
			constexpr static int Mask = Align-1;

		public:
			// 型定義
			using size_type = std::size_t;
			using difference_type = std::ptrdiff_t;
			using pointer = T*;
			using const_pointer = const T*;
			using reference = T&;
			using const_reference = const T&;
			using value_type = T;

			// アロケータをU型にバインドする
			template <class U>
			struct rebind {
				using other = AlignedPool<U>;
			};

			//! コンストラクタ
			AlignedPool() noexcept {}
			AlignedPool(const AlignedPool&) noexcept {}
			//! デストラクタ
			~AlignedPool() noexcept {}

			//! メモリを割り当てる
			pointer allocate(const size_type num /*, typename AlignedPool<T,Align>::const_pointer hint = 0*/) {
				const auto ret = reinterpret_cast<uintptr_t>(std::malloc(TSize * num + Align));
				const uintptr_t ret2 = (ret + Align) & ~Mask;
				// オフセット記録
				*reinterpret_cast<OffsetType*>(ret2-sizeof(OffsetType)) = ret2 - ret;
				return reinterpret_cast<pointer>(ret2);
			}
			//! 割当て済みの領域を初期化する
			template <class... Ts>
			void construct(pointer p, Ts&&... ts) {
				new( (void*)p ) T(std::forward<Ts>(ts)...);
			}

			//! メモリを解放する
			void deallocate(pointer p, size_type /*num*/) {
				auto up = (uintptr_t)p;
				const auto diff = *reinterpret_cast<OffsetType*>(up-sizeof(OffsetType));
				up -= diff;
				std::free(reinterpret_cast<void*>(up));
			}
			//! 初期化済みの領域を削除する
			void destroy(pointer p) {
				p->~T();
			}

			//! アドレスを返す
			pointer address(reference value) const noexcept { return &value; }
			const_pointer address(const_reference value) const noexcept { return &value; }
			//! 割当てることができる最大の要素数を返す
			size_type max_size() const noexcept {
				return std::numeric_limits<std::size_t>::max() / TSize;
			}
	};
	namespace alignedalloc_i {}

	template <class T1, class T2>
	bool operator==(const AlignedPool<T1>&, const AlignedPool<T2>&) noexcept { return false; }
	template <class T>
	bool operator==(const AlignedPool<T>&, const AlignedPool<T>&) noexcept { return true; }

	template <class T1, class T2>
	bool operator!=(const AlignedPool<T1>&, const AlignedPool<T2>&) noexcept { return true; }
	template <class T>
	bool operator!=(const AlignedPool<T>&, const AlignedPool<T>&) noexcept { return false; }
}
