#include "ObjectSystem.hpp"
#include "../Stores/StoreItem.hpp"
#include "../Graphics/SynchronizationState.hpp"
#include "../Meshes/MeshGroup.hpp"

namespace drk::Objects {

	ObjectSystem::ObjectSystem(
		drk::Devices::DeviceContext *pContext,
		drk::Graphics::EngineState *pState
	)
		: DeviceContext(pContext), EngineState(pState) {}

	void ObjectSystem::UpdateStoreItem(
		Models::Object &objectModel, const Stores::StoreItem<Spatials::Models::Spatial> &spatialStoreItem,
		const Stores::StoreItem<Meshes::Models::Mesh> &meshStoreItem
	) {
		const auto& spatialItemLocation = spatialStoreItem.frameStoreItems[EngineState->FrameIndex];
		objectModel.spatialItemLocation = {
			.storeIndex = spatialItemLocation.pStore->descriptorArrayElement,
			.itemIndex = spatialItemLocation.index
		};
	}

	void ObjectSystem::StoreObjects() {
		EngineState->Store<Models::Object, Stores::StoreItem<Spatials::Models::Spatial>, Meshes::MeshGroup>();
	}

	void ObjectSystem::UpdateObjects() {
		Graphics::SynchronizationState<Models::Object>::Update<Stores::StoreItem<Spatials::Models::Spatial>, Stores::StoreItem<Meshes::Models::Mesh>>(
			EngineState->Registry,
			EngineState->FrameIndex,
			[=](
				Models::Object &model,
				const Stores::StoreItem<Spatials::Models::Spatial> &spatialStoreItem,
				const Stores::StoreItem<Meshes::Models::Mesh> &meshStoreItem
			) { UpdateStoreItem(model, spatialStoreItem, meshStoreItem); }
		);
	}

	void ObjectSystem::AddObjectSystem(entt::registry &registry) {
		registry.on_construct<Object>().connect<ObjectSystem::OnObjectConstruct>();
	}

	void ObjectSystem::RemoveObjectSystem(entt::registry &registry) {
		registry.on_construct<Object>().disconnect<ObjectSystem::OnObjectConstruct>();
	}

	void ObjectSystem::OnObjectConstruct(entt::registry &registry, entt::entity objectEntity) {
		registry.emplace<Graphics::SynchronizationState<Models::Object>>(objectEntity, 2u);
	}
}
