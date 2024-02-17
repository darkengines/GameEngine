#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace drk {
	class GlmExtensions {
	public:
		static glm::vec4 cross(const glm::vec4& left, const glm::vec4& right);
		static inline glm::vec4 front{0, 0, 1, 0};
		static inline glm::vec4 back{0, 0, -1, 0};
		static inline glm::vec4 left{-1, 0, 0, 0};
		static inline glm::vec4 right{1, 0, 0, 0};
		static inline glm::vec4 up{0, 1, 0, 0};
		static inline glm::vec4 down{0, -1, 0, 0};
	};
}


