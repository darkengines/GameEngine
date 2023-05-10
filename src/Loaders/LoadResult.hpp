#pragma once

#include <vector>
#include <memory>
#include "../Meshes/MeshInfo.hpp"
#include "../Materials/Components/Material.hpp"
#include "../Textures/ImageInfo.hpp"
#include <entt/entt.hpp>
#include <unordered_map>

namespace drk::Loaders {
	struct LoadResult {
		LoadResult() = default;
		LoadResult(LoadResult&& x) = default;
		LoadResult(const LoadResult& x) = delete;
		LoadResult& operator=(const LoadResult& other) = delete;

		std::vector<std::unique_ptr<Meshes::MeshInfo>> meshes;
		std::vector<std::unique_ptr<Materials::Components::Material>> materials;
		std::vector<std::unique_ptr<Textures::ImageInfo>> images;
		std::unordered_map<uint32_t, entt::entity> materialIdEntityMap;
		std::unordered_map<uint32_t, entt::entity> meshIdEntityMap;
		entt::entity rootEntity = entt::null;
	};
}
