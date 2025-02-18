
#pragma once

#include <vulkan/vulkan.hpp>
#include <imgui.h>
#include <imfilebrowser.h>
#include "../Loaders/LoadResult.hpp"

namespace drk::Applications {
	struct ApplicationState {
		std::optional<vk::DescriptorSet> sceneTextureImageDescriptorSet;
		vk::Extent3D sceneExtent;
		vk::Extent3D actualExtent;
		std::vector<Loaders::LoadResult> loadResults;
		ImGui::FileBrowser fileBrowser;
		float frameTime;
		std::chrono::time_point<std::chrono::steady_clock> currentTime;
		bool isDemoWindowOpen = false;
	};
}