#include "../../Stores/StoreItem.hpp"
#include "../../Objects/Models/Object.hpp"
#include "../Models/VertexWeight.hpp"
#include "../Models/Bone.hpp"

namespace drk::Animations::Models {
	struct SkinnedVertex {
		uint32_t vertexWeightBufferIndex;
		uint32_t vertexWeightIndex;
		Stores::Models::StoreItemLocation boneInstanceStoreItemLocation;
		Stores::Models::StoreItemLocation boneStoreItemLocation;
	};
}