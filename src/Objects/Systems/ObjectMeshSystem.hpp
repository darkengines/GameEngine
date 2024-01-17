#include "../../Systems/System.hpp"
#include "../Components/ObjectReference.hpp"
#include "../../Meshes/Components/MeshReference.hpp"
#include "../Models/ObjectMesh.hpp"
#include "../../Meshes/Models/Mesh.hpp"
#include "../../Stores/StoreItem.hpp"
#include "../Models/Object.hpp"
#include "../../Meshes/Models/Mesh.hpp"
#include "../../BoundingVolumes/Models/AxisAlignedBoundingBox.hpp"

namespace drk::Objects::Systems {
	class ObjectMeshSystem : public drk::Systems::System<
		Models::ObjectMesh,
		Stores::StoreItem<Objects::Models::Object>,
		Stores::StoreItem<Meshes::Models::Mesh>,
		Stores::StoreItem<BoundingVolumes::Models::AxisAlignedBoundingBox>
	> {
	public:
		void update(
			Models::ObjectMesh& objectMeshModel,
			const Stores::StoreItem<Objects::Models::Object>& objectStoreItem,
			const Stores::StoreItem<Meshes::Models::Mesh>& meshStoreItem,
			const Stores::StoreItem<BoundingVolumes::Models::AxisAlignedBoundingBox>& axisAlignedBoundingBoxStoreItem
		);
		void ProcessDirty();
	};
}