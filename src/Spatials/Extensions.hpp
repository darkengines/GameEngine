#pragma once

#include <boost/di.hpp>
#include "Systems/SpatialSystem.hpp"
namespace drk::Spatials {
	auto AddSpatials() {
		return boost::di::make_injector(
			boost::di::bind<Systems::SpatialSystem>.to<Systems::SpatialSystem>()
		);
	}
}