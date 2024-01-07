#include "../../Planes/Shaders/Plane.glsl"

struct Frustum {
	Plane nearPlan;
	Plane farPlan;
	Plane leftPlan;
	Plane rightPlan;
	Plane topPlan;
	Plane bottomPlan;

	vec4 rightTopFar;
	vec4 leftTopFar;
	vec4 rightBottomFar;
	vec4 leftBottomFar;
	vec4 rightTopNear;
	vec4 leftTopNear;
	vec4 rightBottomNear;
	vec4 leftBottomNear;
};