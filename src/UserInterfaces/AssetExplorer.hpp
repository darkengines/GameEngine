
#pragma once

#include <imgui.h>
#include "../ImGui/imgui_stdlib.h"
#include "../Loaders/AssimpLoader.hpp"

#include <imfilebrowser.h>
#include <iostream>
#include <regex>

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