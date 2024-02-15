#pragma once

#include "../../Stores/Models/StoreItemLocation.hpp"

namespace drk::Meshes::Models {
	struct Mesh {
		Stores::Models::StoreItemLocation materialItemLocation;
		bool hasTangent;
		bool hasBitTangent;
	};
}