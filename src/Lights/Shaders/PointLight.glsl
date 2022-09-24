struct PointLight {
	vec4 relativePosition;
	vec4 absolutePosition;

	mat4 perspective;

	mat4 topView;
	mat4 bottomView;
	mat4 leftView;
	mat4 rightView;
	mat4 backView;
	mat4 frontView;

	vec4 topShadowMapRect;
	vec4 bottomShadowMapRect;
	vec4 leftShadowMapRect;
	vec4 rightShadowMapRect;
	vec4 backShadowMapRect;
	vec4 frontShadowMapRect;

	vec4 normalizedTopShadowMapRect;
	vec4 normalizedBottomShadowMapRect;
	vec4 normalizedLeftShadowMapRect;
	vec4 normalizedRightShadowMapRect;
	vec4 normalizedBackShadowMapRect;
	vec4 normalizedFrontShadowMapRect;

	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
	float far;
	uint lightStateId;
};