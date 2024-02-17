#pragma once
#include <boost/di.hpp>
#include "Configuration.hpp"

namespace drk::Configuration {
	auto AddConfiguration() {
		return boost::di::make_injector(
			boost::di::bind<Configuration>.to(Configuration::loadConfiguration("settings.json"))
		);
	}
}
