#pragma once

#include <boost/di.hpp>
#include "SpatialSystem.hpp"
namespace drk::Spatials {
	auto AddSpatials() {
		return boost::di::make_injector(
			boost::di::bind<SpatialSystem>.to<SpatialSystem>()
		);
	}
}