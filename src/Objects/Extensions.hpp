#pragma once

#include <boost/di.hpp>
#include "ObjectSystem.hpp"
namespace drk::Objects {
	auto AddObjects() {
		return boost::di::make_injector(
			boost::di::bind<ObjectSystem>.to<ObjectSystem>()
		);
	}
}