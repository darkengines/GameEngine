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
		AssimpLoader(Graphics::EngineState *const engineState);

		LoadResult Load(std::filesystem::path scenePath);
	protected:
		Graphics::EngineState *const EngineState;
		void loadMaterials(
			std::span<aiMaterial *> aiMaterials,
			std::span<aiTexture *> aiTextures,
			std::filesystem::path workingDirectoryPath,
			LoadResult& loadResult
		);
		void loadMeshes(
			std::span<aiMesh *> aiMeshes,
			LoadResult& loadResult
		);
		void loadLights(
			std::span<aiLight *> aiLights,
			std::unordered_map<std::string, std::tuple<entt::entity, aiLightSourceType>> &lightNameMap
		);
		void loadCameras(
			std::span<aiCamera *> aiCameras,
			std::unordered_map<std::string, entt::entity>& cameraNameMap
		);
		entt::entity loadNode(
			const aiNode *aiNode,
			const std::unordered_map<std::string, std::tuple<entt::entity, aiLightSourceType>> &lightMap,
			const std::unordered_map<std::string, entt::entity> &cameraMap,
			LoadResult& loadResult
		);

		static glm::vec3 &toVector(const aiVector3D &aiVector);
		static std::unordered_map<aiTextureType, Textures::TextureType> TextureTypeMap;
	};
}