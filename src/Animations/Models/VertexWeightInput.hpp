#include "../../Stores/StoreItem.hpp"
#include "../../Objects/Models/Object.hpp"
#include "../Models/Bone.hpp"
#include "../Models/VertexWeight.hpp"

namespace drk::Animations::Models {
	struct VertexWeightInput {
		Stores::StoreItem<Objects::Models::Object> instanceStoreItem;
		Stores::StoreItem<Models::Bone> boneStoreItem;
		Stores::StoreItem<Models::VertexWeight> vertexWeightStoreItem;
	};
}