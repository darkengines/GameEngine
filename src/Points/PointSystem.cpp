
#include "PointSystem.hpp"
#include "../Materials/Models/Material.hpp"
#include "../Cameras/Components/Camera.hpp"
#include "../Spatials/Components/Spatial.hpp"
#include "Components/PointDraw.hpp"
#include "Models/PointDraw.hpp"
#include "../Scenes/Draws/SceneDraw.hpp"
#include "PointPrimitivePipeline.hpp"

namespace drk::Points {
	PointSystem::PointSystem(Engine::EngineState& engineState, entt::registry& registry)
		: Systems::System<Models::Point, Components::Point>(engineState, registry) {

	}
	void PointSystem::Update(Models::Point& model, const Components::Point& point) {
		const auto& materialModel = registry.get<Stores::StoreItem<Materials::Models::Material>>(point.materialEntity);
		model.materialItemLocation = materialModel.frameStoreItems[engineState.getFrameIndex()];
	}
	void PointSystem::UpdateDraw(entt::entity drawEntity, int drawIndex) {
		const auto& pointDraw = registry.get<Models::PointDraw>(drawEntity);
		auto& frameState = engineState.getCurrentFrameState();
		//todo: optimization - fetch uniform store in parent scope and give as argument
		auto& drawStore = frameState.getUniformStore<Models::PointDraw>();
		const auto& pointItemLocation = drawStore.get(drawIndex);
		pointItemLocation.pItem->pointItemLocation.storeIndex = pointDraw.pointItemLocation.storeIndex;
		pointItemLocation.pItem->pointItemLocation.itemIndex = pointDraw.pointItemLocation.itemIndex;
	}
	void PointSystem::EmitDraws() {
		auto pointEntities = registry.view<Stores::StoreItem<Models::Point>, Components::Point, Spatials::Components::Spatial>();
		auto cameraEntity = engineState.CameraEntity;
		auto camera = registry.get<Cameras::Components::Camera>(cameraEntity);
		pointEntities.each(
			[&](
				entt::entity pointEntity,
				auto& pointStoreItem,
				auto& point,
				auto& spatial
			) {
				const auto& pointStoreItemLocation = pointStoreItem.frameStoreItems[engineState.getFrameIndex()];
				const auto& material = registry.get<Materials::Components::Material*>(pointEntity);
				Scenes::Draws::SceneDraw draw = {
					.drawSystem = this,
					.indexBufferView = nullptr,
					.vertexBufferView = nullptr,
					.hasTransparency = point.pMaterial->hasTransparency,
					.depth = glm::distance(camera.absolutePosition, spatial.absolutePosition)
				};
				Models::PointDraw pointDraw = {
					.pointItemLocation = pointStoreItemLocation,
				};

				auto entity = registry.create();
				registry.emplace<Scenes::Draws::SceneDraw>(entity, draw);
				registry.emplace<Models::PointDraw>(entity, pointDraw);
			}
		);
	}
	Draws::DrawVertexBufferInfo PointSystem::GetVertexBufferInfo(entt::entity drawEntity) {
		return Draws::DrawVertexBufferInfo{0, 0, 0};
	}
}
