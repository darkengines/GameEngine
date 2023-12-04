#pragma once

#include <vector>
#include <memory>
#include "../Meshes/Components/MeshResource.hpp"
#include "../Materials/Components/Material.hpp"
#include "../Textures/ImageInfo.hpp"
#include "../Lights/Components/PointLight.hpp"
#include "../Lights/Components/DirectionalLight.hpp"
#include "../Lights/Components/Spotlight.hpp"
#include <entt/entt.hpp>
#include <unordered_map>

namespace drk::Loaders {
	struct LoadResult {
		LoadResult() = default;
		LoadResult(LoadResult&& x) = default;
		LoadResult(const LoadResult& x) = delete;
		LoadResult& operator=(const LoadResult& other) = delete;

		std::vector<std::shared_ptr<Meshes::Components::MeshResource>> meshes;
		std::vector<std::shared_ptr<Materials::Components::Material>> materials;
		std::vector<std::shared_ptr<Textures::ImageInfo>> images;
		std::vector<Lights::Components::PointLight> pointLights;
		std::vector<Lights::Components::DirectionalLight> directionalLights;
		std::vector<Lights::Components::Spotlight> spotlights;
		std::unordered_map<uint32_t, entt::entity> materialIdEntityMap;
		std::unordered_map<uint32_t, entt::entity> meshIdEntityMap;
		entt::entity rootEntity = entt::null;
	};
}
