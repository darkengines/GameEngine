#pragma once

#include <cstdint>
#include <typeindex>

namespace drk::Common {
	typedef uint32_t Index;

	template<typename TComponent, typename TIndex = Index>
	struct ComponentIndex {
		ComponentIndex(TIndex index) : Index(index), TypeIndex(std::type_index(typeid(TComponent))) {}
		ComponentIndex(const ComponentIndex<TComponent, TIndex>& other)
			: Index(other.Index), TypeIndex(other.TypeIndex) {}

		TIndex Index;
		std::type_index TypeIndex;

		bool operator==(const ComponentIndex<TComponent, TIndex>& right) const {
			return Index == right.Index;
		}

		bool operator!=(const ComponentIndex<TComponent, TIndex>& right) const {
			return Index != right.Index;
		}

		operator TIndex() const {
			return Index;
		}
	};
}