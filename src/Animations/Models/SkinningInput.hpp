#pragma once

#include "../../Stores/Models/StoreItemLocation.hpp"

namespace drk::Animations::Models {
	struct SkinningInput {
		Stores::Models::StoreItemLocation objectItemLocation;
		Stores::Models::StoreItemLocation vertexItemLocation;
		Stores::Models::StoreItemLocation skinnedVertexItemLocation;
		Stores::Models::StoreItemLocation vertexWeightItemLocation;
		Stores::Models::StoreItemLocation boneItemLocation;
	};
}