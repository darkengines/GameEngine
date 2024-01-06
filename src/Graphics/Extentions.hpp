#pragma once
#include <boost/di.hpp>
#include "../Engine/EngineState.hpp"

namespace drk::Graphics {
	auto AddGraphics() {
		return boost::di::make_injector(
			boost::di::bind<GlobalSystem>.to<GlobalSystem>(),
			boost::di::bind<Graphics>.to<Graphics>()
		);
	}
}