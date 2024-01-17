#include "ObjectMeshSystem.hpp"

namespace drk::Objects::Systems {
	void ObjectMeshSystem::update(
		Models::ObjectMesh& objectMeshModel,
		const Stores::StoreItem<Objects::Models::Object>& objectStoreItem,
		const Stores::StoreItem<Meshes::Models::Mesh>& meshStoreItem,
		const Stores::StoreItem<BoundingVolumes::Models::AxisAlignedBoundingBox>& axisAlignedBoundingBoxStoreItem
	) {
		objectMeshModel.objectStoreItemLocation = objectStoreItem.frameStoreItems[engineState.getFrameIndex()];
		objectMeshModel.meshStoreItemLocation = meshStoreItem.frameStoreItems[engineState.getFrameIndex()];
		objectMeshModel.axisAlignedBoundingBoxStoreItemLocation = axisAlignedBoundingBoxStoreItem.frameStoreItems[engineState.getFrameIndex()];
	}
	void ObjectMeshSystem::ProcessDirty() {
		
	}
}