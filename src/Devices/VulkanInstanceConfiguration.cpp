#include <nameof.hpp>
#include "VulkanInstanceConfiguration.hpp"

namespace drk::Devices {
	void to_json(nlohmann::json& j, const VulkanInstanceConfiguration& p) {
		j = nlohmann::json{{NAMEOF(p.RequiredInstanceExtensions), p.RequiredInstanceExtensions}};
		j = nlohmann::json{{NAMEOF(p.RequiredValidationLayers), p.RequiredValidationLayers}};
		j = nlohmann::json{{NAMEOF(p.RequiredDeviceExtensions), p.RequiredDeviceExtensions}};
		j = nlohmann::json{{NAMEOF(p.EnableValidationLayers), p.EnableValidationLayers}};
	}
	void from_json(const nlohmann::json& j, VulkanInstanceConfiguration& p) {
		j.at(NAMEOF(p.RequiredInstanceExtensions).c_str()).get_to(p.RequiredInstanceExtensions);
		j.at(NAMEOF(p.RequiredValidationLayers).c_str()).get_to(p.RequiredValidationLayers);
		j.at(NAMEOF(p.RequiredDeviceExtensions).c_str()).get_to(p.RequiredDeviceExtensions);
		j.at(NAMEOF(p.EnableValidationLayers).c_str()).get_to(p.EnableValidationLayers);
	}
}
