#pragma once
#include "../../Stores/Models/StoreItemLocation.hpp"

namespace drk::Animations::Models {
	struct BoneInstanceWeight {
		Stores::Models::StoreItemLocation boneInstanceStoreItemLocation;
		Stores::Models::StoreItemLocation boneInstanceSpatialStoreItemLocation;
		Stores::Models::StoreItemLocation boneStoreItemLocation;
		float weight;
	};
}