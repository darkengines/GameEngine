#pragma once
#include <glm/vec4.hpp>

namespace drk::Lights {
	struct Light {
		glm::vec4 ambientColor;
		glm::vec4 diffuseColor;
		glm::vec4 specularColor;
	};
}
