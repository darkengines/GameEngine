#pragma once
#include <fruit/fruit.h>

#include <boost/di.hpp>

#include "FlyCamController.hpp"
#include "../Extensions.hpp"

namespace drk::Controllers {
fruit::Component<FlyCamController> addControllers() {
	return fruit::createComponent()
		.registerConstructor<FlyCamController(entt::registry&)>()
		.install(drk::addRegistry);
}
auto AddControllers() {
	return boost::di::make_injector(
		boost::di::bind<FlyCamController>.to<FlyCamController>()
	);
}
}  // namespace drk::Controllers