#include "Configuration.hpp"

namespace drk::Configuration {
	std::shared_ptr<Configuration> Configuration::loadConfiguration(const std::filesystem::path& inputConfigurationPath) {
		std::ifstream inputConfigurationStream(inputConfigurationPath);
		std::string inputConfiguration;
		inputConfigurationStream.seekg(0, std::ios_base::end);
		auto size = inputConfigurationStream.tellg();
		inputConfiguration.resize(size);
		inputConfigurationStream.seekg(0);
		inputConfigurationStream.read(inputConfiguration.data(), inputConfiguration.size());
		nlohmann::json jsonConfiguration = nlohmann::json::parse(inputConfiguration);
		return std::make_shared<Configuration>(jsonConfiguration);
	}

	Configuration Configuration::loadConfiguration2(const std::filesystem::path& inputConfigurationPath) {
		std::ifstream inputConfigurationStream(inputConfigurationPath);
		std::string inputConfiguration;
		inputConfigurationStream.seekg(0, std::ios_base::end);
		auto size = inputConfigurationStream.tellg();
		inputConfiguration.resize(size);
		inputConfigurationStream.seekg(0);
		inputConfigurationStream.read(inputConfiguration.data(), inputConfiguration.size());
		nlohmann::json jsonConfiguration = nlohmann::json::parse(inputConfiguration);
		return Configuration{jsonConfiguration};
	}
	}