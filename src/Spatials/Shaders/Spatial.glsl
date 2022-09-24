struct Spatial {
	vec4 relativeScale;
	vec4 relativeRotation;
	vec4 relativePosition;
	vec4 absoluteScale;
	vec4 absoluteRotation;
	vec4 absolutePosition;
	mat4 relativeModel;
	mat4 absoluteModel;
	bool hasParentModel;
};