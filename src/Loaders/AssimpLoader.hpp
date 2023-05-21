#pragma once

#include <filesystem>
#include <unordered_map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <span>
#include <glm/vec3.hpp>
#include "LoadResult.hpp"
#include "../Engine/EngineState.hpp"

namespace drk::Loaders {
	class AssimpLoader {
	public:
		AssimpLoader();
		LoadResult Load(std::filesystem::path scenePath, entt::registry& registry) const;
	protected:
		void loadMaterials(
			std::span<aiMaterial*> aiMaterials,
			std::span<aiTexture*> aiTextures,
			std::filesystem::path workingDirectoryPath,
			LoadResult& loadResult,
			entt::registry& registry
		) const;
		void loadMeshes(
			std::span<aiMesh*> aiMeshes,
			LoadResult& loadResult,
			entt::registry& registry
		) const;
		void loadLights(
			std::span<aiLight*> aiLights,
			std::unordered_map<std::string, std::tuple<entt::entity, aiLightSourceType>>& lightNameMap,
			entt::registry& registry
		) const;
		void loadCameras(
			std::span<aiCamera*> aiCameras,
			std::unordered_map<std::string, entt::entity>& cameraNameMap,
			entt::registry& registry
		) const;
		entt::entity loadNode(
			const aiNode* aiNode,
			const std::unordered_map<std::string, std::tuple<entt::entity, aiLightSourceType>>& lightMap,
			const std::unordered_map<std::string, entt::entity>& cameraMap,
			LoadResult& loadResult,
			entt::registry& registry,
			int depth = 0
		) const;

		static glm::vec3& toVector(const aiVector3D& aiVector);
		static std::unordered_map<aiTextureType, Textures::TextureType> TextureTypeMap;
	};
}