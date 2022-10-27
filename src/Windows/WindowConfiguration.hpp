#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include <nameof.hpp>

namespace drk::Windows {
	struct WindowConfiguration {
		std::string Title;
		uint32_t Width;
		uint32_t Height;
	};
}