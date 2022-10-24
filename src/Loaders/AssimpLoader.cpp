#include "AssimpLoader.hpp"
#include "../Lights/PointLight.hpp"
#include "../Geometries/AxisAlignedBoundingBox.hpp"
#include "../Lights/DirectionalLight.hpp"
#include "../Lights/Spotlight.hpp"
#include "../Lights/Light.hpp"
#include "../Cameras/Camera.hpp"
#include "../Spatials/Spatial.hpp"
#include "../Meshes/MeshGroup.hpp"
#include "../Objects/Relationship.hpp"
#include "../Objects/Object.hpp"
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/quaternion.hpp>

namespace drk::Loaders {
	AssimpLoader::AssimpLoader(Graphics::EngineState* const engineState) : EngineState(engineState) {}

	LoadResult AssimpLoader::Load(std::filesystem::path scenePath) {
		Assimp::Importer importer;
		auto aiScene = importer.ReadFile(
			scenePath.string(),
			aiProcess_FindInstances
			| aiProcess_GenBoundingBoxes
			| aiProcess_Triangulate
		);

		std::span<aiMaterial*> aiMaterials(aiScene->mMaterials, aiScene->mNumMaterials);
		std::span<aiTexture*> aiTextures(aiScene->mTextures, aiScene->mNumTextures);
		std::span<aiMesh*> aiMeshes(aiScene->mMeshes, aiScene->mNumMeshes);
		std::span<aiLight*> aiLights(aiScene->mLights, aiScene->mNumLights);
		std::span<aiCamera*> aiCameras(aiScene->mCameras, aiScene->mNumCameras);

		LoadResult loadResult;
		loadResult.meshes.resize(aiMeshes.size());
		loadResult.materials.resize(aiMaterials.size());

		const auto workingDirectoryPath = scenePath.parent_path();
		loadMaterials(aiMaterials, aiTextures, workingDirectoryPath, loadResult);
		loadMeshes(aiMeshes, loadResult);

		std::unordered_map<std::string, std::tuple<entt::entity, aiLightSourceType>> lightMap;
		std::unordered_map<std::string, entt::entity> cameraMap;

		loadLights(aiLights, lightMap);
		loadCameras(aiCameras, cameraMap);

		auto rootEntity = loadNode(aiScene->mRootNode, lightMap, cameraMap, loadResult);


		return loadResult;
	}

