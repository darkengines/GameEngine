#include "./VertexWeightInstance.hpp"
#include "./SkinnedVertexRange.hpp"
#include <vector>

namespace drk::Animations::Components {
	struct SkinnedMeshInstance {
		std::vector<Components::SkinnedVertexRange> skinnedVertexRanges;
		std::vector<Components::VertexWeightInstance> skinnedVertices;
	};
}