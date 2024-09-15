#include "SpatialEditor.hpp"
#include "../../GlmExtensions.hpp"

namespace drk::Spatials::Components {

bool SpatialEditor::Spatial(
	Spatials::Components::Spatial<Spatials::Components::Relative>& relativeSpatial,
	const Spatials::Components::Spatial<Spatials::Components::Absolute>& absoluteSpatial
) {
	bool hasChanged = false;
	bool orientationChanged;
	glm::vec3 speed{0, 0, 0};

	ImGui::SeparatorText("Spatial");
	hasChanged |= ImGui::InputFloat3("Position", (float*)&absoluteSpatial.position);
	//			if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_KeypadAdd)) {
	//				speed = 1.0f;
	//				if (ImGui::IsKeyPressed((ImGuiKey::ImGuiKey_LeftCtrl))) factor = 0.1f;
	//				if (ImGui::IsKeyPressed((ImGuiKey::ImGuiKey_LeftShift))) factor = 10.0f;
	//				hasChanged |= true;
	//			}
	if (ImGui::DragFloat3("Drag position", reinterpret_cast<float*>(&speed), 0.01f, -1.0f, 1.0f)) {
		auto hasSpeed = speed.x != 0 || speed.y != 0 || speed.z != 0;
		hasChanged |= hasSpeed;
		if (hasChanged) {
			relativeSpatial.position += glm::vec4(speed, 0.f);
		}
	}
	hasChanged |= ImGui::InputFloat3("Scale", reinterpret_cast<float*>(&relativeSpatial.scale));

	glm::vec3 orientation = glm::eulerAngles(relativeSpatial.rotation);
	orientationChanged = ImGui::InputFloat3("Orientation", reinterpret_cast<float*>(&orientation));

	auto pi = glm::pi<float>();
	speed = glm::zero<glm::vec3>();
	if (ImGui::DragFloat3("Drag orientation", reinterpret_cast<float*>(&speed), 0.1f)) {
		auto hasSpeed = speed.x != 0 || speed.y != 0 || speed.z != 0;
		orientationChanged |= hasSpeed;
		if (orientationChanged) {
			relativeSpatial.rotation *= glm::quat(speed);
		}
	} else if (orientationChanged) {
		// relativeSpatial.rotation = glm::quat(orientation) * glm::inverse(absoluteSpatial.rotation);
	}
	hasChanged |= orientationChanged;
	return hasChanged;
}
}  // namespace drk::Spatials::Components