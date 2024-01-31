#include "../../Stores/StoreItem.hpp"
#include "../../Objects/Models/Object.hpp"
#include "../Models/Bone.hpp"
#include "../Models/VertexWeight.hpp"
#include "../Components/VertexWeight.hpp"

namespace drk::Animations::Components {
	struct VertexWeightInput {
		Components::VertexWeight const * pVertexWeight;
		uint32_t vertexWeightCount;
	};
}