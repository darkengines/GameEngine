#pragma once

#include <filesystem>
#include <unordered_map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <span>
#include <glm/vec3.hpp>
#include "LoadResult.hpp"
#include "../Graphics/EngineState.hpp"

namespace drk::Loaders {
	class AssimpLoader {
	public:
		AssimpLoader(entt::registry& registry, Graphics::EngineState& engineState);
		LoadResult Load(std::filesystem::path scenePath) const;
	protected:
		entt::registry& Registry;
		Graphics::EngineState& EngineState;
		void loadMaterials(
			std::span<aiMaterial *> aiMaterials,
			std::span<aiTexture *> aiTextures,
			std::filesystem::path workingDirectoryPath,
			LoadResult& loadResult
		) const;
		void loadMeshes(
			std::span<aiMesh *> aiMeshes,
			LoadResult& loadResult
		) const;
		void loadLights(
			std::span<aiLight *> aiLights,
			std::unordered_map<std::string, std::tuple<entt::entity, aiLightSourceType>> &lightNameMap
		) const;
		void loadCameras(
			std::span<aiCamera *> aiCameras,
			std::unordered_map<std::string, entt::entity>& cameraNameMap
		) const;
		entt::entity loadNode(
			const aiNode *aiNode,
			const std::unordered_map<std::string, std::tuple<entt::entity, aiLightSourceType>> &lightMap,
			const std::unordered_map<std::string, entt::entity> &cameraMap,
			LoadResult& loadResult
		) const;

		static glm::vec3 &toVector(const aiVector3D &aiVector);
		static std::unordered_map<aiTextureType, Textures::TextureType> TextureTypeMap;
	};
}