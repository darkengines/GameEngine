#pragma once

#include <cstdint>
#include <vector>
#include <functional>
#include <entt/entity/registry.hpp>
#include "../Stores/Store.hpp"
#include "../Stores/StoreItem.hpp"

namespace drk::Graphics {
	template<typename TModel>
	class SynchronizationState {
	public:
		SynchronizationState(uint32_t storeCount) : RemainingCount(storeCount), Stores(storeCount) {
			for (auto storeIndex = 0u; storeIndex < Stores.size(); storeIndex++) {
				Stores[storeIndex] = false;
			}
		};

		bool Update(uint32_t index) {
			if (!Stores[index]) {
				Stores[index] = true;
				RemainingCount--;
			}
			return RemainingCount;
		};

		template<typename ...TComponents>
		static void Update(
			entt::registry &registry,
			uint32_t frameIndex,
			std::function<void(TModel &model, const TComponents &... components)> updater
		) {
			auto entities =
				registry.view<Stores::StoreItem<TModel>, SynchronizationState<TModel>, TComponents...>();
			entities.each(
				[&registry, &frameIndex, &updater](
					const auto entity,
					const auto &storeItem,
					auto &synchronizationState,
					const TComponents &...
					components
				) {
					if (!synchronizationState.Update(frameIndex)) {
						registry.remove<SynchronizationState<TModel >>(entity);
					}
					auto model = storeItem.frameStoreItems[frameIndex].pItem;
					updater(*model, components...);
				}
			);
		}

	protected:
		uint32_t RemainingCount;
		std::vector<bool> Stores;
	};
}