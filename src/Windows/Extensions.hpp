#pragma once

#include <GLFW/glfw3.h>

#include <boost/di.hpp>
#include <memory>
#include <nameof.hpp>
#include <nlohmann/json.hpp>

#include "../Configuration/Configuration.hpp"
#include "../Configuration/Extensions.hpp"
#include "Window.hpp"
#include "WindowConfiguration.hpp"

namespace drk::Windows {
fruit::Component<drk::Windows::Window> addWindows() {
	return fruit::createComponent()
		.registerConstructor<drk::Windows::Window(
			const Configuration::Configuration&
		)>()
		.install(drk::Configuration::addConfiguration);
}
static auto AddWindows() {
	return boost::di::make_injector(boost::di::bind<drk::Windows::Window>.to<drk::Windows::Window>());
}
}  // namespace drk::Windows
