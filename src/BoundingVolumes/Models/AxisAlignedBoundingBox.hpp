#include <glm/glm.hpp>

namespace drk::BoundingVolumes::Models {
	struct AxisAlignedBoundingBox {
		glm::vec4 center;
		glm::vec4 extent;
	};
}