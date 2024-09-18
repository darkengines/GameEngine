#include "LightEditor.hpp"

#include "../../GlmExtensions.hpp"

namespace drk::Lights::Editors {

bool LightEditor::Light(Lights::Components::Light& light) {
	bool hasChanged = ImGui::ColorEdit3("Ambient color", glm::value_ptr(light.ambientColor), ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);
	hasChanged |= ImGui::ColorEdit3("Diffuse color", glm::value_ptr(light.diffuseColor), ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);
	hasChanged |= ImGui::ColorEdit3("Specular color", glm::value_ptr(light.specularColor), ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);
	return hasChanged;
}
}  // namespace drk::Lights::Editors