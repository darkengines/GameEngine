#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace drk {
	class GlmExtensions {
	public:
		static glm::vec4 cross(const glm::vec4& left, const glm::vec4& right);
	};
}


