#pragma once

#include <boost/di.hpp>
#include "LineSystem.hpp"
#include "LinePipeline.hpp"
namespace drk::Lines {
	auto AddLines() {
		return boost::di::make_injector(
			boost::di::bind<LineSystem>.to<LineSystem>(),
			boost::di::bind<Lines::LinePipeline>.to<Lines::LinePipeline>().in(boost::di::unique)
		);
	}
}