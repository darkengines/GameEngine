#include "../../Stores/StoreItem.hpp"
#include "../../Objects/Models/Object.hpp"
#include "../Models/Bone.hpp"
#include "../Models/VertexWeight.hpp"
#include "../Components/VertexWeight.hpp"

namespace drk::Animations::Components {
	struct VertexWeightInputMap {
		uint32_t vertexArrayElement;
		uint32_t vertexIndex;
		uint32_t skinnedVertexArrayElement;
		uint32_t skinnedVertexIndex;
	};
}