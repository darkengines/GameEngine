#pragma once
#include <glm/ext.hpp>
#include <glm/glm.hpp>

namespace drk {
class GlmExtensions {
public:
	static glm::vec4 cross(const glm::vec4& left, const glm::vec4& right);
	static inline glm::vec4 front{0, 0, 1, 0};
	static inline glm::vec3 front3{0, 0, 1};
	static inline glm::vec4 back{0, 0, -1, 0};
	static inline glm::vec4 left{-1, 0, 0, 0};
	static inline glm::vec4 right{1, 0, 0, 0};
	static inline glm::vec3 right3{1, 0, 0 };
	static inline glm::vec4 up{0, 1, 0, 0};
	static inline glm::vec3 up3{0.f, 1.f, 0.f};
	static inline glm::vec4 down{0, -1, 0, 0};
};
}  // namespace drk
