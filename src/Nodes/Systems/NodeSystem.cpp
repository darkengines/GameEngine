#include "NodeSystem.hpp"
#include "../../Meshes/Systems/MeshSystem.hpp"
#include "../../Common/Components/Name.hpp"

namespace drk::Nodes {

	Systems::NodeSystem::NodeSystem(
		const Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		entt::registry& registry
	)
		: StorageSystem(engineState, registry), deviceContext(deviceContext) {}

	void Systems::NodeSystem::update(
		Models::Node& nodeModel,
		const Stores::StoreItem<Spatials::Models::Spatial>& spatialStoreItem,
		const Stores::StoreItem<Spatials::Models::RelativeSpatial>& relativeSpatialStoreItem
	) {
		const auto& spatialItemLocation = spatialStoreItem.frameStoreItems[engineState.getFrameIndex()];
		nodeModel.spatialItemLocation.storeIndex = spatialItemLocation.pStore->descriptorArrayElement;
		nodeModel.spatialItemLocation.itemIndex = spatialItemLocation.index;
		const auto& relativeSpatialItemLocation = relativeSpatialStoreItem.frameStoreItems[engineState.getFrameIndex()];
		nodeModel.relativeSpatialItemLocation.storeIndex = relativeSpatialItemLocation.pStore->descriptorArrayElement;
		nodeModel.relativeSpatialItemLocation.itemIndex = relativeSpatialItemLocation.index;
	}
	entt::entity
	Systems::NodeSystem::copyNodeEntity(
		const entt::registry& source,
		entt::registry& destination,
		entt::entity sourceEntity,
		entt::entity parent,
		entt::entity previousSibling
	) {
		const auto& sourceNameComponent = source.get<Common::Components::Name>(sourceEntity);
		auto nodeMeshes = source.view<Components::NodeMesh>();
		auto sourceSpatial = source.try_get<Spatials::Components::Spatial<Spatials::Components::Relative>>(sourceEntity);
		auto sourceRelationship = source.try_get<Components::Node>(sourceEntity);

		auto destinationEntity = destination.create();
		Common::Components::Name destinationNode{
			.name = sourceNameComponent.name
		};
		destination.emplace<Common::Components::Name>(destinationEntity, destinationNode);


		nodeMeshes.each(
			[&](
				entt::entity sourceNodeMeshEntity,
				const Components::NodeMesh& nodeMesh
			) {
				if (nodeMesh.nodeEntity == sourceEntity) {
					auto destinationNodeMeshEntity = destination.create();
					auto destinationMeshEntity = Meshes::Systems::MeshSystem::copyMeshEntity(
						source,
						destination,
						nodeMesh.meshEntity
					);

					destination.emplace<Nodes::Components::NodeMesh>(
						destinationNodeMeshEntity,
						destinationEntity,
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
				const auto& destinationChild = copyNodeEntity(source, destination, sourceChild, destinationEntity);
				destinationRelationship.children.emplace_back(std::move(destinationChild));
			}
			destination.emplace<Components::Node>(destinationEntity, std::move(destinationRelationship));
		}

		return destinationEntity;
	}
}
