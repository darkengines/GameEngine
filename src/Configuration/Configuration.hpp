#pragma once

#include <filesystem>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <memory>

namespace drk::Configuration {
	struct Configuration {
		explicit Configuration(const nlohmann::json& jsonConfiguration) : jsonConfiguration(jsonConfiguration) {};
		nlohmann::json jsonConfiguration;
		static std::shared_ptr<Configuration> loadConfiguration(const std::filesystem::path& inputConfigurationPath);
	};
}