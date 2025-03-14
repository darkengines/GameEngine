#include "./BoneSpatialSystem.hpp"
#include "../../Common/Components/Dirty.hpp"
#include "../Components/RootBoneInstanceReference.hpp"
#include "../../Nodes/Components/Node.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <entt/entt.hpp>

namespace drk::Animations::Systems {
	BoneSpatialSystem::BoneSpatialSystem(
		Engine::EngineState& engineState,
		entt::registry& registry
	) : drk::Systems::StorageSystem<Models::BoneSpatial, Spatials::Components::Spatial<Components::Bone>>(
		engineState,
		registry
	) {}
	void BoneSpatialSystem::update(
		Models::BoneSpatial& boneSpatialModel,
		const Spatials::Components::Spatial<Components::Bone>& boneSpatial
	) {
		boneSpatialModel.position = boneSpatial.position;
		boneSpatialModel.rotation = boneSpatial.rotation;
		boneSpatialModel.scale = boneSpatial.scale;
		boneSpatialModel.model = boneSpatial.model;
	}

	void BoneSpatialSystem::propagateChanges() {
		auto view = registry.view<
			Animations::Components::RootBoneInstanceReference,
			Nodes::Components::Node,
			Spatials::Components::Spatial<Spatials::Components::Relative>,
			Spatials::Components::Spatial<Spatials::Components::Absolute>,
			Common::Components::Dirty<Spatials::Components::Spatial<Spatials::Components::Relative>>
		>();
		view.use<Common::Components::Dirty<Spatials::Components::Spatial<Spatials::Components::Relative>>>();
		view.each(
			[&](
				entt::entity entity,
				const Animations::Components::RootBoneInstanceReference& rootBoneInstanceReference,
				const Nodes::Components::Node& relationship,
				Spatials::Components::Spatial<Spatials::Components::Relative>& relativeSpatial,
				Spatials::Components::Spatial<Spatials::Components::Absolute>& absoluteSpatial
			) {
				auto& boneSpatial = registry.get_or_emplace<Spatials::Components::Spatial<Components::Bone>>(entity);

				auto translationMatrix = glm::translate(
					glm::identity<glm::mat4>(),
					glm::vec3(relativeSpatial.position));
				auto rotationMatrix = glm::toMat4(relativeSpatial.rotation);
				auto scalingMatrix = glm::scale(glm::identity<glm::mat4>(), glm::vec3(relativeSpatial.scale));
				relativeSpatial.model = translationMatrix * rotationMatrix * scalingMatrix;

				if (rootBoneInstanceReference.rootBoneInstanceEntity != entt::null &&
					relationship.parent != entt::null) {
					auto& parentSpatial = registry.get<Spatials::Components::Spatial<Components::Bone>>(relationship.parent);
					boneSpatial.scale = parentSpatial.scale * relativeSpatial.scale;
					boneSpatial.rotation = parentSpatial.rotation * relativeSpatial.rotation;
					boneSpatial.position = parentSpatial.position +
										   parentSpatial.rotation * (parentSpatial.scale * relativeSpatial.position);
					boneSpatial.model = parentSpatial.model * relativeSpatial.model;
				} else {

					boneSpatial.scale = relativeSpatial.scale;
					boneSpatial.rotation = relativeSpatial.rotation;
					boneSpatial.position = relativeSpatial.position;
					boneSpatial.model = relativeSpatial.model;
				}
				registry.emplace_or_replace<Common::Components::Dirty<Spatials::Components::Spatial<Components::Bone>>>(
					entity
				);
				registry.emplace_or_replace<Graphics::SynchronizationState<Models::BoneSpatial>>(
					entity,
					(uint32_t) engineState.getFrameCount());
			}
		);
	}
}
