
#pragma once

#include "../Loaders/AssimpLoader.hpp"

#include <iostream>
#include <regex>
#include <imgui.h>
#include "../ImGui/imgui_stdlib.h"
#include <imfilebrowser.h>

namespace drk::UserInterfaces {
	class AssetExplorer {
	protected:
		ImGui::FileBrowser fileBrowser;
		std::string directoryPath;
		drk::Loaders::AssimpLoader assimpLoader;
		entt::registry assetRegistry;
		std::vector<Loaders::LoadResult> assetLoadResults;
	public:
		AssetExplorer(drk::Loaders::AssimpLoader& assimpLoader);
		void render(entt::registry& destinationRegistry);
		void renderEntity(entt::registry& destinationRegistry, entt::entity entity);
		void renderEntities(entt::registry& destinationRegistry);
	};
}