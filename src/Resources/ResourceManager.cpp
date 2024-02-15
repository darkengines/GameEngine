#include <nameof.hpp>
#include "ResourceManager.hpp"

namespace drk::Resources {
	Configuration::ResourcesConfiguration
	ResourceManager::loadConfiguration(const drk::Configuration::Configuration& configuration) {
		return configuration.jsonConfiguration[std::string(nameof::nameof_short_type<ResourceManager>()).c_str()].get<Configuration::ResourcesConfiguration>();
	}
	ResourceManager::ResourceManager(const drk::Configuration::Configuration& configuration) : configuration(
		ResourceManager::loadConfiguration(configuration)) {
	}
}