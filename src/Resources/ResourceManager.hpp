#pragma once
#include "../Configuration/Extensions.hpp"
#include "./Configuration/ResourcesConfiguration.hpp"

namespace drk::Resources {
	class ResourceManager {
	protected:
		static Configuration::ResourcesConfiguration
		loadConfiguration(const drk::Configuration::Configuration& configuration);
		const Configuration::ResourcesConfiguration configuration;
	public:
		ResourceManager(const drk::Configuration::Configuration& configuration);
	};
}