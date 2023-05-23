#include "ObjectSystem.hpp"
#include "../Stores/StoreItem.hpp"
#include "../Graphics/SynchronizationState.hpp"
#include "../Meshes/MeshGroup.hpp"
#include "../Meshes/MeshSystem.hpp"
#include "Relationship.hpp"

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
	entt::entity
	ObjectSystem::copyObjectEntity(
		const entt::registry& source,
		entt::registry& destination,
		entt::entity sourceEntity,
		entt::entity parent,
		entt::entity previousSibling
	) {
		const auto& sourceObject = source.get<Object>(sourceEntity);
		auto sourceMeshGroup = source.try_get<Meshes::MeshGroup>(sourceEntity);
		auto sourceSpatial = source.try_get<Spatials::Components::Spatial>(sourceEntity);
		auto sourceRelationship = source.try_get<Relationship>(sourceEntity);

		auto destinationEntity = destination.create();
		Object destinationObject{
			.Name = sourceObject.Name
		};
		destination.emplace<Object>(destinationEntity, destinationObject);
		if (sourceMeshGroup != nullptr) {
			auto destinationMeshGroup = Meshes::MeshSystem::copyMeshGroup(source, destination, *sourceMeshGroup);
			destination.emplace<Meshes::MeshGroup>(destinationEntity, destinationMeshGroup);
		}
		if (sourceSpatial != nullptr) {
			destination.emplace<Spatials::Components::Spatial>(destinationEntity, *sourceSpatial);
		}

		if (sourceRelationship != nullptr) {

			Relationship destinationRelationship{
				.parent = parent,
				.depth = sourceRelationship->depth
			};

			for (const auto& sourceChild: sourceRelationship->children) {
				const auto& destinationChild = copyObjectEntity(source, destination, sourceChild, destinationEntity);
				destinationRelationship.children.emplace_back(std::move(destinationChild));
			}
			destination.emplace<Relationship>(destinationEntity, std::move(destinationRelationship));
		}

		return destinationEntity;
	}
}
