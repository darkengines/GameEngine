#include "SpatialSystem.hpp"
#include "../../Objects/Components/Relationship.hpp"
#include "../../Objects/Components/Dirty.hpp"
#include "../../Objects/Components/Object.hpp"
#include <algorithm>
#include "glm/gtx/quaternion.hpp"
#include "entt/entt.hpp"
#include <string>
#include "fmt/format.h"

namespace drk::Spatials::Systems {

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
		registry.emplace<Objects::Components::Dirty<Components::Spatial>>(spatialEntity);
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

	void SpatialSystem::MakeDirty(const entt::entity spatialEntity) {
		auto& relationship = registry.get<Objects::Components::Relationship>(spatialEntity);
		auto& object = registry.get<Objects::Components::Object>(spatialEntity);
		registry.emplace_or_replace<Objects::Components::Dirty<Spatials::Components::Spatial>>(spatialEntity, true);
		for (const auto& child: relationship.children) {
			MakeDirty(child);
		}
	}

	bool SpatialSystem::IsParent(entt::entity left, entt::entity right) {
		if (right == entt::null) return false;
		const auto& relationship = registry.get<Objects::Components::Relationship>(right);
		return relationship.parent == left || IsParent(left, relationship.parent);
	}

	uint32_t SpatialSystem::GetDepth(const entt::registry& registry, entt::entity entity) {
		const auto& relationship = registry.get<Objects::Components::Relationship>(entity);
		if (relationship.parent == entt::null) return 0;
		return GetDepth(registry, relationship.parent) + 1;
	}

	std::string SpatialSystem::GetPath(entt::entity entity) {
		const auto& relationship = registry.get<Objects::Components::Relationship>(entity);
		const auto& object = registry.get<Objects::Components::Object>(entity);
		if (relationship.parent == entt::null) return object.Name;
		return fmt::format("{0}-->{1}", GetPath(relationship.parent), object.Name);
	}

//	bool SpatialSystem::compareRelationship(
//		const entt::registry& registry,
//		const entt::entity leftEntity,
//		const entt::entity rightEntity
//	) {
//		Objects::Relationship leftRelationship = registry.get<Objects::Relationship>(leftEntity);
//		Objects::Relationship rightRelationship = registry.get<Objects::Relationship>(rightEntity);
//
//		return rightRelationship.parent == leftEntity
//			   || leftRelationship.nextSibling == rightEntity
//			   || (
//				   !(leftRelationship.parent == rightEntity || rightRelationship.nextSibling == leftEntity)
//				   && (
//					   leftRelationship.parent < rightRelationship.parent
//					   || (leftRelationship.parent == rightRelationship.parent && &leftRelationship < &rightRelationship)
//				   )
//			   );
//	}

	void SpatialSystem::PropagateChanges() {
		registry.sort<Objects::Components::Dirty<Components::Spatial>>(
			[&](entt::entity left, entt::entity& right) {
				return GetDepth(registry, left) < GetDepth(registry, right);
			}
		);
		registry.view<Objects::Components::Dirty<Components::Spatial>>().each(
			[&](entt::entity entity, Objects::Components::Dirty<Components::Spatial>& dirtySpatial) {
				const auto& relationship = registry.get<Objects::Components::Relationship>(entity);
				const auto& object = registry.get<Objects::Components::Object>(entity);
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
