#include "../../Stores/StoreItem.hpp"
#include "../../Meshes/Vertex.hpp"
#include "../Models/SkinnedVertex.hpp"

namespace drk::Animations::Components {
	struct SkinnedVertexRange {
		Stores::StoreItem<Meshes::Vertex> vertexStoreItem;
		Stores::StoreItem<Models::SkinnedVertex> skinnedVertexStoreItem;
		uint32_t skinnedVertexCount;
	};
}