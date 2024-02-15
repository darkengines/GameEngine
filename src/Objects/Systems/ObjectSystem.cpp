#include "ObjectSystem.hpp"
#include "../../Meshes/Systems/MeshSystem.hpp"
#include "../../Common/Components/Name.hpp"

namespace drk::Nodes {

	Systems::ObjectSystem::ObjectSystem(
		const Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		entt::registry& registry
	)
		: System(engineState, registry), DeviceContext(deviceContext) {}

	void Systems::ObjectSystem::update(
		Models::Object& objectModel,
		const Stores::StoreItem<Spatials::Models::Spatial>& spatialStoreItem,
		const Stores::StoreItem<Spatials::Models::RelativeSpatial>& relativeSpatialStoreItem
	) {
		const auto& spatialItemLocation = spatialStoreItem.frameStoreItems[engineState.getFrameIndex()];
		objectModel.spatialItemLocation.storeIndex = spatialItemLocation.pStore->descriptorArrayElement;
		objectModel.spatialItemLocation.itemIndex = spatialItemLocation.index;
		const auto& relativeSpatialItemLocation = relativeSpatialStoreItem.frameStoreItems[engineState.getFrameIndex()];
		objectModel.relativeSpatialItemLocation.storeIndex = relativeSpatialItemLocation.pStore->descriptorArrayElement;
		objectModel.relativeSpatialItemLocation.itemIndex = relativeSpatialItemLocation.index;
	}
	entt::entity
	Systems::ObjectSystem::copyObjectEntity(
		const entt::registry& source,
		entt::registry& destination,
		entt::entity sourceEntity,
		entt::entity parent,
		entt::entity previousSibling
	) {
		const auto& sourceNameComponent = source.get<Common::Components::Name>(sourceEntity);
		auto objectMeshes = source.view<Components::ObjectReference, Meshes::Components::MeshReference>();
		auto sourceSpatial = source.try_get<Spatials::Components::Spatial<Spatials::Components::Relative>>(sourceEntity);
		auto sourceRelationship = source.try_get<Components::Node>(sourceEntity);

		auto destinationEntity = destination.create();
		Common::Components::Name destinationObject{
			.name = sourceNameComponent.name
		};
		destination.emplace<Common::Components::Name>(destinationEntity, destinationObject);


		objectMeshes.each(
			[&](
				entt::entity sourceObjectMeshEntity,
				const Nodes::Components::ObjectReference& objectReference,
				const Meshes::Components::MeshReference& meshReference
			) {
				if (objectReference.objectEntity == sourceEntity) {
					auto destinationObjectMeshEntity = destination.create();
					auto destinationMeshEntity = Meshes::Systems::MeshSystem::copyMeshEntity(
						source,
						destination,
						meshReference.meshEntity
					);

					destination.emplace<Nodes::Components::ObjectReference>(
						destinationObjectMeshEntity,
						destinationObjectMeshEntity
					);
					destination.emplace<Meshes::Components::MeshReference>(
						destinationObjectMeshEntity,
						destinationMeshEntity
					);
				}
			}
		);
		if (sourceSpatial != nullptr) {
			destination.emplace<Spatials::Components::Spatial<Spatials::Components::Relative>>(
				destinationEntity,
				*sourceSpatial
			);
		}

		if (sourceRelationship != nullptr) {

			Components::Node destinationRelationship{
				.parent = parent,
				.depth = sourceRelationship->depth
			};

			for (const auto& sourceChild: sourceRelationship->children) {
				const auto& destinationChild = copyObjectEntity(source, destination, sourceChild, destinationEntity);
				destinationRelationship.children.emplace_back(std::move(destinationChild));
			}
			destination.emplace<Components::Node>(destinationEntity, std::move(destinationRelationship));
		}

		return destinationEntity;
	}
}
