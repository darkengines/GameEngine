#pragma once

#include <entt/entt.hpp>
#include <taskflow/taskflow.hpp>

#include "./IStorageSystem.hpp"
#include "../Engine/EngineState.hpp"
#include "../Graphics/SynchronizationState.hpp"

namespace drk::Systems {

template<typename TModel, typename... TComponents>
class StorageSystem: public IStorageSystem {
protected:
	Engine::EngineState& engineState;
	entt::registry& registry;
	uint32_t itemCount;

public:
	StorageSystem(Engine::EngineState& engineState, entt::registry& registry) : engineState(engineState), registry(registry), itemCount(0) {
          const auto itemView = registry.view<TComponents...>(entt::exclude<Stores::StoreItem<TModel>>);
          const auto synchronizationView = registry.view<Stores::StoreItem<TModel>, Graphics::SynchronizationState<TModel>, TComponents...>();
    }
	virtual void update(TModel& model, const TComponents&... components) = 0;
	uint32_t getItemCount() { return itemCount; }
	void setup(tf::Taskflow& taskflow) {
		auto [store, update] = taskflow.emplace([this]() { this->store(); }, [this]() { this->update(); });
	}
	void store() {
		auto entities = registry.view<TComponents...>(entt::exclude<Stores::StoreItem<TModel>>);
		for (const auto entity : entities) {
			auto storeItem = engineState.GetStoreItem<TModel>();
			registry.emplace<Stores::StoreItem<TModel>>(entity, storeItem);
			registry.emplace_or_replace<Graphics::SynchronizationState<TModel>>(entity, engineState.getFrameCount());
			itemCount++;
		}
	}
	void updateStore() {
		std::function<void(TModel & model, const TComponents&...)> updater = [this](TModel& model, const TComponents&... components) { this->update(model, components...); };
		Graphics::update<TModel, TComponents...>(registry, engineState.getFrameIndex(), updater);
	}
};
}  // namespace drk::Systems
