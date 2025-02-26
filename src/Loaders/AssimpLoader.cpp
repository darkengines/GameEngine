#include "AssimpLoader.hpp"

#include <assimp/postprocess.h>
#include <filesystem>

#include <algorithm>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "../Animations/Components/Animation.hpp"
#include "../Animations/Components/AnimationState.hpp"
#include "../Animations/Components/Bone.hpp"
#include "../Animations/Components/BoneCollection.hpp"
#include "../Animations/Components/BoneMesh.hpp"
#include "../Animations/Components/MeshBoneCollection.hpp"
#include "../Animations/Components/NodeBoneMesh.hpp"
#include "../Animations/Components/RootBoneInstanceReference.hpp"
#include "../Animations/Components/Skinned.hpp"
#include "../Animations/Components/SkinnedMeshInstance.hpp"
#include "../Animations/Components/VertexWeightInstance.hpp"
#include "../BoundingVolumes/Components/AxisAlignedBoundingBox.hpp"
#include "../Cameras/Components/Camera.hpp"
#include "../Common/Components/Name.hpp"
#include "../GlmExtensions.hpp"
#include "../Lights/Components/DirectionalLight.hpp"
#include "../Lights/Components/Light.hpp"
#include "../Lights/Components/LightPerspective.hpp"
#include "../Lights/Components/LightPerspectiveCollection.hpp"
#include "../Lights/Components/PointLight.hpp"
#include "../Lights/Components/Spotlight.hpp"
#include "../Materials/Components/MaterialReference.hpp"
#include "../Meshes/Components/Mesh.hpp"
#include "../Nodes/Components/Node.hpp"
#include "../Nodes/Components/NodeMesh.hpp"
#include "../Nodes/Components/NodeMeshCollection.hpp"
#include "../Spatials/Components/Spatial.hpp"

namespace drk::Loaders {

struct PostProcessed {};

AssimpLoader::AssimpLoader() {}
LoadResult AssimpLoader::Load(std::filesystem::path scenePath, entt::registry& registry) const {
	Assimp::Importer importer;
	auto aiScene = importer.ReadFile(
		scenePath.string(),
		aiProcess_FindInstances | aiProcess_GenBoundingBoxes | aiProcess_Triangulate | aiProcess_FindInstances | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices |
			aiProcess_CalcTangentSpace | aiProcess_OptimizeMeshes | aiProcess_PopulateArmatureData
	);
	std::span<aiMaterial*> aiMaterials(aiScene->mMaterials, aiScene->mNumMaterials);
	std::span<aiTexture*> aiTextures(aiScene->mTextures, aiScene->mNumTextures);
	std::span<aiMesh*> aiMeshes(aiScene->mMeshes, aiScene->mNumMeshes);
	std::span<aiLight*> aiLights(aiScene->mLights, aiScene->mNumLights);
	std::span<aiCamera*> aiCameras(aiScene->mCameras, aiScene->mNumCameras);
	std::span<aiAnimation*> aiAnimations(aiScene->mAnimations, aiScene->mNumAnimations);
	std::span<aiSkeleton*> aiSkeletons(aiScene->mSkeletons, aiScene->mNumSkeletons);

	LoadResult loadResult;

	const auto workingDirectoryPath = scenePath.parent_path();
	loadMaterials(aiMaterials, aiTextures, workingDirectoryPath, loadResult, registry);

	std::unordered_map<std::string, entt::entity> aiBoneNodeNameBoneEntityMap;
	std::unordered_map<aiBone*, entt::entity> aiBonePtrBoneEntityMap;

	loadMeshes(aiMeshes, loadResult, registry, aiBoneNodeNameBoneEntityMap, aiBonePtrBoneEntityMap);

	std::unordered_map<std::string, std::tuple<entt::entity, aiLightSourceType>> lightMap;
	std::unordered_map<std::string, entt::entity> cameraMap;
	std::unordered_map<std::string, entt::entity> aiNodeNameNodeAnimationMap;
	std::unordered_map<entt::entity, entt::entity> map;
	std::stack<std::unordered_map<entt::entity, entt::entity>> meshEntityInstanceEntityMap;
	meshEntityInstanceEntityMap.push(map);

	loadLights(aiLights, lightMap, registry, loadResult);
	loadCameras(aiCameras, cameraMap, registry);
	loadSkeletons(aiSkeletons, loadResult, registry);
	loadAnimations(aiAnimations, aiNodeNameNodeAnimationMap, loadResult, registry);

	std::unordered_map<const aiNode*, entt::entity> cache;

	auto rootEntity = loadNode(
		aiScene->mRootNode, lightMap, cameraMap, aiBoneNodeNameBoneEntityMap, aiNodeNameNodeAnimationMap, meshEntityInstanceEntityMap, loadResult, registry, cache, entt::null
	);

	postProcessSkinnedMeshes(registry);

	return loadResult;
}

std::string replaceAll(std::string str, const std::string& from, const std::string& to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();  // Handles case where 'to' is a substring of 'from'
	}
	return str;
}

