#include "SpatialSystem.hpp"
#include "../Objects/Relationship.hpp"
#include "../Objects/Dirty.hpp"
#include "../Objects/Object.hpp"
#include <algorithm>
#include <glm/gtx/quaternion.hpp>
#include <entt/entt.hpp>
#include <string>
#include <fmt/format.h>

namespace drk::Spatials {

	SpatialSystem::SpatialSystem(
		drk::Devices::DeviceContext* pContext,
		drk::Graphics::EngineState* pState
	)
		: DeviceContext(pContext), EngineState(pState) {}

	void SpatialSystem::AddSpatialSystem(entt::registry& registry) {
		registry.on_construct<Spatial>().connect<SpatialSystem::OnSpatialConstruct>();
	}

	void SpatialSystem::RemoveSpatialSystem(entt::registry& registry) {
		registry.on_construct<Spatial>().disconnect<SpatialSystem::OnSpatialConstruct>();
	}

	void SpatialSystem::OnSpatialConstruct(entt::registry& registry, entt::entity spatialEntity) {
		registry.emplace<Objects::Dirty<Spatial>>(spatialEntity);
	}

	void SpatialSystem::UpdateStoreItem(const Spatial& spatial, Models::Spatial& spatialModel) {
		spatialModel.relativeScale = spatial.relativeScale;
		spatialModel.relativeRotation = spatial.relativeRotation;
		spatialModel.relativePosition = spatial.relativePosition;
		spatialModel.absoluteScale = spatial.absoluteScale;
		spatialModel.absoluteRotation = spatial.absoluteRotation;
		spatialModel.absolutePosition = spatial.absolutePosition;
		spatialModel.relativeModel = spatial.relativeModel;
		spatialModel.absoluteModel = spatial.absoluteModel;
	}

	void SpatialSystem::StoreSpatials() {
		EngineState->Store<Models::Spatial, Spatial>();
	}

	void MakeDirty(const entt::entity spatialEntity) {

	}

	bool SpatialSystem::IsParent(entt::entity left, entt::entity right) {
		if (right == entt::null) return false;
		const auto& relationship = EngineState->Registry.get<Objects::Relationship>(right);
		return relationship.parent == left || IsParent(left, relationship.parent);
	}

	uint32_t SpatialSystem::GetDepth(entt::entity entity) {
		const auto& relationship = EngineState->Registry.get<Objects::Relationship>(entity);
		if (relationship.parent == entt::null) return 0;
		return GetDepth(relationship.parent) + 1;
	}

	std::string SpatialSystem::GetPath(entt::entity entity) {
		const auto& relationship = EngineState->Registry.get<Objects::Relationship>(entity);
		const auto& object = EngineState->Registry.get<Objects::Object>(entity);
		if (relationship.parent == entt::null) return object.Name;
		return fmt::format("{0}-->{1}", GetPath(relationship.parent), object.Name);
	}

	void SpatialSystem::PropagateChanges() {
		EngineState->Registry.sort<Objects::Dirty<Spatial>>(
			[&](entt::entity left, entt::entity& right) {
				const auto& rightRelation = EngineState->Registry.get<Objects::Relationship>(right);
				return GetDepth(left) < GetDepth(right);
			}
		);
		EngineState->Registry.view<Objects::Dirty<Spatial>>().each(
			[&](const entt::entity entity) {
				const auto& relationship = EngineState->Registry.get<Objects::Relationship>(entity);
				const auto& object = EngineState->Registry.get<Objects::Object>(entity);

				auto& spatial = EngineState->Registry.get<Spatial>(entity);

				auto translationMatrix = glm::translate(
					glm::identity<glm::mat4>(),
					glm::vec3(spatial.relativePosition));
				auto rotationMatrix = glm::toMat4(spatial.relativeRotation);
				auto scalingMatrix = glm::scale(glm::identity<glm::mat4>(), glm::vec3(spatial.relativeScale));
				spatial.relativeModel = translationMatrix * rotationMatrix * scalingMatrix;

				if (relationship.parent != entt::null) {
					auto& parentSpatial = EngineState->Registry.get<Spatial>(relationship.parent);
					spatial.absoluteScale = parentSpatial.absoluteScale * spatial.relativeScale;
					spatial.absoluteRotation = parentSpatial.absoluteRotation * spatial.relativeRotation;
					spatial.absolutePosition = parentSpatial.absolutePosition + parentSpatial.absoluteRotation *
																				(parentSpatial.absoluteScale *
																				 spatial.relativePosition);
					spatial.absoluteModel = parentSpatial.absoluteModel * spatial.relativeModel;
				} else {
					spatial.absoluteScale = spatial.relativeScale;
					spatial.absoluteRotation = spatial.relativeRotation;
					spatial.absolutePosition = spatial.relativePosition;
					spatial.absoluteModel = spatial.relativeModel;
				}
				EngineState->Registry.emplace_or_replace<Graphics::SynchronizationState<Models::Spatial>>(
					entity,
					(uint32_t) EngineState->FrameStates.size());
			}
		);
	}

	void SpatialSystem::UpdateSpatials() {
		Graphics::SynchronizationState<Models::Spatial>::Update<Spatial>(
			EngineState->Registry,
			EngineState->FrameIndex,
			std::function<void(Models::Spatial&, const Spatial&)>(
				[=](
					Models::Spatial& model,
					const Spatial component
				) { UpdateStoreItem(component, model); }
			)
		);
	}
}
