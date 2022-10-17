#include "SpatialSystem.hpp"
#include "../Objects/Relationship.hpp"
#include "../Objects/Dirty.hpp"
#include <algorithm>
#include <glm/gtx/quaternion.hpp>
#include <entt/entt.hpp>

namespace drk::Spatials {

	SpatialSystem::SpatialSystem(
		drk::Devices::DeviceContext *pContext,
		drk::Graphics::EngineState *pState
	)
		: DeviceContext(pContext), EngineState(pState) {}

	void SpatialSystem::AddSpatialSystem(entt::registry &registry) {
		registry.on_construct<Spatial>().connect<SpatialSystem::OnSpatialConstruct>();
	}

	void SpatialSystem::RemoveSpatialSystem(entt::registry &registry) {
		registry.on_construct<Spatial>().disconnect<SpatialSystem::OnSpatialConstruct>();
	}

	void SpatialSystem::OnSpatialConstruct(entt::registry &registry, entt::entity spatialEntity) {
		registry.emplace<Objects::Dirty<Spatial>>(spatialEntity);
	}

	void SpatialSystem::UpdateStoreItem(const Spatial &spatial, Models::Spatial &spatialModel) {
		spatialModel = {
			.relativeScale = spatial.relativeScale,
			.relativeRotation = spatial.relativeRotation,
			.relativePosition = spatial.relativePosition,
			.absoluteScale = spatial.absoluteScale,
			.absoluteRotation = spatial.absoluteRotation,
			.absolutePosition = spatial.absolutePosition,
			.relativeModel = spatial.relativeModel,
			.absoluteModel = spatial.absoluteModel
		};
	}

	void SpatialSystem::StoreSpatials() {
		EngineState->Store<Models::Spatial, Spatial>();
	}

	void MakeDirty(const entt::entity spatialEntity) {

	}

	void SpatialSystem::PropagateChanges() {
		EngineState->Registry.sort<Spatial>(
			[&](const entt::entity left, const entt::entity right) {
				const auto &rightRelation = EngineState->Registry.get<Objects::Relationship>(right);
				return left == rightRelation.parent || left == rightRelation.previousSibling;
			}
		);
		auto entities = EngineState->Registry.view<Objects::Dirty<Spatial>>();
		for (const auto entity : entities) {
			const auto &relationship = EngineState->Registry.get<Objects::Relationship>(entity);
			auto &spatial = EngineState->Registry.get<Spatial>(entity);

			auto translationMatrix = glm::translate(glm::identity<glm::mat4>(), glm::vec3(spatial.relativePosition));
			auto rotationMatrix = glm::toMat4(spatial.relativeRotation);
			auto scalingMatrix = glm::scale(glm::identity<glm::mat4>(), glm::vec3(spatial.relativeScale));
			spatial.relativeModel = translationMatrix * rotationMatrix * scalingMatrix;

			std::optional<glm::mat4> parentAbsoluteModel = std::nullopt;
			auto worldModel = spatial.relativeModel;
			if (relationship.parent != entt::null) {
				auto &parentSpatial = EngineState->Registry.get<Spatial>(relationship.parent);
				parentAbsoluteModel = parentSpatial.absoluteModel;
			}

			spatial.absoluteModel = parentAbsoluteModel.has_value() ? parentAbsoluteModel.value() *
																	  spatial.relativeModel : spatial.relativeModel;

			EngineState->Registry.remove<Objects::Dirty<Spatial>>(entity);
			EngineState->Registry.emplace_or_replace<Graphics::SynchronizationState<Models::Spatial>>(
				entity,
				(uint32_t) EngineState->FrameStates.size());
		}
	}

	void SpatialSystem::UpdateSpatials() {
		Graphics::SynchronizationState<Models::Spatial>::Update<Spatial>(
			EngineState->Registry,
			EngineState->FrameIndex,
			[=](
				Models::Spatial &model,
				const Spatial component
			) { UpdateStoreItem(component, model); }
		);
	}
}
