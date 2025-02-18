
#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <imgui.h>
#include <vector>

#include "../Components/Light.hpp"

namespace drk::Lights::Editors {
class LightEditor {
public:
	static bool Light(Components::Light& light);
};
}