void AssimpLoader::loadMaterials(
	std::span<aiMaterial*> aiMaterials,
	std::span<aiTexture*> aiTextures,
	std::filesystem::path workingDirectoryPath,
	LoadResult& loadResult,
	entt::registry& registry
) const {
	auto aiTextureIndex = 0u;
	for (auto aiMaterialIndex = 0u; aiMaterialIndex < aiMaterials.size(); aiMaterialIndex++) {
		auto aiMaterial = aiMaterials[aiMaterialIndex];
		std::unordered_map<std::string, entt::entity> textureNameMap;
		std::unordered_map<Textures::TextureType, entt::entity> textureTypeMap;
		for (auto textureTypePair : TextureTypeMap) {
			auto textureCount = aiMaterial->GetTextureCount(textureTypePair.first);
			if (textureCount) {
				aiString aiTexturePath;
				auto result = aiMaterial->GetTexture(textureTypePair.first, 0, &aiTexturePath);
				if (result == aiReturn::aiReturn_SUCCESS) {
					std::string texturePath(aiTexturePath.C_Str());
					texturePath = replaceAll(texturePath, "%20", " ");
					auto texturePathPair = textureNameMap.find(texturePath);
					Textures::ImageInfo image;
					if (texturePathPair == textureNameMap.end()) {
						if (texturePath[0] == '*') {
							int textureIndex = 0;
							sscanf_s(texturePath.c_str(), "*%d", &textureIndex);
							auto aiTexture = aiTextures[textureIndex];
							std::span<unsigned char> data((unsigned char*)aiTexture->pcData, aiTexture->mWidth);
							image = std::move(Textures::ImageInfo::fromMemory(std::string(texturePath), data, textureTypePair.second));
						} else {
							auto textureFileSystemPath = workingDirectoryPath.make_preferred() / std::filesystem::path(texturePath).make_preferred().relative_path();
							image = std::move(Textures::ImageInfo::fromFile(textureFileSystemPath.string(), textureFileSystemPath.string(), textureTypePair.second));
						}
						if (image.pixels.size() > 0) {
							auto textureEntity = registry.create();
							registry.emplace<Common::Components::Name>(textureEntity, std::filesystem::path(texturePath).filename().string());
							registry.emplace<Textures::ImageInfo>(textureEntity, std::move(image));
							textureNameMap[texturePath] = textureEntity;
							textureTypeMap[textureTypePair.second] = textureEntity;
						}
						aiTextureIndex++;
					} else {
						textureTypeMap[textureTypePair.second] = texturePathPair->second;
					}
				} 
			} else {
				auto regexEntry = TextureTypeRegexMap.find(textureTypePair.second);
				if (regexEntry != TextureTypeRegexMap.end()) {
					for (const auto& entry : std::filesystem::directory_iterator(workingDirectoryPath)) {
						auto ok = std::regex_search(entry.path().filename().string(), regexEntry->second);
						if (ok) {
							auto image = std::move(Textures::ImageInfo::fromFile(entry.path().filename().string(), entry.path().string(), textureTypePair.second));
							if (image.pixels.size() > 0) {
								auto textureEntity = registry.create();
								registry.emplace<Common::Components::Name>(textureEntity, entry.path().filename().string());
								registry.emplace<Textures::ImageInfo>(textureEntity, std::move(image));
								textureNameMap[entry.path().string()] = textureEntity;
								textureTypeMap[textureTypePair.second] = textureEntity;
							}
						}
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
		const auto& roughnessPair = textureTypeMap.find(Textures::TextureType::RoughnessMap);
		const auto& metallicPair = textureTypeMap.find(Textures::TextureType::MetalnessMap);

		entt::entity baseColorTexture = baseColorTexturePair != textureTypeMap.end() ? baseColorTexturePair->second : entt::null;
		entt::entity ambientColorTexture = ambientColorTexturePair != textureTypeMap.end() ? ambientColorTexturePair->second : entt::null;
		entt::entity diffuseColorTexture = diffuseColorTexturePair != textureTypeMap.end() ? diffuseColorTexturePair->second : entt::null;
		entt::entity specularColorTexture = specularColorTexturePair != textureTypeMap.end() ? specularColorTexturePair->second : entt::null;
		entt::entity normalMap = normalMapPair != textureTypeMap.end() ? normalMapPair->second : entt::null;
		entt::entity metallicRoughnessTexture = metallicRoughnessPair != textureTypeMap.end() ? metallicRoughnessPair->second : entt::null;
		entt::entity metallicTexture = metallicPair != textureTypeMap.end() ? metallicPair->second : entt::null;
		entt::entity roughnessTexture = roughnessPair != textureTypeMap.end() ? roughnessPair->second : entt::null;

		aiColor4D ambientColor, diffuseColor, specularColor;
		auto hasAmbientColor = aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_AMBIENT, &ambientColor) == AI_SUCCESS;
		auto hasDiffuseColor = aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_DIFFUSE, &diffuseColor) == AI_SUCCESS;
		auto hasSpecularColor = aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_SPECULAR, &specularColor) == AI_SUCCESS;

		auto materialEntity = registry.create();

		auto hasTransparency = (baseColorTexture != entt::null && registry.get<Textures::ImageInfo>(baseColorTexture).depth > 3) ||
							   (ambientColorTexture != entt::null && registry.get<Textures::ImageInfo>(ambientColorTexture).depth > 3) ||
							   (diffuseColorTexture != entt::null && registry.get<Textures::ImageInfo>(diffuseColorTexture).depth > 3) || (hasAmbientColor && ambientColor.a < 1) ||
							   (hasDiffuseColor && diffuseColor.a < 1);

		Materials::Components::Material material = {
			.baseColor = glm::vec4(ambientColor.r, ambientColor.g, ambientColor.b, ambientColor.a),
			.ambientColor = glm::vec4(ambientColor.r, ambientColor.g, ambientColor.b, ambientColor.a),
			.diffuseColor = glm::vec4(diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a),
			.specularColor = glm::vec4(specularColor.r, specularColor.g, specularColor.b, specularColor.a),
			.baseColorTexture = baseColorTexture,
			.ambientColorTexture = ambientColorTexture,
			.diffuseColorTexture = diffuseColorTexture,
			.specularColorTexture = specularColorTexture,
			.normalMap = normalMap,
			.metallicRoughnessTexture = metallicRoughnessTexture,
			.metallicTexture = metallicTexture,
			.roughnessTexture = roughnessTexture,
			.hasTransparency = hasTransparency
		};

		registry.emplace<Materials::Components::Material>(materialEntity, material);
		registry.emplace<Common::Components::Name>(materialEntity, materialName);
		loadResult.materialIdEntityMap[aiMaterialIndex] = materialEntity;
	}
}
void AssimpLoader::postProcessSkinnedMeshes(entt::registry& registry) {
	registry.sort<Animations::Components::NodeBoneNodeMesh>([](const Animations::Components::NodeBoneNodeMesh& left, const Animations::Components::NodeBoneNodeMesh& right) {
		return left.nodeMeshEntity < right.nodeMeshEntity;
	});
	auto skinnedNodeMeshes = registry.view<Animations::Components::NodeBoneNodeMesh>(entt::exclude<PostProcessed>);
	std::vector<std::pair<entt::entity, std::vector<entt::entity>>> skinnedMeshInstances;
	entt::entity previousMeshInstanceEntity = entt::null;
	skinnedNodeMeshes.each(
		[&previousMeshInstanceEntity, &skinnedMeshInstances, &registry](entt::entity skinnedNodeMeshEntity, Animations::Components::NodeBoneNodeMesh& nodeBoneNodeMesh) {
			if (previousMeshInstanceEntity != nodeBoneNodeMesh.nodeMeshEntity) {
				skinnedMeshInstances.emplace_back(nodeBoneNodeMesh.nodeMeshEntity, std::vector<entt::entity>());
			}
			skinnedMeshInstances.back().second.emplace_back(skinnedNodeMeshEntity);
			previousMeshInstanceEntity = nodeBoneNodeMesh.nodeMeshEntity;
			registry.emplace<PostProcessed>(skinnedNodeMeshEntity);
		}
	);

	for (const auto& skinnedMeshInstance : skinnedMeshInstances) {
		const auto& nodeMesh = registry.get<Nodes::Components::NodeMesh>(skinnedMeshInstance.first);
		std::vector<Animations::Components::VertexWeightInstance> instanceSkinnedVertices;
		for (const auto& instance : skinnedMeshInstance.second) {
			const auto& nodeBoneNodeMesh = registry.get<Animations::Components::NodeBoneNodeMesh>(instance);
			const auto& boneMesh = registry.get<Animations::Components::BoneMesh>(nodeBoneNodeMesh.boneMeshEntity);
			uint32_t vertexWeightIndex = 0;
			for (const auto& vertexWeight : boneMesh.vertexWeights) {
				instanceSkinnedVertices.emplace_back(vertexWeight.vertexIndex, vertexWeight.weight, nodeBoneNodeMesh.nodeBoneEntity, nodeBoneNodeMesh.boneMeshEntity);
				vertexWeightIndex++;
			}
		}
		std::sort(
			instanceSkinnedVertices.begin(),
			instanceSkinnedVertices.end(),
			[](const Animations::Components::VertexWeightInstance& left, const Animations::Components::VertexWeightInstance& right) { return left.vertexIndex < right.vertexIndex; }
		);
		uint32_t const* previousInstanceVertexWeightVertexIndexPtr = nullptr;
		std::vector<Animations::Components::SkinnedVertexRange> instanceVertexWeightRanges;
		uint32_t instanceVertexWeightOffset = 0;
		for (const auto& instanceVertexWeight : instanceSkinnedVertices) {
			if (previousInstanceVertexWeightVertexIndexPtr == nullptr || *previousInstanceVertexWeightVertexIndexPtr != instanceVertexWeight.vertexIndex) {
				instanceVertexWeightRanges.emplace_back(instanceVertexWeight.vertexIndex, instanceVertexWeightOffset, 0);
			}
			instanceVertexWeightRanges.back().weightCount++;
			previousInstanceVertexWeightVertexIndexPtr = &instanceVertexWeight.vertexIndex;
			instanceVertexWeightOffset++;
		}
		registry.emplace<Animations::Components::SkinnedMeshInstance>(skinnedMeshInstance.first, std::move(instanceVertexWeightRanges), std::move(instanceSkinnedVertices));
	}
}
void AssimpLoader::loadMeshes(
	std::span<aiMesh*> aiMeshes,
	LoadResult& loadResult,
	entt::registry& registry,
	std::unordered_map<std::string, entt::entity>& aiBoneNodeNameBoneEntityMap,
	std::unordered_map<aiBone*, entt::entity>& aiBonePtrBoneEntityMap
) const {
	for (auto aiMeshIndex = 0u; aiMeshIndex < aiMeshes.size(); aiMeshIndex++) {
		const auto& aiMesh = aiMeshes[aiMeshIndex];
		auto meshEntity = registry.create();
		std::span<aiAnimMesh*> aiMeshAnims(aiMesh->mAnimMeshes, aiMesh->mNumAnimMeshes);
		for (const aiAnimMesh* aiAnimMesh : aiMeshAnims) {
		}
		if (aiMesh->HasBones()) {
			std::vector<entt::entity> boneEntities(aiMesh->mNumBones);
			registry.emplace<Animations::Components::MeshAnimation>(meshEntity);
			const std::span<aiBone*> aiBones(aiMesh->mBones, aiMesh->mNumBones);
			uint32_t boneIndex = 0;
			for (const auto& aiBone : aiBones) {
				auto boneEntityEntry = aiBoneNodeNameBoneEntityMap.find(aiBone->mName.C_Str());
				entt::entity boneEntity;
				if (boneEntityEntry != aiBoneNodeNameBoneEntityMap.end()) {
					boneEntity = boneEntityEntry->second;
				} else {
					boneEntity = registry.create();
					aiBonePtrBoneEntityMap[aiBone] = boneEntity;
					aiBoneNodeNameBoneEntityMap[aiBone->mName.C_Str()] = boneEntity;
					registry.emplace<Common::Components::Name>(boneEntity, aiBone->mName.C_Str());
					registry.emplace<Animations::Components::Bone>(boneEntity, std::vector<entt::entity>{});
				}

				entt::entity boneMeshEntity = registry.create();

				aiVector3D aiScale, aiPosition;
				aiQuaternion aiRotation;
				aiBone->mOffsetMatrix.Decompose(aiScale, aiRotation, aiPosition);
				glm::vec4 scale(aiScale.x, aiScale.y, aiScale.z, 1), position(aiPosition.x, aiPosition.y, aiPosition.z, 1);
				glm::quat rotation(aiRotation.w, aiRotation.x, aiRotation.y, aiRotation.z);
				auto translationMatrix = glm::translate(glm::identity<glm::mat4>(), glm::vec3(position));
				auto rotationMatrix = glm::toMat4(rotation);
				auto scalingMatrix = glm::scale(glm::identity<glm::mat4>(), glm::vec3(scale));
				auto localModel = translationMatrix * rotationMatrix * scalingMatrix;

				Spatials::Components::Spatial<Spatials::Components::Relative> boneSpatial{
					position,
					rotation,
					scale,
					localModel,
				};

				std::vector<Animations::Components::VertexWeight> weights(aiBone->mNumWeights);
				memcpy(weights.data(), (Animations::Components::VertexWeight*)aiBone->mWeights, aiBone->mNumWeights * sizeof(Animations::Components::VertexWeight));

				Animations::Components::BoneMesh boneMesh{
					.boneEntity = boneEntity, .skinnedMeshEntity = meshEntity, .offset = std::move(boneSpatial), .vertexWeights = std::move(weights)
				};
				registry.emplace<Animations::Components::BoneMesh>(boneMeshEntity, boneMesh);

				auto& bone = registry.get<Animations::Components::Bone>(boneEntity);
				bone.boneMeshEntities.push_back(boneMeshEntity);

				boneEntities[boneIndex] = boneEntity;
				boneIndex++;
			}
			registry.emplace<Animations::Components::BoneCollection>(meshEntity, std::move(boneEntities));
		}

		std::vector<uint32_t> indices(aiMesh->mNumFaces * 3);
		for (uint32_t faceIndex = 0u; faceIndex < aiMesh->mNumFaces; faceIndex++) {
			memcpy(indices.data() + faceIndex * 3, aiMesh->mFaces[faceIndex].mIndices, aiMesh->mFaces->mNumIndices * sizeof(uint32_t));
		}
		std::vector<Meshes::Vertex> vertices(aiMesh->mNumVertices);
		auto hasTangentsAndBitangents = aiMesh->HasTangentsAndBitangents();
		for (uint32_t vertexIndex = 0; vertexIndex < aiMesh->mNumVertices; vertexIndex++) {
			auto& vertex = vertices[vertexIndex];
			vertex.position = {aiMesh->mVertices[vertexIndex].x, aiMesh->mVertices[vertexIndex].y, aiMesh->mVertices[vertexIndex].z, 1};
			vertex.normal = {aiMesh->mNormals[vertexIndex].x, aiMesh->mNormals[vertexIndex].y, aiMesh->mNormals[vertexIndex].z, 0};
			vertex.tangent = aiMesh->mTangents != nullptr ? glm::vec4{aiMesh->mTangents[vertexIndex].x, aiMesh->mTangents[vertexIndex].y, aiMesh->mTangents[vertexIndex].z, 0}
														  : glm::vec4{0, 0, 0, 0};
			vertex.bitangent = aiMesh->mBitangents != nullptr
								   ? glm::vec4{aiMesh->mBitangents[vertexIndex].x, aiMesh->mBitangents[vertexIndex].y, aiMesh->mBitangents[vertexIndex].z, 0}
								   : glm::vec4{0, 0, 0, 0};
			if (aiMesh->HasVertexColors(0)) {
				vertex.diffuseColor.r = aiMesh->mColors[0][vertexIndex].r;
				vertex.diffuseColor.g = aiMesh->mColors[0][vertexIndex].g;
				vertex.diffuseColor.b = aiMesh->mColors[0][vertexIndex].b;
				vertex.diffuseColor.a = aiMesh->mColors[0][vertexIndex].a;
			} else {
				vertex.diffuseColor = {1.0, 1.0, 1.0, 1.0};
			}

			if (aiMesh->HasTextureCoords(0)) {
				vertex.textureCoordinates.x = aiMesh->mTextureCoords[0][vertexIndex].x;
				vertex.textureCoordinates.y = 1 - aiMesh->mTextureCoords[0][vertexIndex].y;
			} else {
				vertex.textureCoordinates = {0.0, 0.0, 0.0, 0.0};
			}
		}
		std::string meshName = aiMesh->mName.C_Str();
		auto materialEntity = loadResult.materialIdEntityMap[aiMesh->mMaterialIndex];
		Meshes::Components::MeshResource mesh = {.vertices = vertices, .indices = indices, .hasTangent = hasTangentsAndBitangents, .hasBitTangent = hasTangentsAndBitangents};
		auto axisAlignedBoundingBox = BoundingVolumes::Components::AxisAlignedBoundingBox::fromMinMax(
			glm::vec4(AssimpLoader::toVector(aiMesh->mAABB.mMin), 1.0f), glm::vec4(AssimpLoader::toVector(aiMesh->mAABB.mMax), 1.0f)
		);

		registry.emplace<Common::Components::Name>(meshEntity, meshName);
		registry.emplace<Meshes::Components::MeshResource>(meshEntity, std::move(mesh));
		registry.emplace<Materials::Components::MaterialReference>(meshEntity, materialEntity);
		registry.emplace<BoundingVolumes::Components::AxisAlignedBoundingBox>(meshEntity, axisAlignedBoundingBox);

		loadResult.meshIdEntityMap[aiMeshIndex] = meshEntity;
	}
}

void AssimpLoader::loadSkeletons(std::span<aiSkeleton*> aiSkeletons, LoadResult& loadResult, entt::registry& registry) const {
	for (const auto& aiSkeleton : aiSkeletons) {
		entt::entity skeletonEntity = registry.create();
		std::span<aiSkeletonBone*> aiSkeletonBones(aiSkeleton->mBones, aiSkeleton->mNumBones);
		for (const auto& aiSkeletonBone : aiSkeletonBones) {
			auto x = aiSkeletonBone->mArmature;
		}
	}
}

void AssimpLoader::loadAnimations(
	std::span<aiAnimation*> aiAnimations,
	std::unordered_map<std::string, entt::entity>& nodeNameAnimationMap,
	LoadResult& loadResult,
	entt::registry& registry
) const {
	for (const auto& aiAnimation : aiAnimations) {
		entt::entity animationEntity = registry.create();
		registry.emplace<Common::Components::Name>(animationEntity, aiAnimation->mName.C_Str());
		std::span<aiNodeAnim*> aiChannels(aiAnimation->mChannels, aiAnimation->mNumChannels);
		Animations::Components::Animation animation{.duration = aiAnimation->mDuration, .ticksPerSecond = aiAnimation->mTicksPerSecond};
		animation.nodeAnimations.resize(aiAnimation->mNumChannels);

		std::transform(aiChannels.begin(), aiChannels.end(), animation.nodeAnimations.data(), [&nodeNameAnimationMap, &registry, animationEntity](const aiNodeAnim* aiChannel) {
			std::span<aiVectorKey> aiPositionKeys(aiChannel->mPositionKeys, aiChannel->mNumPositionKeys);
			std::span<aiVectorKey> aiScalingKeys(aiChannel->mScalingKeys, aiChannel->mNumScalingKeys);
			std::span<aiQuatKey> aiRotationKeys(aiChannel->mRotationKeys, aiChannel->mNumRotationKeys);

			Animations::Components::NodeAnimation nodeAnimation{
				.animationEntity = animationEntity,
				.preState = animationBehaviorMap[aiChannel->mPreState],
				.postState = animationBehaviorMap[aiChannel->mPostState],
			};

			nodeAnimation.positionKeys.resize(aiChannel->mNumPositionKeys);
			nodeAnimation.scalingKeys.resize(aiChannel->mNumScalingKeys);
			nodeAnimation.rotationKeys.resize(aiChannel->mNumRotationKeys);

			std::transform(aiPositionKeys.begin(), aiPositionKeys.end(), nodeAnimation.positionKeys.data(), [](const aiVectorKey& aiVectorKey) {
				return Animations::Components::VectorKey{.time = aiVectorKey.mTime, .vector = glm::vec3{aiVectorKey.mValue.x, aiVectorKey.mValue.y, aiVectorKey.mValue.z}};
			});
			std::sort(nodeAnimation.positionKeys.begin(), nodeAnimation.positionKeys.end(), [](const auto& left, const auto right) { return left.time < right.time; });
			std::transform(aiScalingKeys.begin(), aiScalingKeys.end(), nodeAnimation.scalingKeys.data(), [](const aiVectorKey& aiVectorKey) {
				return Animations::Components::VectorKey{.time = aiVectorKey.mTime, .vector = glm::vec3{aiVectorKey.mValue.x, aiVectorKey.mValue.y, aiVectorKey.mValue.z}};
			});
			std::sort(nodeAnimation.rotationKeys.begin(), nodeAnimation.rotationKeys.end(), [](const auto& left, const auto right) { return left.time < right.time; });
			std::transform(aiRotationKeys.begin(), aiRotationKeys.end(), nodeAnimation.rotationKeys.data(), [](const aiQuatKey& aiQuatKey) {
				return Animations::Components::QuatKey{.time = aiQuatKey.mTime, .quat = glm::quat{aiQuatKey.mValue.w, aiQuatKey.mValue.x, aiQuatKey.mValue.y, aiQuatKey.mValue.z}};
			});
			std::sort(nodeAnimation.scalingKeys.begin(), nodeAnimation.scalingKeys.end(), [](const auto& left, const auto right) { return left.time < right.time; });
			entt::entity nodeAnimationEntity = registry.create();
			registry.emplace<Animations::Components::NodeAnimation>(nodeAnimationEntity, nodeAnimation);
			registry.emplace<Animations::Components::AnimationState>(nodeAnimationEntity, 0.0);
			nodeNameAnimationMap[aiChannel->mNodeName.C_Str()] = nodeAnimationEntity;
			return nodeAnimation;
		});
		std::span<aiMeshAnim*> aiMeshChannels(aiAnimation->mMeshChannels, aiAnimation->mNumMeshChannels);
		for (const auto& aiMeshChannel : aiMeshChannels) {
			std::span<aiMeshKey> aiMeshKeys(aiMeshChannel->mKeys, aiMeshChannel->mNumKeys);
			for (const auto& aiMeshKey : aiMeshKeys) {
			}
		}
		std::span<aiMeshMorphAnim*> aiMorphMeshChannels(aiAnimation->mMorphMeshChannels, aiAnimation->mNumMorphMeshChannels);
		for (const auto& aiMeshMorphChannel : aiMorphMeshChannels) {
			std::span<aiMeshMorphKey> aiMeshMorphKeys(aiMeshMorphChannel->mKeys, aiMeshMorphChannel->mNumKeys);
			for (const auto& aiMeshMorphKey : aiMeshMorphKeys) {
			}
		}
		registry.emplace<Animations::Components::Animation>(animationEntity, animation);
	}
}

void AssimpLoader::loadLights(
	std::span<aiLight*> aiLights,
	std::unordered_map<std::string, std::tuple<entt::entity, aiLightSourceType>>& lightNameMap,
	entt::registry& registry,
	LoadResult& loadResult
) const {
	for (auto aiLight : aiLights) {
		auto entity = registry.create();
		auto lightName = std::string(aiLight->mName.C_Str());
		if (aiLight->mType == aiLightSourceType::aiLightSource_POINT) {
			Lights::Components::PointLight pointLight;
			pointLight.constantAttenuation = aiLight->mAttenuationConstant;
			pointLight.linearAttenuation = aiLight->mAttenuationLinear;
			pointLight.quadraticAttenuation = aiLight->mAttenuationQuadratic;

			Spatials::Components::Spatial<Spatials::Components::Relative> lightSpatial{.position = {aiLight->mPosition.x, aiLight->mPosition.y, aiLight->mPosition.z, 1}};

			Lights::Components::LightPerspective frontLightPerspective = {
				.relativeFront = GlmExtensions::front, .relativeUp = GlmExtensions::up, .verticalFov = glm::half_pi<float>(), .aspectRatio = 1.0f, .near = 0.001f, .far = 128.0f
			};

			Lights::Components::LightPerspective backLightPerspective = {
				.relativeFront = GlmExtensions::back, .relativeUp = GlmExtensions::up, .verticalFov = glm::half_pi<float>(), .aspectRatio = 1.0f, .near = 0.001f, .far = 128.0f
			};

			Lights::Components::LightPerspective leftLightPerspective = {
				.relativeFront = GlmExtensions::left, .relativeUp = GlmExtensions::up, .verticalFov = glm::half_pi<float>(), .aspectRatio = 1.0f, .near = 0.001f, .far = 128.0f
			};

			Lights::Components::LightPerspective rightLightPerspective = {
				.relativeFront = GlmExtensions::right, .relativeUp = GlmExtensions::up, .verticalFov = glm::half_pi<float>(), .aspectRatio = 1.0f, .near = 0.001f, .far = 128.0f
			};

			Lights::Components::LightPerspective upLightPerspective = {
				.relativeFront = GlmExtensions::up, .relativeUp = GlmExtensions::back, .verticalFov = glm::half_pi<float>(), .aspectRatio = 1.0f, .near = 0.001f, .far = 128.0f
			};

			Lights::Components::LightPerspective downLightPerspective = {
				.relativeFront = GlmExtensions::down, .relativeUp = GlmExtensions::front, .verticalFov = glm::half_pi<float>(), .aspectRatio = 1.0f, .near = 0.001f, .far = 128.0f
			};

			auto frontLightPerspectiveEntity = registry.create();
			registry.emplace<Lights::Components::LightPerspective>(frontLightPerspectiveEntity, std::move(frontLightPerspective));
			auto backLightPerspectiveEntity = registry.create();
			registry.emplace<Lights::Components::LightPerspective>(backLightPerspectiveEntity, std::move(backLightPerspective));
			auto leftLightPerspectiveEntity = registry.create();
			registry.emplace<Lights::Components::LightPerspective>(leftLightPerspectiveEntity, std::move(leftLightPerspective));
			auto rightLightPerspectiveEntity = registry.create();
			registry.emplace<Lights::Components::LightPerspective>(rightLightPerspectiveEntity, std::move(rightLightPerspective));
			auto upLightPerspectiveEntity = registry.create();
			registry.emplace<Lights::Components::LightPerspective>(upLightPerspectiveEntity, std::move(upLightPerspective));
			auto downLightPerspectiveEntity = registry.create();
			registry.emplace<Lights::Components::LightPerspective>(downLightPerspectiveEntity, std::move(downLightPerspective));

			pointLight.frontLightPerspectiveEntity = frontLightPerspectiveEntity;
			pointLight.backLightPerspectiveEntity = backLightPerspectiveEntity;
			pointLight.leftLightPerspectiveEntity = leftLightPerspectiveEntity;
			pointLight.rightLightPerspectiveEntity = rightLightPerspectiveEntity;
			pointLight.topLightPerspectiveEntity = upLightPerspectiveEntity;
			pointLight.downLightPerspectiveEntity = downLightPerspectiveEntity;

			Lights::Components::LightPerspectiveCollection lightPerspectiveCollection{
				.lightPerspectives =
					{frontLightPerspectiveEntity,
					 backLightPerspectiveEntity,
					 leftLightPerspectiveEntity,
					 rightLightPerspectiveEntity,
					 upLightPerspectiveEntity,
					 downLightPerspectiveEntity}
			};

			registry.emplace<Lights::Components::PointLight>(entity, pointLight);
			registry.emplace<Spatials::Components::Spatial<Spatials::Components::Relative>>(entity, lightSpatial);
			registry.emplace<Lights::Components::LightPerspectiveCollection>(entity, lightPerspectiveCollection);

			loadResult.pointLights.emplace_back(pointLight);
		}
		if (aiLight->mType == aiLightSourceType::aiLightSource_DIRECTIONAL) {
			Lights::Components::LightPerspective lightPerspective{
				.relativeFront = {aiLight->mDirection.x, aiLight->mDirection.y, aiLight->mDirection.z, 0}, .relativeUp = {aiLight->mUp.x, aiLight->mUp.y, aiLight->mUp.z, 0}
			};
			Lights::Components::DirectionalLight directionalLight;

			Spatials::Components::Spatial<Spatials::Components::Relative> lightSpatial{
				.position = {aiLight->mPosition.x, aiLight->mPosition.y, aiLight->mPosition.z, 1},
			};

			registry.emplace<Spatials::Components::Spatial<Spatials::Components::Relative>>(entity, lightSpatial);
			registry.emplace<Lights::Components::DirectionalLight>(entity, directionalLight);
			registry.emplace<Lights::Components::LightPerspective>(entity, lightPerspective);

			loadResult.directionalLights.emplace_back(directionalLight);
		}
		if (aiLight->mType == aiLightSourceType::aiLightSource_SPOT) {
			Lights::Components::Spotlight spotlight;
			spotlight.constantAttenuation = aiLight->mAttenuationConstant;
			spotlight.linearAttenuation = aiLight->mAttenuationLinear;
			spotlight.quadraticAttenuation = aiLight->mAttenuationQuadratic;
			spotlight.innerConeAngle = aiLight->mAngleInnerCone;
			spotlight.outerConeAngle = aiLight->mAngleOuterCone;

			Spatials::Components::Spatial<Spatials::Components::Relative> lightSpatial{
				.position = {aiLight->mPosition.x, aiLight->mPosition.y, aiLight->mPosition.z, 1},
			};

			Lights::Components::LightPerspective lightPerspective{
				.relativeFront = {aiLight->mDirection.x, aiLight->mDirection.y, aiLight->mDirection.z, 0},
				.relativeUp = {aiLight->mUp.x, aiLight->mUp.y, aiLight->mUp.z, 0},
				.verticalFov = spotlight.outerConeAngle * 2.0f,
				.aspectRatio = 1.0f,
				.near = 0.001f,
				.far = 128.0f
			};

			registry.emplace<Lights::Components::Spotlight>(entity, spotlight);
			registry.emplace<Spatials::Components::Spatial<Spatials::Components::Relative>>(entity, lightSpatial);
			registry.emplace<Lights::Components::LightPerspective>(entity, lightPerspective);
			loadResult.spotlights.emplace_back(spotlight);
		}
		Lights::Components::Light light = {
			{aiLight->mColorAmbient.r / 256.0f, aiLight->mColorAmbient.g / 256.0f, aiLight->mColorAmbient.b / 256.0f, 1},
			{aiLight->mColorDiffuse.r / 256.0f, aiLight->mColorDiffuse.g / 256.0f, aiLight->mColorDiffuse.b / 256.0f, 1},
			{aiLight->mColorSpecular.r / 256.0f, aiLight->mColorSpecular.g / 256.0f, aiLight->mColorSpecular.b / 256.0f, 1}
		};
		registry.emplace<Lights::Components::Light>(entity, light);
		lightNameMap[lightName] = {entity, aiLight->mType};
	}
}

void AssimpLoader::loadCameras(std::span<aiCamera*> aiCameras, std::unordered_map<std::string, entt::entity>& cameraNameMap, entt::registry& registry) const {
	for (auto aiCamera : aiCameras) {
		auto cameraName = std::string(aiCamera->mName.C_Str());
		auto cameraPosition = glm::vec3{aiCamera->mPosition.x, aiCamera->mPosition.y, aiCamera->mPosition.z};
		auto cameraLookAt = glm::vec3{
			aiCamera->mLookAt.x,
			aiCamera->mLookAt.y,
			aiCamera->mLookAt.z,
		};
		auto cameraUp = glm::vec3{
			aiCamera->mUp.x,
			aiCamera->mUp.y,
			aiCamera->mUp.z,
		};
		auto perspective = glm::perspectiveZO(aiCamera->mHorizontalFOV, aiCamera->mAspect, aiCamera->mClipPlaneNear, aiCamera->mClipPlaneFar);
		perspective[1][1] *= -1.0f;

		auto verticalFov = aiCamera->mHorizontalFOV / aiCamera->mAspect;
		auto front = glm::normalize(cameraLookAt - cameraPosition);

		Cameras::Components::Camera camera{
			perspective,
			glm::lookAt(cameraPosition, cameraLookAt, cameraUp),
			cameraPosition,
			front,
			cameraUp,
			verticalFov,
			aiCamera->mAspect,
			aiCamera->mClipPlaneNear,
			aiCamera->mClipPlaneFar
		};

		auto cameraFrustum = Frustums::Components::Frustum::createFrustumFromView(
			glm::vec4(cameraPosition, 1.f), 
			glm::vec4(front, 0.f), 
			glm::vec4(cameraUp, 0.f), 
			verticalFov, 
			aiCamera->mAspect, 
			aiCamera->mClipPlaneNear, 
			aiCamera->mClipPlaneFar
		);

		auto entity = registry.create();
		registry.emplace<Cameras::Components::Camera>(entity, camera);
		registry.emplace<Frustums::Components::Frustum>(entity, cameraFrustum);

		cameraNameMap[cameraName] = entity;
	}
}

entt::entity AssimpLoader::loadNode(
	const aiNode* assimpNode,
	const std::unordered_map<std::string, std::tuple<entt::entity, aiLightSourceType>>& lightMap,
	const std::unordered_map<std::string, entt::entity>& cameraMap,
	const std::unordered_map<std::string, entt::entity>& aiBonePtrBoneEntityMap,
	const std::unordered_map<std::string, entt::entity>& aiNodeNameNodeAnimationMap,
	std::stack<std::unordered_map<entt::entity, entt::entity>>& meshEntityInstanceEntityMap,
	LoadResult& loadResult,
	entt::registry& registry,
	std::unordered_map<const aiNode*, entt::entity>& cache,
	entt::entity rootBoneInstanceEntity,
	int depth
) const {
	auto cacheEntry = cache.find(assimpNode);
	if (cacheEntry != cache.end()) {
		return cacheEntry->second;
	}
	auto nodeName = std::string(assimpNode->mName.C_Str());

	aiVector3D aiScale, aiPosition;
	aiQuaternion aiRotation;
	assimpNode->mTransformation.Decompose(aiScale, aiRotation, aiPosition);
	glm::vec4 scale(aiScale.x, aiScale.y, aiScale.z, 1), position(aiPosition.x, aiPosition.y, aiPosition.z, 1);
	glm::quat rotation(aiRotation.w, aiRotation.x, aiRotation.y, aiRotation.z);
	auto translationMatrix = glm::translate(glm::identity<glm::mat4>(), glm::vec3(position));
	auto rotationMatrix = glm::toMat4(rotation);
	auto scalingMatrix = glm::scale(glm::identity<glm::mat4>(), glm::vec3(scale));
	auto localModel = translationMatrix * rotationMatrix * scalingMatrix;

	Spatials::Components::Spatial<Spatials::Components::Relative> spatial{position, rotation, scale, localModel};
    Spatials::Components::Spatial<Spatials::Components::Absolute> absoluteSpatial{ position, rotation, scale, localModel };

	entt::entity entity = entt::null;
	bool shouldEmplaceSpatial = true;

	auto light = lightMap.find(nodeName);
	if (light != lightMap.end()) {
		shouldEmplaceSpatial = false;
		entity = std::get<0>(light->second);
		auto lightType = std::get<1>(light->second);
		if (lightType == aiLightSourceType::aiLightSource_DIRECTIONAL) {
			auto& directionalLightSpatial = registry.get<Spatials::Components::Spatial<Spatials::Components::Relative>>(entity);
			directionalLightSpatial.rotation = spatial.rotation;
			directionalLightSpatial.position += glm::vec4(glm::vec3(spatial.position), 0.0f);
		} else if (lightType == aiLightSourceType::aiLightSource_POINT) {
			auto& pointLight = registry.get<Lights::Components::PointLight>(entity);
			auto& pointLightSpatial = registry.get<Spatials::Components::Spatial<Spatials::Components::Relative>>(entity);
			pointLightSpatial.rotation = spatial.rotation;
			pointLightSpatial.position += glm::vec4(glm::vec3(spatial.position), 0.0f);
		} else if (lightType == aiLightSourceType::aiLightSource_SPOT) {
			auto& spotlight = registry.get<Lights::Components::Spotlight>(entity);
			auto& spotlightSpatial = registry.get<Spatials::Components::Spatial<Spatials::Components::Relative>>(entity);
			spotlightSpatial.rotation = spatial.rotation;
			spotlightSpatial.position += glm::vec4(glm::vec3(spatial.position), 0.0f);
		}
	}
	auto cameraEntry = cameraMap.find(nodeName);
	if (cameraEntry != cameraMap.end()) {
		entity = cameraEntry->second;
		auto camera = registry.get<Cameras::Components::Camera>(entity);
		auto eulerAngles = glm::eulerAngles(spatial.rotation);
	}

	if (entity == entt::null)
		entity = registry.create();
	registry.emplace<Common::Components::Name>(entity, assimpNode->mName.C_Str());

	auto nodeAnimationEntityEntry = aiNodeNameNodeAnimationMap.find(assimpNode->mName.C_Str());
	if (nodeAnimationEntityEntry != aiNodeNameNodeAnimationMap.end()) {
		auto& nodeAnimation = registry.get<Animations::Components::NodeAnimation>(nodeAnimationEntityEntry->second);
		nodeAnimation.nodeEntity = entity;
	}

	auto boneEntityEntry = aiBonePtrBoneEntityMap.find(assimpNode->mName.C_Str());
	if (boneEntityEntry != aiBonePtrBoneEntityMap.end()) {
		auto boneEntity = boneEntityEntry->second;
		const auto& bone = registry.get<Animations::Components::Bone>(boneEntity);
		auto boneInstanceEntity = registry.create();
		for (auto boneMeshEntity : bone.boneMeshEntities) {
			auto nodeBoneMeshEntity = registry.create();
			const auto& boneMesh = registry.get<Animations::Components::BoneMesh>(boneMeshEntity);
			auto& map = meshEntityInstanceEntityMap.top();
			if (map.contains(boneMesh.skinnedMeshEntity)) {
				auto nodeMeshEntity = map[boneMesh.skinnedMeshEntity];
				registry.emplace<Animations::Components::NodeBoneNodeMesh>(nodeBoneMeshEntity, entity, rootBoneInstanceEntity, nodeMeshEntity, boneMeshEntity);
			}
		}
		registry.emplace<Animations::Components::RootBoneInstanceReference>(entity, rootBoneInstanceEntity);
		if (rootBoneInstanceEntity == entt::null) {
			rootBoneInstanceEntity = boneInstanceEntity;
		}
	} else {
		rootBoneInstanceEntity = entt::null;
	}
	if (assimpNode->mNumMeshes) {
		Nodes::Components::NodeMeshCollection objectMeshCollection;
		for (auto meshIndex = 0u; meshIndex < assimpNode->mNumMeshes; meshIndex++) {
			auto& aiMesh = assimpNode->mMeshes[meshIndex];
			auto meshEntity = loadResult.meshIdEntityMap[assimpNode->mMeshes[meshIndex]];
			auto objectMeshEntity = registry.create();
			auto& map = meshEntityInstanceEntityMap.top();
			map[meshEntity] = objectMeshEntity;
			registry.emplace<Nodes::Components::NodeMesh>(objectMeshEntity, entity, meshEntity);
			registry.emplace<Meshes::Components::Mesh>(objectMeshEntity);

			auto meshAABB = registry.get<BoundingVolumes::Components::AxisAlignedBoundingBox>(meshEntity);
			auto instanceAABB = meshAABB.transform(spatial.model);

			Nodes::Components::Node objectMeshRelationship{.parent = entity, .depth = depth + 1};
			registry.emplace<BoundingVolumes::Components::AxisAlignedBoundingBox>(objectMeshEntity, meshAABB);
			registry.emplace<Nodes::Components::Node>(objectMeshEntity, std::move(objectMeshRelationship));
			auto boneCollection = registry.try_get<Animations::Components::BoneCollection>(meshEntity);
			if (boneCollection != nullptr)
				registry.emplace<Animations::Components::Skinned>(objectMeshEntity);

			objectMeshCollection.nodeMeshes.push_back(objectMeshEntity);
		}
		registry.emplace<Nodes::Components::NodeMeshCollection>(entity, std::move(objectMeshCollection));
	}

	if (shouldEmplaceSpatial)
        {
          registry.emplace<Spatials::Components::Spatial<Spatials::Components::Relative>>(entity, spatial);
          registry.emplace<Spatials::Components::Spatial<Spatials::Components::Absolute>>(entity, absoluteSpatial);
        }
        

	Nodes::Components::Node relationship;
	Nodes::Components::Node* previousRelationship = nullptr;
	entt::entity previousSibling{entt::null};

	if (loadResult.rootEntity == entt::null) {
		loadResult.rootEntity = entity;
	}

	relationship.depth = depth;
	cache[assimpNode] = entity;
	if (assimpNode->mNumChildren) {
		meshEntityInstanceEntityMap.push(meshEntityInstanceEntityMap.top());
		for (auto childIndex = 0u; childIndex < assimpNode->mNumChildren; childIndex++) {
			auto aiChildNode = assimpNode->mChildren[childIndex];
			auto childEntity = loadNode(
				aiChildNode,
				lightMap,
				cameraMap,
				aiBonePtrBoneEntityMap,
				aiNodeNameNodeAnimationMap,
				meshEntityInstanceEntityMap,
				loadResult,
				registry,
				cache,
				rootBoneInstanceEntity,
				depth + 1
			);
			relationship.children.push_back(childEntity);
			auto& childRelationship = registry.get<Nodes::Components::Node>(childEntity);
			childRelationship.parent = entity;
		}
		meshEntityInstanceEntityMap.pop();
	}

	registry.emplace<Nodes::Components::Node>(entity, relationship);

	return entity;
}

std::unordered_map<aiTextureType, Textures::TextureType> AssimpLoader::TextureTypeMap = {
	{aiTextureType::aiTextureType_AMBIENT, Textures::TextureType::AmbientColor},
	{aiTextureType::aiTextureType_AMBIENT_OCCLUSION, Textures::TextureType::AmbientOcclusionMap},
	{aiTextureType::aiTextureType_BASE_COLOR, Textures::TextureType::BaseColor},
	{aiTextureType::aiTextureType_DIFFUSE, Textures::TextureType::DiffuseColor},
	{aiTextureType::aiTextureType_DIFFUSE_ROUGHNESS, Textures::TextureType::RoughnessMap},
	{aiTextureType::aiTextureType_DISPLACEMENT, Textures::TextureType::DisplacementMap},
	{aiTextureType::aiTextureType_EMISSION_COLOR, Textures::TextureType::EmissionColor},
	{aiTextureType::aiTextureType_EMISSIVE, Textures::TextureType::EmissionMap},
	{aiTextureType::aiTextureType_HEIGHT, Textures::TextureType::HeightMap},
	{aiTextureType::aiTextureType_LIGHTMAP, Textures::TextureType::LightMap},
	{aiTextureType::aiTextureType_METALNESS, Textures::TextureType::MetalnessMap},
	{aiTextureType::aiTextureType_NONE, Textures::TextureType::None},
	{aiTextureType::aiTextureType_NORMALS, Textures::TextureType::NormalMap},
	{aiTextureType::aiTextureType_NORMAL_CAMERA, Textures::TextureType::NormalCameraMap},
	{aiTextureType::aiTextureType_OPACITY, Textures::TextureType::OpacityMap},
	{aiTextureType::aiTextureType_REFLECTION, Textures::TextureType::ReflectionMap},
	{aiTextureType::aiTextureType_SHININESS, Textures::TextureType::ShininessMap},
	{aiTextureType::aiTextureType_SPECULAR, Textures::TextureType::SpecularColor},
	{aiTextureType::aiTextureType_UNKNOWN, Textures::TextureType::RoughnessMetalnessMap},
};

std::unordered_map<Textures::TextureType, std::regex> AssimpLoader::TextureTypeRegexMap = {
	{Textures::TextureType::BaseColor, std::regex(".*_albedo.*", std::regex_constants::syntax_option_type::icase)},
	{Textures::TextureType::AlbedoColor, std::regex(".*_albedo.*", std::regex_constants::syntax_option_type::icase)},
	{Textures::TextureType::AmbientColor, std::regex(".*_albedo.*", std::regex_constants::syntax_option_type::icase)},
	{Textures::TextureType::DiffuseColor, std::regex(".*_albedo.*", std::regex_constants::syntax_option_type::icase)},
	{Textures::TextureType::AmbientOcclusionMap, std::regex(".*_ao.*", std::regex_constants::syntax_option_type::icase)},
	{Textures::TextureType::MetalnessMap, std::regex(".*_metalness.*", std::regex_constants::syntax_option_type::icase)},
	{Textures::TextureType::NormalMap, std::regex(".*_normal.*", std::regex_constants::syntax_option_type::icase)},
	{Textures::TextureType::SpecularColor, std::regex(".*_specular.*", std::regex_constants::syntax_option_type::icase)},
	{Textures::TextureType::RoughnessMap, std::regex(".*_roughness.*", std::regex_constants::syntax_option_type::icase)}
};

std::unordered_map<aiAnimBehaviour, Animations::Components::AnimationBehavior> AssimpLoader::animationBehaviorMap = {
	{aiAnimBehaviour::aiAnimBehaviour_CONSTANT, Animations::Components::AnimationBehavior::Constant},
	{aiAnimBehaviour::aiAnimBehaviour_REPEAT, Animations::Components::AnimationBehavior::Repeat},
	{aiAnimBehaviour::aiAnimBehaviour_LINEAR, Animations::Components::AnimationBehavior::Linear},
};

glm::vec3& AssimpLoader::toVector(const aiVector3D& aiVector) { return (*(glm::vec3*)&aiVector); }
[[maybe_unused]] glm::vec4 AssimpLoader::toVector4(const aiVector3D& aiVector, float w) { return glm::vec4(aiVector.x, aiVector.y, aiVector.z, w); }
}  // namespace drk::Loaders
