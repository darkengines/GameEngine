#pragma once

struct LightPerspective {
	mat4 perspective;
	mat4 view;
	vec4 relativeFront;
	vec4 relativeUp;
	vec4 absoluteFront;
	vec4 absoluteUp;

	float verticalFov;
	float aspectRatio;
	float near;
	float far;
};