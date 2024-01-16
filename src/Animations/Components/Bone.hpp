#include <vector>
#include <glm/glm.hpp>
#include "../../Spatials/Components/Spatial.hpp"
#include "./VertexWeight.hpp"

namespace drk::Animations::Components {
	struct Bone {
		std::vector<VertexWeight> weights;
		Spatials::Components::Spatial spatialOffset;
	};
}