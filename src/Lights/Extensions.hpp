#pragma once

#include <boost/di.hpp>
#include "LightSystem.hpp"
#include "DirectionalLightSystem.hpp"
#include "SpotlightSystem.hpp"
#include "PointLightSystem.hpp"

namespace drk::Lights {
	auto AddLights() {
		return boost::di::make_injector(
			boost::di::bind<LightSystem>.to<LightSystem>(),
			boost::di::bind<PointLightSystem>.to<PointLightSystem>(),
			boost::di::bind<SpotlightSystem>.to<SpotlightSystem>(),
			boost::di::bind<DirectionalLightSystem>.to<DirectionalLightSystem>()
		);
	}
}