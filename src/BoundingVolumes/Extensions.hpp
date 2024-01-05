#include "./Pipelines/BoundingVolumePipeline.hpp"
#include "./Systems/AxisAlignedBoundingBoxSystem.hpp"

namespace drk::BoundingVolumes {
	auto AddBoundingVolumes() {
		return boost::di::make_injector(
			boost::di::bind<Pipelines::BoundingVolumePipeline>.to<Pipelines::BoundingVolumePipeline>(),
			boost::di::bind<Systems::AxisAlignedBoundingBoxSystem>.to<Systems::AxisAlignedBoundingBoxSystem>()
		);
	}
}