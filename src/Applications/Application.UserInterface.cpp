#include "Application.hpp"
#include <iostream>
#include "../Spatials/Components/SpatialEditor.hpp"
#include "../Cameras/Editors/CameraEditor.hpp"
#include <entt/entt.hpp>
#include <stack>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <imgui.h>
#include <imfilebrowser.h>
#include "../Common/Components/Name.hpp"

namespace drk::Applications {
	void Application::renderEntities() {
		const auto& relationships = registry.view<Objects::Components::Relationship>();
		relationships.each(
			[this](entt::entity entity, Objects::Components::Relationship& relationship) {
				if (relationship.parent == entt::null) renderEntity(entity);
			}
		);
	}

	void Application::renderEntity(entt::entity entity) {
		const auto& [relationship, object] = registry.get<Objects::Components::Relationship, Objects::Components::Object>(entity);
		if (relationship.children.size() > 0) {
			auto isOpen = ImGui::TreeNode(
				(void*)entity,
				fmt::format("{0}", object.Name).c_str()
			);
			if (ImGui::IsItemClicked()) {
				selectedEntity = entity;
			}
			if (isOpen) {
				for (const auto& childEntity : relationship.children) {
					renderEntity(childEntity);
				}
				ImGui::TreePop();
			}
		}
		else {
			ImGui::Text(fmt::format("{0}", object.Name).c_str());
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
				ImGui::Text(typeInfo.name().data());
				auto spatialComponentTypeId = entt::type_id<Spatials::Components::Spatial<Spatials::Components::Relative>>();
				auto cameraComponentTypeId = entt::type_id<Cameras::Components::Camera>();
				if (typeInfo == spatialComponentTypeId) {
					auto& spatial = registry.get<Spatials::Components::Spatial<Spatials::Components::Relative>>(entity);
					if (Spatials::Components::SpatialEditor::Spatial(spatial)) {
						Spatials::Systems::SpatialSystem::makeDirty(registry, entity);
					}
				}
				if (typeInfo == cameraComponentTypeId) {
					auto previousCamera = engineState.cameraEntity;
					if (Cameras::Editors::CameraEditor::setActiveCamera(entity, globalSystem)) {
						if (previousCamera != entity) {
							flyCamController.Attach(entity);
						}
					}
				}
			}
		}
		ImGui::End();
	};

	void Application::renderInspector() {
		ImGui::Begin("Inspector");
		registry.each([this](entt::entity entity) {
			auto nameComponent = registry.try_get<Common::Components::Name>(entity);
			if (nameComponent != nullptr) {
				ImGui::Text(nameComponent->name.c_str());
			}
			else {
				ImGui::Text(fmt::format("entity #{0}", (int)entity).c_str());
			}
			if (ImGui::IsItemClicked()) {
				selectedEntity = entity;
			}
			});
		ImGui::End();
	};

	void Application::renderAnimations() {
		ImGui::Begin("Animations");
		auto animationView = registry.view<
			Animations::Components::Animation,
			Common::Components::Name
		>();
		animationView.each([](
			entt::entity animationEntity,
			const Animations::Components::Animation animation,
			const Common::Components::Name& name
			) {
				ImGui::Text(name.name.c_str());
			});
		ImGui::End();
	};

	void Application::renderSystemInfos() {
		ImGui::Begin("System");
		ImGui::Text(fmt::format("{0}", engineState.getDuration()).c_str());
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
							"frame:{0}, type:{1}, buffer index:{2}, count:{3}",
							frameIndex,
							store.first.name(),
							storeBuffer.get()->descriptorArrayElement,
							storeBuffer.get()->count
						).c_str());
				}
			}
			frameIndex++;
		}
		ImGui::End();
	};
}