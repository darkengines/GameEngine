
#include "AssetExplorer.hpp"
#include "../Objects/Components/Relationship.hpp"
#include "../Objects/Components/Object.hpp"
#include "../Objects/Systems/ObjectSystem.hpp"
#include "../Spatials/Systems/SpatialSystem.hpp"
#include "../UserInterfaces/UserInterface.hpp"

namespace drk::UserInterfaces {
	AssetExplorer::AssetExplorer(Loaders::AssimpLoader& assimpLoader) :
		assimpLoader(assimpLoader),
		fileBrowser(ImGui::FileBrowser{ImGuiFileBrowserFlags_SelectDirectory}),
		directoryPath("./") {

	}
	void AssetExplorer::renderEntities(entt::registry& destinationRegistry) {
		const auto& relationships = assetRegistry.view<Objects::Components::Relationship>();
		relationships.each(
			[this, &destinationRegistry](entt::entity entity, Objects::Components::Relationship& relationship) {
				if (relationship.parent == entt::null) renderEntity(destinationRegistry, entity);
			}
		);
	}

	void AssetExplorer::renderEntity(entt::registry& destinationRegistry, entt::entity entity) {
		const auto& [relationship, object] = assetRegistry.get<Objects::Components::Relationship, Objects::Components::Object>(
			entity
		);
		if (relationship.children.size() > 0) {
			auto isOpen = ImGui::TreeNode(
				(void*) entity,
				fmt::format("{0}", object.Name).c_str()
			);
			ImGui::SameLine();
			ImGui::PushID((void*) entity);
			if (ImGui::Button("Copy##node")) {
				Objects::Systems::ObjectSystem::copyObjectEntity(assetRegistry, destinationRegistry, entity);
			}
			ImGui::PopID();
			if (isOpen) {
				for (const auto& childEntity: relationship.children) {
					renderEntity(destinationRegistry, childEntity);
				}
				ImGui::TreePop();
			}
		} else {
			ImGui::Text(fmt::format("{0}", object.Name).c_str());
			ImGui::SameLine();
			ImGui::PushID((void*) entity);
			if (ImGui::Button("Copy##leaf")) {
				Objects::Systems::ObjectSystem::copyObjectEntity(assetRegistry, destinationRegistry, entity);
			}
			ImGui::PopID();
		}
	}
	void AssetExplorer::render(entt::registry& destinationRegistry) {
		ImGui::Begin("Asset explorer");
		ImGui::InputText("Directory", &directoryPath);
		ImGui::SameLine();
		if (ImGui::Button("Browse")) {
			fileBrowser.Open();
		}
		renderEntities(destinationRegistry);
		ImGui::End();
		fileBrowser.Display();
		if (fileBrowser.HasSelected()) {
			directoryPath = fileBrowser.GetSelected().string();
			fileBrowser.SetPwd(directoryPath);
			fileBrowser.ClearSelected();
			std::regex self_regex(
				".gltf|.obj|.glb",
				std::regex_constants::ECMAScript | std::regex_constants::icase
			);
			std::vector<std::filesystem::directory_entry> filePaths;
			for (const auto& entry: std::filesystem::recursive_directory_iterator(directoryPath)) {
				if (std::filesystem::is_regular_file(entry) &&
					std::regex_search(entry.path().extension().string(), self_regex)) {
					try {
						filePaths.push_back(entry);
						auto result = assimpLoader.Load(entry, assetRegistry);
						assetLoadResults.emplace_back(std::move(result));
						std::wcout << entry << std::endl;
					} catch (std::exception exception) {
						std::cerr << fmt::format(
							"Failed to load file {0}, see exception bellow:\r\n{1}",
							entry.path().string(),
							exception.what()) << std::endl;
					}
				}
			}
			assetRegistry.sort<Objects::Components::Relationship>(
				[this](const entt::entity left, const entt::entity right) {
					return Spatials::Systems::SpatialSystem::GetDepth(assetRegistry, left) <
						   Spatials::Systems::SpatialSystem::GetDepth(assetRegistry, right) || left < right;
				}
			);
		}
	}
}