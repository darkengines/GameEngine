#pragma once

#include <unordered_map>
#include <typeindex>
#include <queue>
#include "ComponentIndex.hpp"

namespace drk::Common {
	template<typename TIndex = uint32_t>
	struct Indexation {
		TIndex NextIndex;
		std::queue<TIndex> AvailableIndices;
	};

	template<typename TIndex = uint32_t>
	class IndexGenerator {
	public:
		template<typename T>
		ComponentIndex<T, TIndex> Generate() {
			const auto typeIndex = std::type_index(typeid(T));
			const auto& indexationPair = Indexations.find(typeIndex);
			if (indexationPair == Indexations.end()) {
				Indexation<TIndex> indexation = {.NextIndex = 1u, .AvailableIndices = {}};
				Indexations[typeIndex] = indexation;
				return ComponentIndex<T, TIndex>(0);
			}
			auto& indexation = indexationPair->second;
			if (!indexation.AvailableIndices.empty()) {
				auto index = indexation.AvailableIndices.front();
				indexation.AvailableIndices.pop();
				return ComponentIndex<T, TIndex>(index);
			} else {
				auto index = indexation.NextIndex++;
				return ComponentIndex<T, TIndex>(index);
			}
		}

		template<typename T>
		void Remove(TIndex index) {
			const auto typeIndex = std::type_index(typeid(T));
			auto &indexationPair = Indexations.find(typeIndex);
			if (indexationPair != Indexations.end()) {
				auto &indexation = indexationPair.second();
				indexation.availableIndices.push_back(index);
			}
		}

	protected:
		std::unordered_map<std::type_index, Indexation<TIndex>> Indexations;
	};
}