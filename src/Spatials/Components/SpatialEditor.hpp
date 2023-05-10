
#pragma once

#include "Spatial.hpp"
#include <imgui.h>
#include <vector>

namespace drk::Spatials::Components {
	class SpatialEditor {
	public:
		static bool Spatial(Spatial& spatial) {
			bool hasChanged = false;
			bool orientationChanged = false;
			glm::vec4 speed{0, 0, 0, 0};

			ImGui::SeparatorText("Spatial");
			hasChanged |= ImGui::InputFloat3("Position", reinterpret_cast<float*>(&spatial.relativePosition));
			if (ImGui::DragFloat3("Drag position", reinterpret_cast<float*>(&speed), 0.01f, -1.0f, 1.0f)) {
				auto hasSpeed = speed.x != 0 || speed.y != 0 || speed.z != 0;
				hasChanged |= hasSpeed;
				if (hasChanged) {
					spatial.relativePosition += speed;
				}
			}
			hasChanged |= ImGui::InputFloat3("Scale", reinterpret_cast<float*>(&spatial.relativeScale));

			glm::vec3 orientation = glm::eulerAngles(spatial.relativeRotation);
			orientationChanged = ImGui::InputFloat3("Orientation", reinterpret_cast<float*>(&orientation));

			auto pi = glm::pi<float>();
			if (ImGui::DragFloat3("Drag orientation", reinterpret_cast<float*>(&speed), 0.01f, -pi, pi)) {
				auto hasSpeed = speed.x != 0 || speed.y != 0 || speed.z != 0;
				orientationChanged |= hasSpeed;
				if (orientationChanged) {
					spatial.relativeRotation = glm::angleAxis(
						glm::length(speed),
						glm::normalize(glm::make_vec3(speed))) * spatial.relativeRotation;
				}
			} else if (orientationChanged) {
				spatial.relativeRotation = glm::quat(orientation);
			}
			hasChanged |= orientationChanged;
			return hasChanged;
		}
	};
}