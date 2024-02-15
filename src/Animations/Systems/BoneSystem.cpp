#include "./BoneSystem.hpp"

namespace drk::Animations::Systems {
	BoneSystem::BoneSystem(
		Engine::EngineState& engineState,
		entt::registry& registry
	) : drk::Systems::System<Models::Bone, Components::Bone>(engineState, registry) {}
	void BoneSystem::update(Models::Bone& boneModel, const Components::Bone& boneComponent) {
		boneModel.spatialOffset = boneComponent.spatialOffset;
	}
}