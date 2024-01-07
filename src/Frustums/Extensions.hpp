#include "./Pipelines/FrustumPipeline.hpp"
#include "./Systems/FrustumSystem.hpp"

namespace drk::Frustums {
	auto AddFrustums() {
		return boost::di::make_injector(
			boost::di::bind<Pipelines::FrustumPipeline>.to<Pipelines::FrustumPipeline>(),
			boost::di::bind<Systems::FrustumSystem>.to<Systems::FrustumSystem>()
		);
	}
}