#pragma once
#include <algorithm>
#include <utility>
#include <iterator>

namespace lubee {
	//! 単純挿入ソート
	template <class Itr, class Cmp>
	void insertion_sort(Itr itrB, Itr itrE, const Cmp& cmp) {
		if(std::distance(itrB, itrE) < 2)
			return;

		// ソート済のカーソル(直前までがソート済み)
		Itr sortEnd = std::next(itrB);
		for(;;) {
			Itr r = sortEnd,
				l = std::prev(r);
			auto tmp = std::move(*r);
			while(cmp(tmp, *l)) {
				*r = std::move(*l);
				--r;
				if(l == itrB)
					break;
				--l;
			}
			*r = std::move(tmp);
			// ソート済みカーソルがリストの最後まで達したら終了
			if(++sortEnd == itrE)
				break;
		}
	}
	//! 単純挿入ソート (リスト用)
	template <class List, class Cmp>
	void insertion_sort_list(List& l, typename List::iterator itrB, typename List::iterator itrE, const Cmp& cmp) {
		if(std::distance(itrB, itrE) < 2)
			return;

		auto itrLE = std::next(itrB);
		for(;;) {
			auto itrL = itrLE;
			do {
				--itrL;
				if(cmp(*itrL, *itrLE)) {
					++itrL;
					break;
				}
			} while(itrL != itrB);

			if(itrL != itrLE) {
				auto tmp = std::move(*itrLE);
				itrLE = l.erase(itrLE);
				auto itr = l.insert(itrL, std::move(tmp));
				if(itrL == itrB)
					itrB = itr;
			} else
				++itrLE;
			if(itrLE == itrE)
				break;
		}
	}
}

