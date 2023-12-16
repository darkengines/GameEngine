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

	public:
		System(Engine::EngineState& engineState, entt::registry& registry)
			: engineState(engineState), registry(registry) {}
		virtual void Update(TModel& model, const TComponents& ... components) = 0;
		void Store() {
			auto entities = registry.view<TComponents...>(entt::exclude<Stores::StoreItem<TModel>>);
			for (const auto entity : entities) {
				auto storeItem = engineState.GetStoreItem<TModel>();
				registry.emplace<Stores::StoreItem<TModel>>(
					entity,
					storeItem
				);
				registry.emplace<Graphics::SynchronizationState<TModel>>(entity, engineState.getFrameCount());
			}
		}
		void UpdateStore() {
			auto updater = [this](TModel& model, const TComponents&... components) {
				this->Update(model, components...);
				};
			Graphics::SynchronizationState<TModel>::template Update<TComponents...>(
				registry,
				engineState.getFrameIndex(),
				updater
			);
		}
	};
}