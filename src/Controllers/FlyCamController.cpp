#include "FlyCamController.hpp"
#include "../Cameras/Components/Camera.hpp"
#include "../Spatials/Components/Spatial.hpp"
#include "../GlmExtensions.hpp"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "../Graphics/SynchronizationState.hpp"
#include "../Spatials/Models/Spatial.hpp"
#include "../Common/Components/Dirty.hpp"
#include <imgui.h>

namespace drk::Controllers {

	FlyCamController::FlyCamController(entt::registry& registry) : Registry(registry) {}

	void FlyCamController::Attach(entt::entity cameraEntity) {
		CameraEntity = cameraEntity;
	}

	void FlyCamController::HandleKeyboardEvents() {
		if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Z)) {
			MoveForward = true;
		}
		if (ImGui::IsKeyReleased(ImGuiKey::ImGuiKey_Z)) {
			MoveForward = false;
		}
		if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_S)) {
			MoveBackward = true;
		}
		if (ImGui::IsKeyReleased(ImGuiKey::ImGuiKey_S)) {
			MoveBackward = false;
		}
		if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Q)) {
			MoveLeft = true;
		}
		if (ImGui::IsKeyReleased(ImGuiKey::ImGuiKey_Q)) {
			MoveLeft = false;
		}
		if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_D)) {
			MoveRight = true;
		}
		if (ImGui::IsKeyReleased(ImGuiKey::ImGuiKey_D)) {
			MoveRight = false;
		}
		if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Space)) {
			MoveUp = true;
		}
		if (ImGui::IsKeyReleased(ImGuiKey::ImGuiKey_Space)) {
			MoveUp = false;
		}
		if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_LeftCtrl)) {
			MoveDown = true;
		}
		if (ImGui::IsKeyReleased(ImGuiKey::ImGuiKey_LeftCtrl)) {
			MoveDown = false;
		}

	}

	void FlyCamController::OnCursorPositionEvent(double xpos, double ypos) {
		glm::vec2 mousePosition{xpos, ypos};
		if (LastMousePosition.has_value()) {
			MousePositionDelta = (mousePosition - *LastMousePosition) * 0.0005f;
		}
		LastMousePosition = mousePosition;
	}

	void FlyCamController::OnMouseButtonEvent(int button, int action, int mods) {

	}

	void FlyCamController::Step() {
		const auto& [camera, spatial] = Registry.get<
			Cameras::Components::Camera,
			Spatials::Components::Spatial<Spatials::Components::Relative>
		>(CameraEntity);
		auto up = camera.absoluteUp;
		auto front = camera.absoluteFront;
		auto side = GlmExtensions::cross(front, up);

		auto hasUpdate = false;

		if (MoveForward) {
			spatial.position += front * 0.01f;
			hasUpdate = true;
		}
		if (MoveBackward) {
			spatial.position -= front * 0.01f;
			hasUpdate = true;
		}
		if (MoveRight) {
			spatial.position += side * 0.005f;
			hasUpdate = true;
		}
		if (MoveLeft) {
			spatial.position -= side * 0.005f;
			hasUpdate = true;
		}
		if (MoveUp) {
			spatial.position += spatial.model * camera.relativeUp * 0.005f;
			hasUpdate = true;
		}
		if (MoveDown) {
			spatial.position -= spatial.model * camera.relativeUp * 0.005f;
			hasUpdate = true;
		}

		if (MousePositionDelta.x != 0 || MousePositionDelta.y != 0) {
			if (MousePositionDelta.x) {
				auto q = glm::angleAxis(-MousePositionDelta.x, glm::vec3(GlmExtensions::up));
				spatial.rotation = q * spatial.rotation;
				MousePositionDelta.x = 0;
			}
			if (MousePositionDelta.y) {
				auto side = glm::vec3(GlmExtensions::cross(front, GlmExtensions::up));
				auto q = glm::angleAxis(-MousePositionDelta.y, side);
				spatial.rotation = q * spatial.rotation;
				MousePositionDelta.y = 0;
			}
			hasUpdate = true;
		}
		if (hasUpdate) {
			Registry.emplace_or_replace<Graphics::SynchronizationState<Spatials::Models::Spatial>>(CameraEntity, 2u);
			Registry.emplace_or_replace<Common::Components::Dirty<Spatials::Components::Spatial<Spatials::Components::Relative>>>(
				CameraEntity
			);
		}
	}
}
