#pragma once

namespace drk::Textures {
	enum class TextureType {
		None = 0,
		BaseColor = 1 << 0,
		AmbientColor = 1 << 1,
		DiffuseColor = 1 << 2,
		SpecularColor = 1 << 3,
		EmissionColor = 1 << 4,
		NormalMap = 1 << 5,
		MetalnessMap = 1 << 6,
		RoughnessMap = 1 << 7,
		OpacityMap = 1 << 8,
		AlbedoColor = 1 << 9,
		AmbientOcclusionMap = 1 << 10,
		DiffuseRoughnessMap = 1 << 11,
		DisplacementMap = 1 << 12,
		EmissionMap = 1 << 13,
		LightMap = 1 << 14,
		HeightMap = 1 << 15,
		NormalCameraMap = 1 << 16,
		ReflectionMap = 1 << 17,
		ShininessMap = 1 << 18,
		RoughnessMetalnessMap = RoughnessMap | MetalnessMap
	};
}
