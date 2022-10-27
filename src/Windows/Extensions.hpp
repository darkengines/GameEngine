#pragma once

#include <boost/di.hpp>
#include <GLFW/glfw3.h>
#include "Window.hpp"
#include <memory>
#include <nameof.hpp>
#include "../Configuration/Extensions.hpp"
#include "WindowConfiguration.hpp"
#include <nlohmann/json.hpp>

namespace drk::Windows {

	static auto AddWindows() {
		return boost::di::make_injector(boost::di::bind<Window>.to<Window>());
	}
}
