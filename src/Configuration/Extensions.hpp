#pragma once

#include <filesystem>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <boost/di.hpp>
#include <memory>

namespace drk::Configuration {
	struct Configuration {
		explicit Configuration(const nlohmann::json& jsonConfiguration) : jsonConfiguration(jsonConfiguration) {};
		nlohmann::json jsonConfiguration;
	};

	static std::shared_ptr<Configuration> loadConfiguration(const std::filesystem::path& inputConfigurationPath) {
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
	static auto AddConfiguration() {
		return boost::di::make_injector(
			boost::di::bind < Configuration >.to(loadConfiguration("settings.json"))
		);
	}
}
