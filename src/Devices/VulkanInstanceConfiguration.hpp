#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace drk::Devices {
	struct VulkanInstanceConfiguration {
		std::vector<std::string> RequiredInstanceExtensions;
		std::vector<std::string> RequiredValidationLayers;
		std::vector<std::string> RequiredDeviceExtensions;
		bool EnableValidationLayers;
	};
	void to_json(nlohmann::json& j, const VulkanInstanceConfiguration& p);
	void from_json(const nlohmann::json& j, VulkanInstanceConfiguration& p);
}
