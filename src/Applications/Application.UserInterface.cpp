#define NOMINMAX
#include "Application.hpp"
#include <tuple>

#include "../Cameras/Editors/CameraEditor.hpp"
#include "../Common/Components/Name.hpp"
#include "../Lights/Editors/LightEditor.hpp"
#include "../Spatials/Components/SpatialEditor.hpp"

namespace drk::Applications {

void Application::renderEntities() {
	auto entities = registry.storage<entt::entity>().each();
	for (const auto& entity : entities) {
		renderEntity(std::get<0>(entity));
	}
	/*const auto& relationships = registry.view<Nodes::Components::Node>();
	relationships.each([this](entt::entity entity, Nodes::Components::Node& relationship) {
		if (relationship.parent == entt::null)
			renderEntity(entity);
	});*/
}

void Application::renderEntity(const entt::entity entity) {
	const auto& node = registry.try_get<Nodes::Components::Node>(entity);
	const auto& name = registry.try_get<Common::Components::Name>(entity);
	if (node && node->children.size() > 0) {
		auto isOpen = false;
		if (name) {
			isOpen = ImGui::TreeNode((void*)entity, fmt::format("{0}", name->name).c_str());
		} else {
			isOpen = ImGui::TreeNode((void*)entity, fmt::format("{0}", (int)entity).c_str());
		}
		if (ImGui::IsItemClicked()) {
			selectedEntity = entity;
		}
		if (isOpen) {
			for (const auto& childEntity : node->children) {
				renderEntity(childEntity);
			}
			ImGui::TreePop();
		}
	} else {
		if (name) {
			ImGui::Text(fmt::format("{0}", name->name).c_str());
		} else {
			ImGui::Text(fmt::format("{0}", (int)entity).c_str());
		}
		if (ImGui::IsItemClicked()) {
			selectedEntity = entity;
		}
	}
}

void Application::renderProperties(entt::entity entity) {
	ImGui::Begin("Properties");
	for (auto&& curr : registry.storage()) {
		entt::id_type id = curr.first;

		if (auto& storage = curr.second; storage.contains(entity)) {
			auto typeInfo = storage.type();
			auto component = storage.find(entity);
			auto spatialComponentTypeId = entt::type_id<Spatials::Components::Spatial<Spatials::Components::Relative>>();
			auto cameraComponentTypeId = entt::type_id<Cameras::Components::Camera>();
			auto lightComponentTypeId = entt::type_id<Lights::Components::Light>();
			ImGui::SeparatorText(typeInfo.name().data());
			if (typeInfo == lightComponentTypeId) {
				auto& light = registry.get<Lights::Components::Light>(entity);
				if (Lights::Editors::LightEditor::Light(light)) {
					registry.emplace_or_replace<Graphics::SynchronizationState<Lights::Models::Light>>(entity, engineState.getFrameCount());
				}
				continue;
			}
			if (typeInfo == spatialComponentTypeId) {
				auto& relativeSpatial = registry.get<Spatials::Components::Spatial<Spatials::Components::Relative>>(entity);
				const auto& absoluteSpatial = registry.get<Spatials::Components::Spatial<Spatials::Components::Absolute>>(entity);
				if (Spatials::Components::SpatialEditor::Spatial(relativeSpatial, absoluteSpatial)) {
					Spatials::Systems::SpatialSystem::makeDirty(registry, entity);
				}
				continue;
			}
			if (typeInfo == cameraComponentTypeId) {
				auto previousCamera = engineState.cameraEntity;
				if (Cameras::Editors::CameraEditor::setActiveCamera(entity, globalSystem)) {
					if (previousCamera != entity) {
						flyCamController.Attach(entity);
					}
				}
				continue;
			}
		}
	}
	ImGui::End();
};

void Application::renderAnimations() {
	ImGui::Begin("Animations");
	auto animationView = registry.view<Animations::Components::Animation, Common::Components::Name>();
	animationView.each([](entt::entity animationEntity, const Animations::Components::Animation animation, const Common::Components::Name& name) { ImGui::Text(name.name.c_str()); }
	);
	ImGui::End();
};

void Application::renderSystemInfos() {
	ImGui::Begin("System");
	ImGui::Text(fmt::format("FPS: {:f}", 1.0f / applicationState.frameTime).c_str());
	ImGui::Text(fmt::format("frame time: {0}", applicationState.frameTime).c_str());
	ImGui::End();
};

void Application::renderStorageBuffers() {
	ImGui::Begin("StorageBuffers");
	auto frameIndex = 0;
	for (auto& frameState : engineState.frameStates) {
		for (auto& store : frameState.Stores) {
			for (auto& storeBuffer : store.second.get()->stores) {
				ImGui::Text(
					fmt::format(
						"frame:{0}, type:{1}, buffer index:{2}, count:{3}", frameIndex, store.first.name(), storeBuffer.get()->descriptorArrayElement, storeBuffer.get()->count
					)
						.c_str()
				);
			}
		}
		frameIndex++;
	}
	ImGui::End();
};
}  // namespace drk::Applications