	void AssimpLoader::loadMaterials(
		std::span<aiMaterial*> aiMaterials,
		std::span<aiTexture*> aiTextures,
		std::filesystem::path workingDirectoryPath,
		LoadResult& loadResult
	) {
		auto aiTextureIndex = 0u;
		for (auto aiMaterialIndex = 0u; aiMaterialIndex < aiMaterials.size(); aiMaterialIndex++) {
			auto aiMaterial = aiMaterials[aiMaterialIndex];
			std::unordered_map<std::string, entt::entity> textureNameMap;
			std::unordered_map<Textures::TextureType, entt::entity> textureTypeMap;
			for (auto textureTypePair : TextureTypeMap) {
				if (aiMaterial->GetTextureCount(textureTypePair.first)) {
					aiString aiTexturePath;
					auto result = aiMaterial->GetTexture(textureTypePair.first, 0, &aiTexturePath);
					if (result == aiReturn::aiReturn_SUCCESS) {
						auto texturePath = aiTexturePath.C_Str();
						auto texturePathPair = textureNameMap.find(texturePath);
						std::unique_ptr<Textures::ImageInfo> image;
						if (texturePathPair == textureNameMap.end()) {
							if (texturePath[0] == '*') {
								int textureIndex = 0;
								sscanf_s(texturePath, "*%d", &textureIndex);
								auto aiTexture = aiTextures[textureIndex];
								std::span<unsigned char> data((unsigned char*) aiTexture->pcData, aiTexture->mWidth);
								image = Textures::ImageInfo::fromMemory(
									std::string(texturePath),
									data,
									textureTypePair.second
								);
							} else {
								auto textureFileSystemPath = workingDirectoryPath.make_preferred() / std::filesystem::path(texturePath).make_preferred().relative_path();
								image = Textures::ImageInfo::fromFile(
									textureFileSystemPath.string(),
									textureFileSystemPath.string(),
									textureTypePair.second
								);
							}
							auto textureEntity = EngineState->Registry.create();
							EngineState->Registry.emplace<Textures::ImageInfo*>(textureEntity, image.get());
							auto textureIndex = EngineState->IndexGenerator.Generate<Textures::ImageInfo>();
							EngineState->Registry.emplace<Common::ComponentIndex<Textures::ImageInfo>>(
								textureEntity,
								textureIndex
							);
							textureNameMap[texturePath] = textureEntity;
							textureTypeMap[textureTypePair.second] = textureEntity;
							loadResult.images.push_back(std::move(image));
							aiTextureIndex++;
						} else {
							textureTypeMap[textureTypePair.second] = texturePathPair->second;
						}
					}
				}
			}
			std::string materialName = aiMaterial->GetName().C_Str();

			const auto& baseColorTexturePair = textureTypeMap.find(Textures::TextureType::BaseColor);
			const auto& ambientColorTexturePair = textureTypeMap.find(Textures::TextureType::AmbientColor);
			const auto& diffuseColorTexturePair = textureTypeMap.find(Textures::TextureType::DiffuseColor);
			const auto& specularColorTexturePair = textureTypeMap.find(Textures::TextureType::SpecularColor);
			const auto& normalMapPair = textureTypeMap.find(Textures::TextureType::NormalMap);
			const auto& metallicRoughnessPair = textureTypeMap.find(Textures::TextureType::RoughnessMetalnessMap);

			entt::entity baseColorTexture =
				baseColorTexturePair != textureTypeMap.end()
				? baseColorTexturePair->second
				: entt::null;
			entt::entity ambientColorTexture =
				ambientColorTexturePair != textureTypeMap.end()
				? ambientColorTexturePair->second
				: entt::null;
			entt::entity diffuseColorTexture =
				diffuseColorTexturePair != textureTypeMap.end()
				? diffuseColorTexturePair->second
				: entt::null;
			entt::entity specularColorTexture =
				specularColorTexturePair != textureTypeMap.end()
				? specularColorTexturePair->second : entt::null;
			entt::entity normalMap =
				normalMapPair != textureTypeMap.end()
				? normalMapPair->second
				: entt::null;
			entt::entity metallicRoughnessTexture =
				metallicRoughnessPair != textureTypeMap.end()
				? metallicRoughnessPair->second
				: entt::null;

			aiColor4D ambientColor, diffuseColor, specularColor;
			auto hasAmbientColor = aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_AMBIENT, &ambientColor) == AI_SUCCESS;
			auto hasDiffuseColor = aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_DIFFUSE, &diffuseColor) == AI_SUCCESS;
			auto hasSpecularColor =
				aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_SPECULAR, &specularColor) == AI_SUCCESS;

			auto materialIndex = EngineState->IndexGenerator.Generate<Materials::Material>();
			auto materialEntity = EngineState->Registry.create();

			auto hasTransparency =
				baseColorTexture != entt::null &&
				EngineState->Registry.get<Textures::ImageInfo*>(baseColorTexture)->depth > 3
				|| ambientColorTexture != entt::null &&
				   EngineState->Registry.get<Textures::ImageInfo*>(ambientColorTexture)->depth > 3
				|| diffuseColorTexture != entt::null &&
				   EngineState->Registry.get<Textures::ImageInfo*>(diffuseColorTexture)->depth > 3
				|| hasAmbientColor && ambientColor.a < 1
				|| hasDiffuseColor && diffuseColor.a < 1;

			Materials::Material material = {
				.name = materialName,
				.baseColor = glm::vec4(ambientColor.r, ambientColor.g, ambientColor.b, ambientColor.a),
				.ambientColor = glm::vec4(ambientColor.r, ambientColor.g, ambientColor.b, ambientColor.a),
				.diffuseColor =  glm::vec4(diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a),
				.specularColor = glm::vec4(specularColor.r, specularColor.g, specularColor.b, specularColor.a),
				.baseColorTexture = baseColorTexture,
				.ambientColorTexture = ambientColorTexture,
				.diffuseColorTexture = diffuseColorTexture,
				.specularColorTexture = specularColorTexture,
				.normalMap = normalMap,
				.metallicRoughnessTexture = metallicRoughnessTexture,
				.hasTransparency = hasTransparency
			};

			auto materialPtr = std::make_unique<Materials::Material>(material);
			EngineState->Registry.emplace<Materials::Material*>(materialEntity, materialPtr.get());
			EngineState->Registry.emplace<Common::ComponentIndex<Materials::Material >>(materialEntity, materialIndex);
			loadResult.materials[aiMaterialIndex] = std::move(materialPtr);
			loadResult.materialIdEntityMap[aiMaterialIndex] = materialEntity;
		}
	}

	void AssimpLoader::loadMeshes(std::span<aiMesh*> aiMeshes, LoadResult& loadResult) {
		for (auto aiMeshIndex = 0u; aiMeshIndex < aiMeshes.size(); aiMeshIndex++) {
			const auto& aiMesh = aiMeshes[aiMeshIndex];
			std::vector<uint32_t> indices(aiMesh->mNumFaces * 3);
			for (uint32_t faceIndex = 0u; faceIndex < aiMesh->mNumFaces; faceIndex++) {
				memcpy(
					indices.data() + faceIndex * 3,
					aiMesh->mFaces[faceIndex].mIndices,
					aiMesh->mFaces->mNumIndices * sizeof(uint32_t));
			}
			std::vector<Meshes::Vertex> vertices(aiMesh->mNumVertices);
			for (uint32_t vertexIndex = 0; vertexIndex < aiMesh->mNumVertices; vertexIndex++) {
				glm::vec4 position{
					aiMesh->mVertices[vertexIndex].x,
					aiMesh->mVertices[vertexIndex].y,
					aiMesh->mVertices[vertexIndex].z,
					1
				};
				glm::vec4 normal{
					aiMesh->mNormals[vertexIndex].x,
					aiMesh->mNormals[vertexIndex].y,
					aiMesh->mNormals[vertexIndex].z,
					0
				};
				glm::vec4 tangent = aiMesh->mTangents != nullptr ? glm::vec4{
					aiMesh->mTangents[vertexIndex].x,
					aiMesh->mTangents[vertexIndex].y,
					aiMesh->mTangents[vertexIndex].z,
					0
				} : glm::vec4{0, 0, 0, 0};
				glm::vec4 bitangent = aiMesh->mBitangents != nullptr ? glm::vec4{
					aiMesh->mBitangents[vertexIndex].x,
					aiMesh->mBitangents[vertexIndex].y,
					aiMesh->mBitangents[vertexIndex].z,
					0
				} : glm::vec4{0, 0, 0, 0};
				glm::vec4 diffuseColor;
				glm::vec2 texel;
				if (aiMesh->HasVertexColors(0)) {
					diffuseColor.r = aiMesh->mColors[0][vertexIndex].r;
					diffuseColor.g = aiMesh->mColors[0][vertexIndex].g;
					diffuseColor.b = aiMesh->mColors[0][vertexIndex].b;
					diffuseColor.a = aiMesh->mColors[0][vertexIndex].a;
				} else {
					diffuseColor = {1.0, 1.0, 1.0, 1.0};
				}

				if (aiMesh->HasTextureCoords(0)) {
					texel.x = aiMesh->mTextureCoords[0][vertexIndex].x;
					texel.y = 1 - aiMesh->mTextureCoords[0][vertexIndex].y;
				} else {
					texel = {0.0, 0.0};
				}
				vertices[vertexIndex] = {
					position,
					normal,
					tangent,
					bitangent,
					diffuseColor,
					texel
				};
			}
			std::string meshName = aiMesh->mName.C_Str();
			auto meshIndex = EngineState->IndexGenerator.Generate<Meshes::MeshInfo>();
			auto meshMaterial = loadResult.materials[aiMesh->mMaterialIndex].get();
			auto materialEntity = loadResult.materialIdEntityMap[aiMesh->mMaterialIndex];
			Meshes::MeshInfo mesh = {
				.name = meshName,
				.vertices =vertices,
				.indices = indices,
				.pMaterial = meshMaterial,
				.materialEntity = materialEntity
			};
			auto meshPtr = std::make_unique<Meshes::MeshInfo>(mesh);
			Geometries::AxisAlignedBoundingBox axisAlignedBoundingBox = Geometries::AxisAlignedBoundingBox::fromMinMax(
				glm::vec4(AssimpLoader::toVector(aiMesh->mAABB.mMin), 1.0f),
				glm::vec4(AssimpLoader::toVector(aiMesh->mAABB.mMax), 1.0f)
			);

			auto meshEntity = EngineState->Registry.create();
			EngineState->Registry.emplace<Meshes::MeshInfo*>(meshEntity, meshPtr.get());
			EngineState->Registry.emplace<Common::ComponentIndex<Meshes::MeshInfo>>(meshEntity, meshIndex);
			EngineState->Registry.emplace<Geometries::AxisAlignedBoundingBox>(meshEntity, axisAlignedBoundingBox);

			loadResult.meshIdEntityMap[aiMeshIndex] = meshEntity;
			loadResult.meshes[aiMeshIndex] = std::move(meshPtr);
		}
	}

	void AssimpLoader::loadLights(
		std::span<aiLight*> aiLights,
		std::unordered_map<std::string, std::tuple<entt::entity, aiLightSourceType>>& lightNameMap
	) {
		for (auto aiLight : aiLights) {
			auto entity = EngineState->Registry.create();
			auto lightName = std::string(aiLight->mName.C_Str());
			if (aiLight->mType == aiLightSourceType::aiLightSource_POINT) {
				auto pointLightIndex = EngineState->IndexGenerator.Generate<Lights::PointLight>();
				Lights::PointLight pointLight;
				pointLight.constantAttenuation = aiLight->mAttenuationConstant;
				pointLight.linearAttenuation = aiLight->mAttenuationLinear;
				pointLight.quadraticAttenuation = aiLight->mAttenuationQuadratic;
				pointLight.relativePosition = {aiLight->mPosition.x, aiLight->mPosition.y, aiLight->mPosition.z, 1};

				EngineState->Registry.emplace<Lights::PointLight>(entity, pointLight);
				EngineState->Registry.emplace<Common::ComponentIndex<Lights::PointLight>>(entity, pointLightIndex);
			}
			if (aiLight->mType == aiLightSourceType::aiLightSource_DIRECTIONAL) {
				auto directionalLightIndex = EngineState->IndexGenerator.Generate<Lights::DirectionalLight>();
				Lights::DirectionalLight directionalLight;
				directionalLight.relativeDirection = {
					aiLight->mDirection.x,
					aiLight->mDirection.y,
					aiLight->mDirection.z,
					0
				};
				directionalLight.relativeUp = {aiLight->mUp.x, aiLight->mUp.y, aiLight->mUp.z, 0};

				EngineState->Registry.emplace<Lights::DirectionalLight>(entity, directionalLight);
				EngineState->Registry.emplace<Common::ComponentIndex<Lights::DirectionalLight >>(
					entity,
					directionalLightIndex
				);
			}
			if (aiLight->mType == aiLightSourceType::aiLightSource_SPOT) {
				auto spotlightIndex = EngineState->IndexGenerator.Generate<Lights::Spotlight>();
				Lights::Spotlight spotlight;
				spotlight.constantAttenuation = aiLight->mAttenuationConstant;
				spotlight.linearAttenuation = aiLight->mAttenuationLinear;
				spotlight.quadraticAttenuation = aiLight->mAttenuationQuadratic;
				spotlight.relativePosition = {aiLight->mPosition.x, aiLight->mPosition.y, aiLight->mPosition.z, 1};
				spotlight.relativeDirection = {aiLight->mDirection.x, aiLight->mDirection.y, aiLight->mDirection.z, 0};
				spotlight.relativeUp = {aiLight->mUp.x, aiLight->mUp.y, aiLight->mUp.z, 0};
				spotlight.innerConeAngle = aiLight->mAngleInnerCone;
				spotlight.outerConeAngle = aiLight->mAngleOuterCone;

				EngineState->Registry.emplace<Lights::Spotlight>(entity, spotlight);
				EngineState->Registry.emplace<Common::ComponentIndex<Lights::Spotlight >>(entity, spotlightIndex);
			}
			auto lightIndex = EngineState->IndexGenerator.Generate<Lights::Light>();
			Lights::Light light = {
				{aiLight->mColorAmbient.r,  aiLight->mColorAmbient.g,  aiLight->mColorAmbient.b,  1},
				{aiLight->mColorDiffuse.r,  aiLight->mColorDiffuse.g,  aiLight->mColorDiffuse.b,  1},
				{aiLight->mColorSpecular.r, aiLight->mColorSpecular.g, aiLight->mColorSpecular.b, 1}
			};
			EngineState->Registry.emplace<Lights::Light>(entity, light);
			EngineState->Registry.emplace<Common::ComponentIndex<Lights::Light >>(entity, lightIndex);
			lightNameMap[lightName] = {entity, aiLight->mType};
		}
	}

	void AssimpLoader::loadCameras(
		std::span<aiCamera*> aiCameras,
		std::unordered_map<std::string, entt::entity>& cameraNameMap
	) {
		for (auto aiCamera : aiCameras) {
			auto cameraName = std::string(aiCamera->mName.C_Str());
			auto relativePosition = glm::vec4{
				aiCamera->mPosition.x,
				aiCamera->mPosition.y,
				aiCamera->mPosition.z,
				1.0f
			};
			auto relativeFront = glm::vec4{aiCamera->mLookAt.x, aiCamera->mLookAt.y, aiCamera->mLookAt.z, 0.0f};
			auto relativeUp = glm::vec4{aiCamera->mUp.x, aiCamera->mUp.y, aiCamera->mUp.z, 0.0f};
			auto cameraIndex = EngineState->IndexGenerator.Generate<Cameras::Camera>();
			auto perspective = glm::perspectiveZO(
				aiCamera->mHorizontalFOV,
				aiCamera->mAspect,
				aiCamera->mClipPlaneNear,
				aiCamera->mClipPlaneFar
			);
			perspective[1][1] *= -1.0f;
			Cameras::Camera camera{
				perspective,
				glm::lookAt(
					glm::make_vec3(relativePosition),
					glm::make_vec3(relativePosition + relativeFront),
					glm::make_vec3(relativeUp)),
				relativePosition,
				relativeFront,
				relativeUp,
				relativePosition,
				relativeFront,
				relativeUp,
				aiCamera->mHorizontalFOV,
				aiCamera->mAspect,
				aiCamera->mClipPlaneNear,
				aiCamera->mClipPlaneFar,
			};

			auto entity = EngineState->Registry.create();
			EngineState->Registry.emplace<Cameras::Camera>(entity, camera);
			EngineState->Registry.emplace<Common::ComponentIndex<Cameras::Camera>>(entity, cameraIndex);
			cameraNameMap[cameraName] = entity;
		}
	}

	entt::entity AssimpLoader::loadNode(
		const aiNode* aiNode,
		const std::unordered_map<std::string, std::tuple<entt::entity, aiLightSourceType>>& lightMap,
		const std::unordered_map<std::string, entt::entity>& cameraMap,
		LoadResult& loadResult
	) {
		auto nodeName = std::string(aiNode->mName.C_Str());
		aiVector3D aiScale, aiPosition;
		aiQuaternion aiRotation;
		aiNode->mTransformation.Decompose(aiScale, aiRotation, aiPosition);
		glm::vec4 scale(aiScale.x, aiScale.y, aiScale.z, 1), position(aiPosition.x, aiPosition.y, aiPosition.z, 1);
		glm::quat rotation(aiRotation.w, aiRotation.x, aiRotation.y, aiRotation.z);
		auto translationMatrix = glm::translate(glm::identity<glm::mat4>(), glm::vec3(position));
		auto rotationMatrix = glm::toMat4(rotation);
		auto scalingMatrix = glm::scale(glm::identity<glm::mat4>(), glm::vec3(scale));
		auto localModel = translationMatrix * rotationMatrix * scalingMatrix;

		Spatials::Spatial spatial{
			scale,
			rotation,
			position,
			scale,
			rotation,
			position,
			localModel,
			localModel,
		};

		entt::entity entity = entt::null;

		auto light = lightMap.find(nodeName);
		if (light != lightMap.end()) {
			entity = std::get<0>(light->second);
			auto lightType = std::get<1>(light->second);
			if (lightType == aiLightSourceType::aiLightSource_DIRECTIONAL) {
				auto& directionalLight = EngineState->Registry.get<Lights::DirectionalLight>(entity);
				spatial.relativeRotation = spatial.relativeRotation * glm::quatLookAt(
					glm::vec3(directionalLight.relativeDirection),
					glm::vec3(directionalLight.relativeUp)
				);
			} else if (lightType == aiLightSourceType::aiLightSource_POINT) {
				auto& pointLight = EngineState->Registry.get<Lights::PointLight>(entity);
				spatial.relativePosition = spatial.relativePosition + pointLight.relativePosition;
			} else if (lightType == aiLightSourceType::aiLightSource_SPOT) {
				auto& spotlight = EngineState->Registry.get<Lights::Spotlight>(entity);
				spatial.relativeRotation = spatial.relativeRotation * glm::quatLookAt(
					glm::vec3(spotlight.relativeDirection),
					glm::vec3(spotlight.relativeUp)
				);
				spatial.relativePosition = spatial.relativePosition + spotlight.relativePosition;
			}
		}
		auto cameraEntity = cameraMap.find(nodeName);
		if (cameraEntity != cameraMap.end()) {
			entity = cameraEntity->second;
		}

		if (entity == entt::null) entity = EngineState->Registry.create();
		EngineState->Registry.emplace<Objects::Object>(entity, aiNode->mName.C_Str());

		if (aiNode->mNumMeshes) {
			Meshes::MeshGroup meshGroup;
			for (auto meshIndex = 0u; meshIndex < aiNode->mNumMeshes; meshIndex++) {
				auto meshEntity = loadResult.meshIdEntityMap[aiNode->mMeshes[meshIndex]];
				meshGroup.meshEntities.push_back(meshEntity);
			}
			EngineState->Registry.emplace<Meshes::MeshGroup>(entity, meshGroup);
		}

		EngineState->Registry.emplace<Spatials::Spatial>(entity, spatial);

		Objects::Relationship relationship;
		Objects::Relationship* previousRelationship = nullptr;
		entt::entity previousSibling{entt::null};

		if (aiNode->mNumChildren) {
			for (auto childIndex = 0u; childIndex < aiNode->mNumChildren; childIndex++) {
				auto aiChildNode = aiNode->mChildren[childIndex];
				auto childEntity = loadNode(
					aiChildNode,
					lightMap,
					cameraMap,
					loadResult
				);
				auto& childRelationship = EngineState->Registry.get<Objects::Relationship>(childEntity);
				if (childIndex == 0) relationship.firstChild = childEntity;
				if (childIndex > 0) {
					childRelationship.previousSibling = previousSibling;
					previousRelationship->nextSibling = childEntity;
				}
				childRelationship.parent = entity;
				previousSibling = childEntity;
				previousRelationship = &childRelationship;
			}
		}

		EngineState->Registry.emplace<Objects::Relationship>(entity, relationship);

		return entity;
	}

	std::unordered_map<aiTextureType, Textures::TextureType> AssimpLoader::TextureTypeMap = {
		{aiTextureType::aiTextureType_AMBIENT,           Textures::TextureType::AmbientColor},
		{aiTextureType::aiTextureType_AMBIENT_OCCLUSION, Textures::TextureType::AmbientOcclusionMap},
		{aiTextureType::aiTextureType_BASE_COLOR,        Textures::TextureType::BaseColor},
		{aiTextureType::aiTextureType_DIFFUSE,           Textures::TextureType::DiffuseColor},
		{aiTextureType::aiTextureType_DIFFUSE_ROUGHNESS, Textures::TextureType::DiffuseRoughnessMap},
		{aiTextureType::aiTextureType_DISPLACEMENT,      Textures::TextureType::DisplacementMap},
		{aiTextureType::aiTextureType_EMISSION_COLOR,    Textures::TextureType::EmissionColor},
		{aiTextureType::aiTextureType_EMISSIVE,          Textures::TextureType::EmissionMap},
		{aiTextureType::aiTextureType_HEIGHT,            Textures::TextureType::HeightMap},
		{aiTextureType::aiTextureType_LIGHTMAP,          Textures::TextureType::LightMap},
		{aiTextureType::aiTextureType_METALNESS,         Textures::TextureType::MetalnessMap},
		{aiTextureType::aiTextureType_NONE,              Textures::TextureType::None},
		{aiTextureType::aiTextureType_NORMALS,           Textures::TextureType::NormalMap},
		{aiTextureType::aiTextureType_NORMAL_CAMERA,     Textures::TextureType::NormalCameraMap},
		{aiTextureType::aiTextureType_OPACITY,           Textures::TextureType::OpacityMap},
		{aiTextureType::aiTextureType_REFLECTION,        Textures::TextureType::ReflectionMap},
		{aiTextureType::aiTextureType_SHININESS,         Textures::TextureType::ShininessMap},
		{aiTextureType::aiTextureType_SPECULAR,          Textures::TextureType::SpecularColor},
		{aiTextureType::aiTextureType_UNKNOWN,           Textures::TextureType::RoughnessMetalnessMap}
	};

	glm::vec3& AssimpLoader::toVector(const aiVector3D& aiVector) { return (*(glm::vec3*) &aiVector); }
}
                                                                                                                                                                                                                                                                                                                