#pragma once

#include <cstdint>
#include <typeindex>

namespace drk {
	typedef uint32_t Index;

	template<typename TComponent, typename TIndex = Index>
	struct ComponentIndex {
		TIndex index;
		std::type_index typeIndex;
	};
}