#pragma once

namespace lubee {
	const static struct none_t{
		template <class Ar>
		void serialize(Ar&) {}
	} none;
}
