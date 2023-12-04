#include <glm/glm.hpp>

namespace drk::Lights::Models {
	struct PointLight {
		glm::vec4 relativePosition;
		glm::vec4 absolutePosition;

		glm::mat4 perspective;

		glm::mat4 topView;
		glm::mat4 bottomView;
		glm::mat4 leftView;
		glm::mat4 rightView;
		glm::mat4 backView;
		glm::mat4 frontView;

		glm::vec4 topShadowMapRect;
		glm::vec4 bottomShadowMapRect;
		glm::vec4 leftShadowMapRect;
		glm::vec4 rightShadowMapRect;
		glm::vec4 backShadowMapRect;
		glm::vec4 frontShadowMapRect;

		glm::vec4 normalizedTopShadowMapRect;
		glm::vec4 normalizedBottomShadowMapRect;
		glm::vec4 normalizedLeftShadowMapRect;
		glm::vec4 normalizedRightShadowMapRect;
		glm::vec4 normalizedBackShadowMapRect;
		glm::vec4 normalizedFrontShadowMapRect;

		float constantAttenuation;
		float linearAttenuation;
		float quadraticAttenuation;
		float far;
	};
}