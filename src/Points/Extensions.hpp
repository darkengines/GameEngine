#pragma once

#include <boost/di.hpp>
#include "Systems/PointSystem.hpp"
#include "Pipelines/PointPrimitivePipeline.hpp"

namespace drk::Points {
	auto AddPoints() {
		return boost::di::make_injector(
			boost::di::bind<Systems::PointSystem>.to<Systems::PointSystem>(),
			boost::di::bind<Points::Pipelines::PointPrimitivePipeline>.to<Points::Pipelines::PointPrimitivePipeline>().in(
				boost::di::unique
			)
		);
	}
}