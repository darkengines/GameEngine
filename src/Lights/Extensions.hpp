#pragma once

#include <boost/di.hpp>
#include "./Systems/LightSystem.hpp"
#include "./Systems/DirectionalLightSystem.hpp"
#include "./Systems/SpotlightSystem.hpp"
#include "./Systems/PointLightSystem.hpp"
#include "../Common/KGuillotineAllocator.hpp"
#include "./Systems/LightPerspectiveSystem.hpp"

namespace drk::Lights {
	auto AddLights() {
		return boost::di::make_injector(
			boost::di::bind<Systems::LightSystem>.to<Systems::LightSystem>(),
			boost::di::bind<Systems::PointLightSystem>.to<Systems::PointLightSystem>(),
			boost::di::bind<Systems::SpotlightSystem>.to<Systems::SpotlightSystem>(),
			boost::di::bind<Systems::DirectionalLightSystem>.to<Systems::DirectionalLightSystem>(),
			boost::di::bind<Systems::LightPerspectiveSystem>.to<Systems::LightPerspectiveSystem>(),
			boost::di::bind<drk::guillotine::Allocator>.to(drk::guillotine::Allocator({ 2048, 2048 }, {}))
		);
	}
}