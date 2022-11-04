#pragma once

#include <boost/di.hpp>
#include "../Points/PointPrimitivePipeline.hpp"

namespace drk::Geometries {
	auto AddGeometries() {
		return boost::di::make_injector(
			boost::di::bind<PointPrimitivePipeline>.to<PointPrimitivePipeline>()
		);
	}
}