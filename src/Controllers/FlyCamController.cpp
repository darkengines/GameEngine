#include "FlyCamController.hpp"
#include "../Cameras/Components/Camera.hpp"
#include "../Spatials/Components/Spatial.hpp"
#include "../GlmExtensions.hpp"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/quaternion.hpp>
#include "../Graphics/SynchronizationState.hpp"
#include "../Spatials/Models/Spatial.hpp"
#include "../Objects/Dirty.hpp"
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
		LastMousePosition = std::move(mousePosition);
	}

	void FlyCamController::OnMouseButtonEvent(int button, int action, int mods) {

	}

	void FlyCamController::Step() {
		auto& camera = Registry.get<Cameras::Components::Camera>(CameraEntity);
		auto& cameraSpatial = Registry.get<Spatials::Components::Spatial>(CameraEntity);
		auto up = camera.absoluteUp;
		auto front = camera.absoluteFront;
		auto side = GlmExtensions::cross(front, up);

		auto hasUpdate = false;

		if (MoveForward) {
			cameraSpatial.relativePosition += front * 0.01f;
			hasUpdate = true;
		}
		if (MoveBackward) {
			cameraSpatial.relativePosition -= front * 0.01f;
			hasUpdate = true;
		}
		if (MoveRight) {
			cameraSpatial.relativePosition += side * 0.005f;
			hasUpdate = true;
		}
		if (MoveLeft) {
			cameraSpatial.relativePosition -= side * 0.005f;
			hasUpdate = true;
		}
		if (MoveUp) {
			cameraSpatial.relativePosition += camera.relativeUp * 0.005f;
			hasUpdate = true;
		}
		if (MoveDown) {
			cameraSpatial.relativePosition -= camera.relativeUp * 0.005f;
			hasUpdate = true;
		}

		if (MousePositionDelta.x != 0 || MousePositionDelta.y != 0) {
			if (MousePositionDelta.x) {
				auto q = glm::angleAxis(-MousePositionDelta.x, glm::vec3(camera.relativeUp));
				cameraSpatial.relativeRotation = q * cameraSpatial.relativeRotation;
				MousePositionDelta.x = 0;
			}
			if (MousePositionDelta.y) {
				cameraSpatial.relativeRotation = glm::rotate(
					cameraSpatial.relativeRotation,
					-MousePositionDelta.y,
					glm::vec3(GlmExtensions::cross(camera.relativeFront, camera.relativeUp))
				);
				MousePositionDelta.y = 0;
			}
			hasUpdate = true;
		}
		if (hasUpdate) {
			Registry.emplace_or_replace<Graphics::SynchronizationState<Spatials::Models::Spatial >>(CameraEntity, 2u);
			Registry.emplace_or_replace<Objects::Dirty<Spatials::Components::Spatial >>(CameraEntity);
		}
	}
}
