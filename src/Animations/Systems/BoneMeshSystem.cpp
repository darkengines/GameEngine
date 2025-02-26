#include "./BoneMeshSystem.hpp"

namespace drk::Animations::Systems {
	BoneMeshSystem::BoneMeshSystem(
		Engine::EngineState& engineState,
		entt::registry& registry
	) : drk::Systems::StorageSystem<Models::BoneMesh, Components::BoneMesh>(engineState, registry) {}
	void BoneMeshSystem::update(Models::BoneMesh& boneMeshModel, const Components::BoneMesh& boneMeshComponent) {
		boneMeshModel.spatialOffset = boneMeshComponent.offset;
	}
}