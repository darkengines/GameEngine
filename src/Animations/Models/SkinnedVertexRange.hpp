#pragma once
#include "../../Stores/StoreItem.hpp"
#include "../../Nodes/Models/Node.hpp"
#include "../Models/VertexWeight.hpp"
#include "../Models/Bone.hpp"

namespace drk::Animations::Models {
	struct SkinnedVertexRange {
		uint32_t vertexBufferIndex;
		uint32_t vertexBufferItemOffset;
		uint32_t vertexIndex;
		uint32_t skinnedVertexBufferIndex;
		uint32_t skinnedVertexBufferItemOffset;
		uint32_t skinnedVertexIndex;
		uint32_t vertexWeightBufferIndex;
		uint32_t vertexWeightBufferItemOffset;
		uint32_t vertexWeightIndex;
		uint32_t vertexWeightCount;
	};
}