#include "./ResourcesConfiguration.hpp"
#include <nameof.hpp>

namespace drk::Resources::Configuration {
	[[maybe_unused]] void to_json(nlohmann::json& j, const ResourcesConfiguration& p) {
		j = nlohmann::json{{NAMEOF(p.frameCount), p.frameCount}};
	}
	[[maybe_unused]] void from_json(const nlohmann::json& j, ResourcesConfiguration& p) {
		j.at(NAMEOF(p.frameCount).c_str()).get_to(p.frameCount);
	}
}