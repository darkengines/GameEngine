#pragma once

#include <boost/di.hpp>
#include "PointSystem.hpp"
#include "PointPrimitivePipeline.hpp"
namespace drk::Points {
	auto AddPoints() {
		return boost::di::make_injector(
			boost::di::bind<PointSystem>.to<PointSystem>(),
			boost::di::bind<Points::PointPrimitivePipeline>.to<Points::PointPrimitivePipeline>().in(boost::di::unique)
		);
	}
}