#include "../../Stores/StoreItem.hpp"
#include "../../Meshes/Vertex.hpp"

namespace drk::Animations::Components {
	struct SkinnedVertexRange {
		uint32_t vertexIndex;
		uint32_t weightOffset;
		uint32_t weightCount;
	};
}