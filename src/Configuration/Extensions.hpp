#pragma once
#include <fruit/fruit.h>

#include <boost/di.hpp>

#include "Configuration.hpp"

namespace drk::Configuration {
fruit::Component<drk::Configuration::Configuration> addConfiguration() {
	return fruit::createComponent() 
		.registerProvider([]() { return Configuration::loadConfiguration2("settings.json"); });
}
auto AddConfiguration() {
	return boost::di::make_injector(
		boost::di::bind<Configuration>.to(Configuration::loadConfiguration("settings.json"))
	);
}
}  // namespace drk::Configuration
