
#include "SceneSystem.hpp"
#include "Draws/SceneDraw.hpp"
#include "../Draws/DrawSystem.hpp"

namespace drk::Scenes {
	SceneSystem::SceneSystem(entt::registry& registry, Engine::EngineState& engineState) :
		registry(registry),
		engineState(engineState) {

	}
	void SceneSystem::UpdateDraws() {
		registry.sort<Scenes::Draws::SceneDraw>(
			[](
				const Draws::SceneDraw& leftDraw,
				const Draws::SceneDraw& rightDraw
			) {
				auto result =
					leftDraw.hasTransparency < rightDraw.hasTransparency; // Sort by hasTransparency in ascending order
				if (leftDraw.
					hasTransparency && rightDraw
						.hasTransparency) {
					result |= leftDraw.depth < rightDraw.
						depth; // If both have transparency, sort by depth in ascending order
				}
				result |= leftDraw.drawSystem < rightDraw.
					drawSystem; // Sort by pipeline in ascending order
				result |= leftDraw.indexBufferView.byteOffset < rightDraw.indexBufferView.byteOffset; // Sort by index buffer view in ascending order
				return
					result;
			}
		);

		auto& frameState = engineState.getCurrentFrameState();
		auto& drawStore = frameState.getUniformStore<Scenes::Draws::SceneDraw>();

		auto sceneDrawEntities = registry.view<Scenes::Draws::SceneDraw>();

		auto drawIndex = 0u;
		sceneDrawEntities.each(
			[&drawIndex, &drawStore](entt::entity sceneDrawEntity, Draws::SceneDraw& draw) {
				draw.drawSystem->UpdateDraw(sceneDrawEntity, drawIndex);
				drawIndex++;
			}
		);
	}
}