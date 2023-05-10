#include "ObjectSystem.hpp"
#include "../Stores/StoreItem.hpp"
#include "../Graphics/SynchronizationState.hpp"
#include "../Meshes/MeshGroup.hpp"

namespace drk::Objects {

	ObjectSystem::ObjectSystem(
		const Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		entt::registry& registry
	)
		: System(engineState, registry), DeviceContext(deviceContext) {}

	void ObjectSystem::Update(
		Models::Object& objectModel, const Stores::StoreItem<Spatials::Models::Spatial>& spatialStoreItem
	) {
		const auto& spatialItemLocation = spatialStoreItem.frameStoreItems[engineState.getFrameIndex()];
		objectModel.spatialItemLocation.storeIndex = spatialItemLocation.pStore->descriptorArrayElement;
		objectModel.spatialItemLocation.itemIndex = spatialItemLocation.index;
	}

	void ObjectSystem::AddObjectSystem(entt::registry& registry) {
		registry.on_construct<Object>().connect<ObjectSystem::OnObjectConstruct>();
	}

	void ObjectSystem::RemoveObjectSystem(entt::registry& registry) {
		registry.on_construct<Object>().disconnect<ObjectSystem::OnObjectConstruct>();
	}

	void ObjectSystem::OnObjectConstruct(entt::registry& registry, entt::entity objectEntity) {

	}
}
