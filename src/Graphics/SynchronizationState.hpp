#pragma once

#include <cstdint>
#include <vector>
#include <entt/entity/registry.hpp>

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

		template<typename TComponent>
		static void Update(
			entt::registry &registry,
			uint32_t frameIndex,
			std::function<void(const TComponent &component, TModel &model)> updater
		) {
			auto entities = registry.view<TComponent, Graphics::StoreItem<TModel>, Graphics::SynchronizationState<TModel>>();
			for (auto entity: entities) {
				auto &component = registry.get<TComponent>(entity);
				auto &storeItem = registry.get<Graphics::StoreItem<TModel>>(entity);
				auto &synchronizationState = registry.get<Graphics::SynchronizationState<TModel>>(
					entity
				);

				if (!synchronizationState.Update(frameIndex)) {
					registry.remove<Graphics::SynchronizationState<TModel>>(entity);
				}
				auto model = storeItem.frameStoreItems[frameIndex].pItem;
				updater(component, *model);
			}
		}

	protected:
		uint32_t RemainingCount;
		std::vector<bool> Stores;
	};
}