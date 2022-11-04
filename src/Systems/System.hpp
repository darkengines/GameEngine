#pragma once

#include <entt/entt.hpp>
#include "../Engine/EngineState.hpp"

namespace drk::Systems {

	template<typename TModel, typename ...TComponents>
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
				auto storeItem = GetStoreItem<TModel>();
				registry.emplace<Stores::StoreItem<TModel>>(
					entity,
					storeItem
				);
				registry.emplace<Graphics::SynchronizationState<TModel>>(entity, engineState.frameStates.size());
			}
		}
	};

	template<typename TDrawModel, typename ...TComponents>
	class DrawSystem {
	protected:
		Engine::EngineState& engineState;
		entt::registry& registry;

	public:
		DrawSystem(Engine::EngineState& engineState, entt::registry& registry)
			: engineState(engineState), registry(registry) {}
		virtual void Update(TDrawModel& model, const TComponents& ... components) = 0;
		void Store() {
			auto componentView = registry.view<TComponents> (entt::exclude<Stores::StoreItem<TDrawModel>>);
			auto drawModelStoreStore = engineState.frameStates[engineState.frameIndex].getUniformStore<TDrawModel>();
			auto drawIndex = 0u;
			componentView.each(
				[&drawModelStoreStore, &drawIndex, this](
					const entt::entity& entity,
					const TComponents& ... components
			)
			{
				auto& pointDrawStoreItem = drawModelStoreStore->Get(drawIndex);
				registry.emplace<TDrawModel>(
					entity,
					pointDrawStoreItem
				);
				drawIndex++;
			}
			);
		}
	};
}