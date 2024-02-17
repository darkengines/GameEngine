#pragma once

#include "../Stores/Models/StoreItemLocation.hpp"
#include "../Meshes/Components/MeshBufferView.hpp"
#include "../Meshes/Components/MeshResource.hpp"
#include "../Spatials/Components/Spatial.hpp"

namespace drk::Graphics {
	struct Draw {
		Meshes::Components::MeshBufferView mesh;
		Stores::Models::StoreItemLocation meshStoreItem;
		Stores::Models::StoreItemLocation objectLocation;
		Spatials::Components::Spatial<Spatials::Components::Absolute> spatial;
		bool hasTransparency;
	};
}