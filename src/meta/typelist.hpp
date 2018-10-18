#pragma once
#include "compare.hpp"
#include <tuple>

namespace lubee {
	template <class... Ts>
	class Types;

	template <class T>
	struct TypesFromTuple;
	template <class... Ts>
	struct TypesFromTuple<std::tuple<Ts...>> {
		using type = Types<Ts...>;
	};
	template <class T>
	using TypesFromTuple_t = typename TypesFromTuple<T>::type;

	template <class... Ts>
	class Types {
		protected:
			template <class>
			struct _CallType {
				using type = Types<decltype(std::declval<Ts>()())...>;
			};
			//! 特定の型を取り出す
			template <int N, class... Ts2>
			struct _At;
			template <int N, class T, class... Ts2>
			struct _At<N, T, Ts2...> {
				using type = typename _At<N-1, Ts2...>::type;
			};
			template <class T, class... Ts2>
			struct _At<0,T,Ts2...> {
				using type = T;
			};

			//! タイプリストが指定のs型を含んでいるか
			template <class T, int N, class... Ts2>
			struct _Find;
			template <class T, int N, class T0, class... Ts2>
			struct _Find<T,N,T0,Ts2...> {
				constexpr static int result = _Find<T,N+1,Ts2...>::result;
			};
			template <class T, int N, class... Ts2>
			struct _Find<T,N,T,Ts2...> {
				constexpr static  int result = N;
			};
			template <class T, int N>
			struct _Find<T,N> {
				constexpr static int result = -1;
			};

			//! タイプリストを逆順にした物を返す（ただしTypes<Types<A,B>, C> の形になる）
			/*! Plainと組み合わせて使う */
			template <class... Ts2>
			struct _Reverse {
				using type = Types<>;
			};
			template <class T>
			struct _Reverse<T> {
				using type = Types<T>;
			};
			template <class T, class... Ts2>
			struct _Reverse<T,Ts2...> {
				using type = Types<typename _Reverse<Ts2...>::type, T>;
			};

			//! Types<Types<...>, ...> の入れ子を展開する
			template <class... Ts2>
			struct _Plain {
				using type = Types<Ts2...>;
			};
			template <class T>
			struct _Plain<Types<T>> {
				using type = T;
			};
			template <class... Ts0, class... Ts1>
			struct _Plain<Types<Ts0...>, Ts1...> {
				using type = typename _Plain<Ts0..., Ts1...>::type;
			};
			template <class... Ts0, class... Ts1>
			struct _Plain<Types<Types<Ts0...>, Ts1...>> {
				using type = typename _Plain<Ts0..., Ts1...>::type;
			};

			//! タイプリストの変数を並べた時に消費するメモリ量を計算
			template <class... Ts2>
			struct _Sum {
				constexpr static int result = 0;
			};
			template <class T, class... Ts2>
			struct _Sum<T, Ts2...> {
				constexpr static int result = sizeof(T)+_Sum<Ts2...>::result;
			};

			//! 通常の型サイズ取得クラス
			template <class T>
			struct _GetSize_Normal {
				static constexpr int get() { return sizeof(T); }
			};

			//! ビットフィールド用: 必要なビット数を計算
			template <class... Ts2>
			struct _MaxBit {
				constexpr static int result = 0;
			};
			template <class T, class... Ts2>
			struct _MaxBit<T,Ts2...> {
				constexpr static int result = Arithmetic<T::offset+T::length, _MaxBit<Ts2...>::result>::great;
			};

			//! インデックス指定による先頭からのサイズ累計
			template <int N, template <class> class Getter, class... Ts2>
			struct _SumN {
				constexpr static int result = 0; };
			template <int N, template <class> class Getter, class T0, class... Ts2>
			struct _SumN<N, Getter, T0, Ts2...> {
				constexpr static int result = Getter<T0>::get() * (N>0 ? 1 : 0) + _SumN<N-1, Getter, Ts2...>::result; };

			//! タイプ指定による先頭からのサイズ累計
			template <class T, template <class> class Getter, class... Ts2>
			struct _SumT;
			template <class T, template <class> class Getter, class T0, class... Ts2>
			struct _SumT<T, Getter, T0, Ts2...> {
				constexpr static int result = Getter<T0>::get() + _SumT<T, Getter, Ts2...>::result; };
			template <class T, template <class> class Getter, class... Ts2>
			struct _SumT<T, Getter, T, Ts2...> {
				constexpr static int result = 0; };

		public:
			//! インデックス指定で型を取り出す
			template <int N>
			using At = typename _At<N,Ts...>::type;
			//! 末尾に型を追加
			template <class... Ts2>
			using Append = Types<Ts..., Ts2...>;
			//! 先頭に型を追加
			template <class... Ts2>
			using Prepend = Types<Ts2..., Ts...>;
			//! 型のインデックス検索
			template <class T>
			constexpr static int Find = _Find<T,0,Ts...>::result;
			//! 型を持っていればintegral_constant<bool,true>
			template <class T>
			struct Has : std::integral_constant<bool, (Find<T> >= 0)> {};
			//! 別の型のテンプレート引数に使う
			template <template <class...> class T>
			using AsTArgs = T<Ts...>;
			//! std::tupleに変換
			using AsTuple = AsTArgs<std::tuple>;
			//! 要素のそれぞれについてoperator()をした結果の型リストを生成
			template <class T=void>
			using CallType = typename _CallType<T>::type;

			//! 型リストを逆順にする
			using Reverse = typename _Plain<typename _Reverse<Ts...>::type>::type;
			//! 自身の型
			using type = Types<Ts...>;

			constexpr static int size = sizeof...(Ts);						//!< 型リストの要素数
			//! タイプリストの位置に対する比較(LessThan)
			template <class T0, class T1>
			using Less = typename ArithmeticT< Find<T0>, Find<T1> >::less;
			//! タイプリストの位置に対する比較(GreaterThan)
			template <class T0, class T1>
			using Greater = typename ArithmeticT< Find<T0>, Find<T1> >::greater;
			//! タイプリストの位置に対する比較(Equal)
			template <class T0, class T1>
			using Equal = std::is_same<T0,T1>;
			//! タイプリストの位置に対する比較(LessThan or Equal)
			template <class T0, class T1>
			using LessEq = typename ArithmeticT< Find<T0>, Find<T1> >::less_eq;
			//! タイプリストの位置に対する比較(GreaterThan or Equal)
			template <class T0, class T1>
			using GreatEq = typename ArithmeticT< Find<T0>, Find<T1> >::great_eq;

			//! ビットフィールド用: 必要なビット数を計算
			//! 各タイプの::offset, ::lengthフィールドを使って計算
			// (型がimcompleteなケースを考えてテンプレートクラスとしてある)
			template <class=void>
			constexpr static int MaxBit = _MaxBit<Ts...>::result;
			//! インデックス指定による先頭からのサイズ累計
			template <int N, template <class> class Getter=_GetSize_Normal>
			constexpr static int SumN = _SumN<N, Getter, Ts...>::result;
			//! タイプ指定による先頭からのサイズ累計
			template <class T, template <class> class Getter=_GetSize_Normal>
			constexpr static int SumT = _SumT<T, Getter, Ts...>::result;
	};
	template <class... Ts>
	class TypesS : public Types<Ts...> {
		private:
			using base_t = Types<Ts...>;
			using S = typename base_t::template _Sum<Ts...>;
		public:
			constexpr static int sum = S::result,						//!< 要素のサイズ合計
								maxsize = IMax<sizeof(Ts)...>::result;	//!< 要素の最大サイズ
	};
}
