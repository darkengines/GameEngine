#pragma once
#include <boost/di.hpp>
#include "FlyCamController.hpp"

namespace drk::Controllers {
	auto AddControllers() {
		return boost::di::make_injector(
			boost::di::bind<FlyCamController>.to<FlyCamController>()
		);
	}
}