#pragma once

#include <boost/di.hpp>
#include "Systems/NodeSystem.hpp"

namespace drk::Nodes {
	auto AddObjects() {
		return boost::di::make_injector(
			boost::di::bind<Systems::NodeSystem>.to<Systems::NodeSystem>()
		);
	}
}