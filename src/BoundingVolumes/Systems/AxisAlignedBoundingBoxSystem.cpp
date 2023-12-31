#include "AxisAlignedBoundingBoxSystem.hpp"

namespace drk::BoundingVolumes::Systems {
	void AxisAlignedBoundingBoxSystem::update(
		Models::AxisAlignedBoundingBox& axisAlignedBoundingBoxModel, 
		const Components::AxisAlignedBoundingBox& axisAlignedBoundingBox
	) {
		axisAlignedBoundingBoxModel.center = axisAlignedBoundingBox.center;
		axisAlignedBoundingBoxModel.extent = axisAlignedBoundingBox.extent;
	}
}