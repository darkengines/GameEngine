
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
			[](const Draws::SceneDraw& leftDraw, const Draws::SceneDraw& rightDraw) {
				if (leftDraw.hasTransparency < rightDraw.hasTransparency)
					return true;
				if (leftDraw.hasTransparency > rightDraw.hasTransparency)
					return false;

				if (leftDraw.hasTransparency && rightDraw.hasTransparency) {
					if (leftDraw.depth < rightDraw.depth)
						return true;
					if (leftDraw.depth > rightDraw.depth)
						return false;
				}

				if (leftDraw.drawSystem < rightDraw.drawSystem)
					return true;
				if (leftDraw.drawSystem > rightDraw.drawSystem)
					return false;

				if (leftDraw.indexBufferView.byteOffset < rightDraw.indexBufferView.byteOffset)
					return true;

				return false;
			}
		);

		auto& frameState = engineState.getCurrentFrameState();
		auto& drawStore = frameState.getUniformStore<Scenes::Draws::SceneDraw>();

		auto sceneDrawEntities = registry.view<Scenes::Draws::SceneDraw>();

		auto drawIndex = 0u;
		//std::cout << "----------------------" << std::endl;
		sceneDrawEntities.each(
			[&drawIndex, &drawStore](entt::entity sceneDrawEntity, Draws::SceneDraw& draw) {
				//std::cout << draw.hasTransparency << " " << draw.depth << std::endl;
				draw.drawSystem->UpdateDraw(sceneDrawEntity, drawIndex);
				drawIndex++;
			}
		);
		//std::cout << "----------------------" << std::endl;
	}
}