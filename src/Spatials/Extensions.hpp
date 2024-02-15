#pragma once

#include <boost/di.hpp>
#include "Systems/SpatialSystem.hpp"
#include "Systems/RelativeSpatialSystem.hpp"

namespace drk::Spatials {
	auto AddSpatials() {
		return boost::di::make_injector(
			boost::di::bind<Systems::SpatialSystem>.to<Systems::SpatialSystem>(),
			boost::di::bind<Systems::RelativeSpatialSystem>.to<Systems::RelativeSpatialSystem>()
		);
	}
}