#pragma once
#include <glm/vec4.hpp>
#include <optional>
#include <glm/mat4x4.hpp>

namespace drk::Lights {
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

		std::optional<glm::vec4> topShadowMapRect = std::nullopt;
		std::optional<glm::vec4> bottomShadowMapRect = std::nullopt;
		std::optional<glm::vec4> leftShadowMapRect = std::nullopt;
		std::optional<glm::vec4> rightShadowMapRect = std::nullopt;
		std::optional<glm::vec4> backShadowMapRect = std::nullopt;
		std::optional<glm::vec4> frontShadowMapRect = std::nullopt;

		std::optional<glm::vec4> normalizedTopShadowMapRect = std::nullopt;
		std::optional<glm::vec4> normalizedBottomShadowMapRect = std::nullopt;
		std::optional<glm::vec4> normalizedLeftShadowMapRect = std::nullopt;
		std::optional<glm::vec4> normalizedRightShadowMapRect = std::nullopt;
		std::optional<glm::vec4> normalizedBackShadowMapRect = std::nullopt;
		std::optional<glm::vec4> normalizedFrontShadowMapRect = std::nullopt;

		float constantAttenuation;
		float linearAttenuation;
		float quadraticAttenuation;
		float far;
	};
}
