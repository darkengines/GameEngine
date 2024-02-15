#pragma once

#include <boost/di.hpp>
#include "Systems/ObjectSystem.hpp"

namespace drk::Nodes {
	auto AddObjects() {
		return boost::di::make_injector(
			boost::di::bind<Systems::ObjectSystem>.to<Systems::ObjectSystem>()
		);
	}
}