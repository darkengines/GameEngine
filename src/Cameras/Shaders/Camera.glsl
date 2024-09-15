struct Camera {
    mat4 perspective;
    mat4 view;
	vec4 position;
	vec4 front;
	vec4 up;
	StoreItemLocation spatialItemLocation;
	StoreItemLocation relativeSpatialItemLocation;
    float verticalFov;
    float aspectRatio;
    float near;
    float far;
};