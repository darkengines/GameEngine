#include "./BoneSpatialSystem.hpp"
#include "../../Objects/Components/Dirty.hpp"
#include "../../Objects/Components/ObjectReference.hpp"
#include "../Components/RootBoneInstanceReference.hpp"
#include "../../Spatials/Components/Spatial.hpp"
#include "../../Objects/Components/Relationship.hpp"
#include "../Components/RootBoneInstanceReference.hpp"
#include "glm/gtx/quaternion.hpp"
#include <algorithm>
#include <entt/entt.hpp>

namespace drk::Animations::Systems {
	BoneSpatialSystem::BoneSpatialSystem(
		Engine::EngineState& engineState,
		entt::registry& registry
	) : drk::Systems::System<Models::BoneSpatial, Spatials::Components::Spatial<Components::Bone>>(engineState, registry) {}
	void BoneSpatialSystem::update(Models::BoneSpatial& boneSpatialModel, const Spatials::Components::Spatial<Components::Bone>& boneSpatial) {
		boneSpatialModel.position = boneSpatial.position;
		boneSpatialModel.rotation = boneSpatial.rotation;
		boneSpatialModel.scale = boneSpatial.scale;
		boneSpatialModel.model = boneSpatial.model;
	}

	void BoneSpatialSystem::propagateChanges() {
		auto view = registry.view<
			Animations::Components::RootBoneInstanceReference,
			Objects::Components::Relationship,
			Spatials::Components::Spatial<Spatials::Components::Relative>,
			Spatials::Components::Spatial<Spatials::Components::Absolute>,
			Objects::Components::Dirty<Spatials::Components::Spatial<Spatials::Components::Relative>>
		>();
		view.use<Objects::Components::Dirty<Spatials::Components::Spatial<Spatials::Components::Relative>>>();
		view.each(
			[&](
				entt::entity entity,
				const Animations::Components::RootBoneInstanceReference& rootBoneInstanceReference,
				const Objects::Components::Relationship& relationship,
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

					if (rootBoneInstanceReference.rootBoneInstanceEntity != entt::null && relationship.parent != entt::null) {
						auto& parentSpatial = registry.get<Spatials::Components::Spatial<Components::Bone>>(relationship.parent);
						boneSpatial.scale = parentSpatial.scale * relativeSpatial.scale;
						boneSpatial.rotation = parentSpatial.rotation * relativeSpatial.rotation;
						boneSpatial.position = parentSpatial.position + parentSpatial.rotation * (parentSpatial.scale * relativeSpatial.position);
						boneSpatial.model = parentSpatial.model * relativeSpatial.model;
					}
					else {

						boneSpatial.scale = relativeSpatial.scale;
						boneSpatial.rotation = relativeSpatial.rotation;
						boneSpatial.position = relativeSpatial.position;
						boneSpatial.model = relativeSpatial.model;
					}
					registry.emplace_or_replace<Objects::Components::Dirty<Spatials::Components::Spatial<Components::Bone>>>(entity);
					registry.emplace_or_replace<Graphics::SynchronizationState<Models::BoneSpatial>>(
						entity,
						(uint32_t)engineState.getFrameCount());
			}
		);
	}
}