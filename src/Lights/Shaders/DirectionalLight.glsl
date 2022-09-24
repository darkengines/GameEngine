struct DirectionalLight {
	mat4 perspective;
	mat4 view;
	vec4 relativeDirection;
	vec4 relativeUp;
	vec4 absoluteDirection;
	vec4 absoluteUp;
	vec4 shadowMapRect;
	uint lightStateId;
};