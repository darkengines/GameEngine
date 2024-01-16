#pragma once
#include <stdint.h>
#include <nlohmann/json.hpp>

namespace drk::Resources::Configuration {
	struct ResourcesConfiguration {
		uint32_t frameCount;
	};
	void to_json(nlohmann::json& j, const ResourcesConfiguration& p);
	void from_json(const nlohmann::json& j, ResourcesConfiguration& p);
}