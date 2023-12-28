#include "ObjectSystem.hpp"
#include "../../Stores/StoreItem.hpp"
#include "../../Graphics/SynchronizationState.hpp"
#include "../../Meshes/Components/MeshGroup.hpp"
#include "../../Meshes/Systems/MeshSystem.hpp"
#include "../Components/Relationship.hpp"

namespace drk::Objects {

	Systems::ObjectSystem::ObjectSystem(
		const Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		entt::registry& registry
	)
		: System(engineState, registry), DeviceContext(deviceContext) {}

	void Systems::ObjectSystem::Update(
		Models::Object& objectModel, const Stores::StoreItem<Spatials::Models::Spatial>& spatialStoreItem
	) {
		const auto& spatialItemLocation = spatialStoreItem.frameStoreItems[engineState.getFrameIndex()];
		objectModel.spatialItemLocation.storeIndex = spatialItemLocation.pStore->descriptorArrayElement;
		objectModel.spatialItemLocation.itemIndex = spatialItemLocation.index;
	}

	void Systems::ObjectSystem::AddObjectSystem(entt::registry& registry) {
		registry.on_construct<Components::Object>().connect<ObjectSystem::OnObjectConstruct>();
	}

	void Systems::ObjectSystem::RemoveObjectSystem(entt::registry& registry) {
		registry.on_construct<Components::Object>().disconnect<ObjectSystem::OnObjectConstruct>();
	}

	void Systems::ObjectSystem::OnObjectConstruct(entt::registry& registry, entt::entity objectEntity) {

	}
	entt::entity
	Systems::ObjectSystem::copyObjectEntity(
		const entt::registry& source,
		entt::registry& destination,
		entt::entity sourceEntity,
		entt::entity parent,
		entt::entity previousSibling
	) {
		const auto& sourceObject = source.get<Components::Object>(sourceEntity);
		auto sourceMeshGroup = source.try_get<Meshes::Components::MeshGroup>(sourceEntity);
		auto sourceSpatial = source.try_get<Spatials::Components::Spatial>(sourceEntity);
		auto sourceRelationship = source.try_get<Components::Relationship>(sourceEntity);

		auto destinationEntity = destination.create();
		Components::Object destinationObject{
			.Name = sourceObject.Name
		};
		destination.emplace<Components::Object>(destinationEntity, destinationObject);
		if (sourceMeshGroup != nullptr) {
			auto destinationMeshGroup = Meshes::Systems::MeshSystem::copyMeshGroup(source, destination, *sourceMeshGroup);
			destination.emplace<Meshes::Components::MeshGroup>(destinationEntity, destinationMeshGroup);
		}
		if (sourceSpatial != nullptr) {
			destination.emplace<Spatials::Components::Spatial>(destinationEntity, *sourceSpatial);
		}

		if (sourceRelationship != nullptr) {

			Components::Relationship destinationRelationship{
				.parent = parent,
				.depth = sourceRelationship->depth
			};

			for (const auto& sourceChild: sourceRelationship->children) {
				const auto& destinationChild = copyObjectEntity(source, destination, sourceChild, destinationEntity);
				destinationRelationship.children.emplace_back(std::move(destinationChild));
			}
			destination.emplace<Components::Relationship>(destinationEntity, std::move(destinationRelationship));
		}

		return destinationEntity;
	}
}
