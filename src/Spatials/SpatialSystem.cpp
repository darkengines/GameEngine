#include "SpatialSystem.hpp"
#include "../Objects/Relationship.hpp"
#include "../Objects/Dirty.hpp"
#include "../Objects/Object.hpp"
#include <algorithm>
#include "glm/gtx/quaternion.hpp"
#include "entt/entt.hpp"
#include <string>
#include "fmt/format.h"

namespace drk::Spatials {

	SpatialSystem::SpatialSystem(
		const Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		entt::registry& registry
	)
		: System(engineState, registry), deviceContext(deviceContext) {}

	void SpatialSystem::AddSpatialSystem(entt::registry& registry) {
		registry.on_construct<Components::Spatial>().connect<SpatialSystem::OnSpatialConstruct>();
	}

	void SpatialSystem::RemoveSpatialSystem(entt::registry& registry) {
		registry.on_construct<Components::Spatial>().disconnect<SpatialSystem::OnSpatialConstruct>();
	}

	void SpatialSystem::OnSpatialConstruct(entt::registry& registry, entt::entity spatialEntity) {
		registry.emplace<Objects::Dirty<Components::Spatial>>(spatialEntity);
	}

	void SpatialSystem::Update(Models::Spatial& spatialModel, const Components::Spatial& spatial) {
		spatialModel.relativeScale = spatial.relativeScale;
		spatialModel.relativeRotation = spatial.relativeRotation;
		spatialModel.relativePosition = spatial.relativePosition;
		spatialModel.absoluteScale = spatial.absoluteScale;
		spatialModel.absoluteRotation = spatial.absoluteRotation;
		spatialModel.absolutePosition = spatial.absolutePosition;
		spatialModel.relativeModel = spatial.relativeModel;
		spatialModel.absoluteModel = spatial.absoluteModel;
	}

	void SpatialSystem::MakeDirty(const entt::entity spatialEntity, bool asChild) {
		auto& relationship = registry.get<Objects::Relationship>(spatialEntity);
		auto& object = registry.get<Objects::Object>(spatialEntity);
		registry.emplace_or_replace<Objects::Dirty<Spatials::Components::Spatial>>(spatialEntity, true);
		if (relationship.childCount > 0) {
			MakeDirty(relationship.firstChild, true);
		}
		if (asChild && relationship.nextSibling != entt::null) {
			MakeDirty(relationship.nextSibling, true);
		}
	}

	bool SpatialSystem::IsParent(entt::entity left, entt::entity right) {
		if (right == entt::null) return false;
		const auto& relationship = registry.get<Objects::Relationship>(right);
		return relationship.parent == left || IsParent(left, relationship.parent);
	}

	uint32_t SpatialSystem::GetDepth(entt::entity entity) {
		const auto& relationship = registry.get<Objects::Relationship>(entity);
		if (relationship.parent == entt::null) return 0;
		return GetDepth(relationship.parent) + 1;
	}

	std::string SpatialSystem::GetPath(entt::entity entity) {
		const auto& relationship = registry.get<Objects::Relationship>(entity);
		const auto& object = registry.get<Objects::Object>(entity);
		if (relationship.parent == entt::null) return object.Name;
		return fmt::format("{0}-->{1}", GetPath(relationship.parent), object.Name);
	}

	void SpatialSystem::PropagateChanges() {
		registry.sort<Objects::Dirty<Components::Spatial>>(
			[&](entt::entity left, entt::entity& right) {
				const auto& rightRelation = registry.get<Objects::Relationship>(right);
				return GetDepth(left) < GetDepth(right);
			}
		);
		registry.view<Objects::Dirty<Components::Spatial>>().each(
			[&](const entt::entity entity) {
				const auto& relationship = registry.get<Objects::Relationship>(entity);
				const auto& object = registry.get<Objects::Object>(entity);
				//std::cout << fmt::format("{0} {1}", GetDepth(entity), object.Name) << std::endl;

				auto& spatial = registry.get<Components::Spatial>(entity);

				auto translationMatrix = glm::translate(
					glm::identity<glm::mat4>(),
					glm::vec3(spatial.relativePosition));
				auto rotationMatrix = glm::toMat4(spatial.relativeRotation);
				auto scalingMatrix = glm::scale(glm::identity<glm::mat4>(), glm::vec3(spatial.relativeScale));
				spatial.relativeModel = translationMatrix * rotationMatrix * scalingMatrix;

				if (relationship.parent != entt::null) {
					auto& parentSpatial = registry.get<Components::Spatial>(relationship.parent);
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
				registry.emplace_or_replace<Graphics::SynchronizationState<Models::Spatial>>(
					entity,
					(uint32_t) engineState.getFrameCount());
			}
		);
	}
}
