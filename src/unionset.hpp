#pragma once
#include <vector>
#include <numeric>
#include <cstddef>

namespace lubee {
	class UnionSet {
		private:
			using id_t = unsigned int;
			using Vec = std::vector<id_t>;
			Vec		_parent;

		public:
			UnionSet(const id_t n):
				_parent(n)
			{
				std::iota(_parent.begin(), _parent.end(), 0);
			}
			id_t getRoot(const id_t id) noexcept {
				id_t cur = id;
				for(;;) {
					// 親Id == 自分のIdならもう親が居ない
					if(_parent[cur] == cur) {
						// 次回からの探索を効率化
						_parent[id] = cur;
						return cur;
					}
					cur = _parent[cur];
				}
			}
			bool isSame(const id_t id0, const id_t id1) noexcept {
				return getRoot(id0) == getRoot(id1);
			}
			void merge(const id_t id0, const id_t id1) noexcept {
				_parent[getRoot(id1)] = id0;
			}
			size_t size() const noexcept {
				return _parent.size();
			}
	};
}
