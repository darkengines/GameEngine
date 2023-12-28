#pragma once

#include <boost/di.hpp>
#include "Systems/LineSystem.hpp"
#include "Pipelines/LinePipeline.hpp"
namespace drk::Lines {
	auto AddLines() {
		return boost::di::make_injector(
			boost::di::bind<Systems::LineSystem>.to<Systems::LineSystem>(),
			boost::di::bind<Lines::Pipelines::LinePipeline>.to<Lines::Pipelines::LinePipeline>().in(boost::di::unique)
		);
	}
}