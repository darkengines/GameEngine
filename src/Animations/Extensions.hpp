#pragma once

#include <boost/di.hpp>
#include "Systems/AnimationSystem.hpp"
#include "Systems/BoneMeshSystem.hpp"
#include "Systems/BoneSpatialSystem.hpp"
#include "Pipelines/SkinningPipeline.hpp"
#include "Resources/AnimationResourceManager.hpp"

namespace drk::Animations {
	auto AddAnimations() {
		return boost::di::make_injector(
			boost::di::bind<Systems::AnimationSystem>.to<Systems::AnimationSystem>(),
			boost::di::bind<Resources::AnimationResourceManager>.to<Resources::AnimationResourceManager>(),
			boost::di::bind<Pipelines::SkinningPipeline>.to<Pipelines::SkinningPipeline>(),
			boost::di::bind<Systems::BoneMeshSystem>.to<Systems::BoneMeshSystem>(),
			boost::di::bind<Systems::BoneSpatialSystem>.to<Systems::BoneSpatialSystem>()
		);
	}
}