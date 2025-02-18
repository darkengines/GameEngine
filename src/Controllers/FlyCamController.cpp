#include "FlyCamController.hpp"

#include <imgui.h>

#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "../Cameras/Components/Camera.hpp"
#include "../Common/Components/Dirty.hpp"
#include "../GlmExtensions.hpp"
#include "../Graphics/SynchronizationState.hpp"
#include "../Spatials/Components/Spatial.hpp"
#include "../Spatials/Models/Spatial.hpp"

namespace drk::Controllers {

FlyCamController::FlyCamController(entt::registry& registry) : Registry(registry) {}

void FlyCamController::Attach(entt::entity cameraEntity) {
	CameraEntity = cameraEntity;
	const auto& [camera, relativeSpatial] = Registry.get<Cameras::Components::Camera, Spatials::Components::Spatial<Spatials::Components::Relative>>(CameraEntity);
	auto eulers = glm::eulerAngles(relativeSpatial.rotation);
	pitch = eulers.x;
	yaw = eulers.y;
	roll = eulers.z;
	relativeSpatial.rotation = glm::quat(glm::vec3(pitch, yaw, roll));
	Registry.emplace_or_replace<Graphics::SynchronizationState<Spatials::Models::Spatial>>(CameraEntity, 2u);
	Registry.emplace_or_replace<Common::Components::Dirty<Spatials::Components::Spatial<Spatials::Components::Relative>>>(CameraEntity);
}

void FlyCamController::HandleKeyboardEvents() {
	if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Z) && forward >= 0)
		forward--;
	if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_S) && forward <= 0)
		forward++;
	if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Q) && right >= 0)
		right--;
	if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_D) && right <= 0)
		right++;
	if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Space) && up <= 0)
		up++;
	if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_LeftCtrl) && up >= 0)
		up--;
	if (ImGui::IsKeyReleased(ImGuiKey::ImGuiKey_Z) && forward <= 0)
		forward++;
	if (ImGui::IsKeyReleased(ImGuiKey::ImGuiKey_S) && forward >= 0)
		forward--;
	if (ImGui::IsKeyReleased(ImGuiKey::ImGuiKey_Q) && right <= 0)
		right++;
	if (ImGui::IsKeyReleased(ImGuiKey::ImGuiKey_D) && right >= 0)
		right--;
	if (ImGui::IsKeyReleased(ImGuiKey::ImGuiKey_Space) && up >= 0)
		up--;
	if (ImGui::IsKeyReleased(ImGuiKey::ImGuiKey_LeftCtrl) && up <= 0)
		up++;
}

void FlyCamController::OnCursorPositionEvent(double xpos, double ypos) { 
	if (!MousePosition.has_value()) {
		MousePosition = glm::vec2(xpos, ypos); 
	} else {
		MousePosition->x = xpos;
		MousePosition->y = ypos;
	}
}

void FlyCamController::OnMouseButtonEvent(int button, int action, int mods) {}

void FlyCamController::Step(float dt) {
	const auto& [camera, relativeSpatial] = Registry.get<Cameras::Components::Camera,
	Spatials::Components::Spatial<Spatials::Components::Relative>>(CameraEntity);
	auto inputs = glm::vec3(right, up, forward);
	auto hasPositionInputs = glm::length(inputs) > std::numeric_limits<float>::epsilon();
	if (hasPositionInputs) {
		auto movement = glm::normalize(inputs);
		relativeSpatial.position += glm::vec4(relativeSpatial.rotation * (inputs * 10.f * dt), 0);

		auto hasUpdate = true;
	}

	auto hasOrientationInputs = false;
	if (MousePosition.has_value()) {
		if (LastMousePosition.has_value()) {
			auto delta = *LastMousePosition - *MousePosition;
			hasOrientationInputs = delta.x != 0 || delta.y != 0;
			if (hasOrientationInputs) {
				if (roll > glm::half_pi<float>() || roll < -glm::half_pi<float>()) {
					yaw -= delta.x * 0.0005f;
				} else {
					yaw += delta.x * 0.0005f;
				}
				pitch += delta.y * 0.0005f;
			}
			relativeSpatial.rotation = glm::quat(glm::vec3(pitch, yaw, roll));
		}
		LastMousePosition = MousePosition;
	}

	auto hasUpdate = hasPositionInputs || hasOrientationInputs;
	if (hasUpdate) {
		Registry.emplace_or_replace<Graphics::SynchronizationState<Spatials::Models::Spatial>>(CameraEntity, 2u);
		Registry.emplace_or_replace<Common::Components::Dirty<Spatials::Components::Spatial<Spatials::Components::Relative>>>(CameraEntity);
	}
}
}  // namespace drk::Controllers
