#pragma once

#include <entt/entt.hpp>
#include "../Engine/EngineState.hpp"
#include "../Graphics/SynchronizationState.hpp"

namespace drk::Systems {

	template<typename TModel, typename... TComponents>
	class System {
	protected:
		Engine::EngineState& engineState;
		entt::registry& registry;
		uint32_t itemCount;

	public:
		System(Engine::EngineState& engineState, entt::registry& registry)
			: engineState(engineState), registry(registry), itemCount(0) {}
		virtual void update(TModel& model, const TComponents& ... components) = 0;
		uint32_t getItemCount() { return itemCount; }
		void store() {
			auto entities = registry.view<TComponents...>(entt::exclude<Stores::StoreItem<TModel>>);
			for (const auto entity : entities) {
				auto storeItem = engineState.GetStoreItem<TModel>();
				registry.emplace<Stores::StoreItem<TModel>>(
					entity,
					storeItem
				);
				registry.emplace<Graphics::SynchronizationState<TModel>>(entity, engineState.getFrameCount());
				itemCount++;
			}
		}
		void updateStore() {
			std::function<void(TModel& model, const TComponents& ...)> updater = [this](TModel& model, const TComponents&... components) {
				this->update(model, components...);
				};
			Graphics::update<TModel, TComponents...>(
				registry,
				engineState.getFrameIndex(),
				updater
			);
		}
	};
}