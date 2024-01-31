#pragma once
#include "../Models/VertexWeightInput.hpp"
#include "../Components/VertexWeightInputRange.hpp"
#include "../Components/VertexWeightInputMap.hpp"
#include "../Components/RootBoneInstanceReference.hpp"
#include "../Components/VertexWeightInput.hpp"
#include "../Components/HasVertexWeightInput.hpp"
#include "../Models/SkinningInput.hpp"
#include "../Components/HasSkinInput.hpp"
#include "../../Spatials/Models/Spatial.hpp"
#include "../../Objects/Models/Object.hpp"
#include "../../Stores/Models/StoreItemLocation.hpp"
#include "../Models/Bone.hpp"

#include "../Models/SkinningInput.hpp"
#include "../../Spatials/Systems/SpatialSystem.hpp"
#include "../Components/Animation.hpp"
#include "../Components/BoneCollection.hpp"	
#include "../Components/Skinned.hpp"
#include "../Components/NodeAnimation.hpp"
#include "../Components/BoneReference.hpp"
#include "../../Spatials/Components/Spatial.hpp"
#include "../Components/AnimationReference.hpp"
#include "../Resources/AnimationResourceManager.hpp"
#include <entt/entt.hpp>
#include <utility>
#include <glm/glm.hpp>
#include "../../Objects/Components/ObjectReference.hpp"
#include "../../Meshes/Components/MeshReference.hpp"
#include "../Components/AnimationState.hpp"
#include "../Components/AnimationVertexBufferView.hpp"
#include "../../Meshes/Components/Mesh.hpp"
#include "../../Meshes/Components/MeshBufferView.hpp"
#include "../../Devices/BufferViewState.hpp"
#include "../../Devices/DeviceContext.hpp"
#include "../../Devices/Device.hpp"
#include "../../Engine/EngineState.hpp"
#include "../../Engine/DescriptorSetAllocator.hpp"
#include "../../Engine/DescriptorSetLayoutCache.hpp"
#include "../Components/Bone.hpp"
#include "../Components/VertexWeightBufferView.hpp"
#include "../Pipelines/SkinningPipeline.hpp"
#include "../../Objects/Components/ObjectMeshReference.hpp"

namespace drk::Animations::Systems {
	class AnimationSystem {
	protected:
		entt::registry& registry;
		Devices::DeviceContext& deviceContext;
		Engine::EngineState& engineState;
		std::vector<Devices::Buffer> buffers;
		Engine::DescriptorSetAllocator& descriptorSetAllocator;
		Engine::DescriptorSetLayoutCache& descriptorSetLayoutCache;
		Animations::Resources::AnimationResourceManager& animationResourceManager;
		Pipelines::SkinningPipeline& skinningPipeline;
	public:
		AnimationSystem(
			entt::registry& registry,
			Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			Animations::Resources::AnimationResourceManager& animationResourceManager,
			Pipelines::SkinningPipeline& skinningPipeline
		) :
			registry(registry),
			deviceContext(deviceContext),
			engineState(engineState),
			descriptorSetAllocator(descriptorSetAllocator),
			descriptorSetLayoutCache(descriptorSetLayoutCache),
			animationResourceManager(animationResourceManager),
			skinningPipeline(skinningPipeline) {}
		~AnimationSystem() {
			for (const auto buffer : buffers) {
				Devices::Device::destroyBuffer(deviceContext.Allocator, buffer);
			}
		}
		void storeMeshes() {
			auto view = registry.view<
				Objects::Components::ObjectReference,
				Meshes::Components::MeshReference,
				Animations::Components::Skinned
			>(entt::exclude<Components::SkinnedBufferView>);
			std::vector<Devices::BufferView> vertexBufferViews;
			std::vector<entt::entity> entities;
			view.each([&](
				entt::entity objectMeshEntity,
				const Objects::Components::ObjectReference& objectReference,
				const Meshes::Components::MeshReference& meshReference
				) {
					const auto& meshBufferView = registry.get<
						Meshes::Components::MeshBufferView
					>(meshReference.meshEntity);
					vertexBufferViews.emplace_back(meshBufferView.VertexBufferView);
					entities.emplace_back(objectMeshEntity);
				});
			if (entities.size() > 0) {
				const auto& skinnedBufferViews = animationResourceManager.createSkinnedMesh(entities, vertexBufferViews);
				for (const auto& skinnedBufferView : skinnedBufferViews) {
					registry.emplace<Components::SkinnedBufferView>(skinnedBufferView.second, skinnedBufferView.first);
				}
			}
		}

		void mamadou() {
			auto view = registry.view<
				Objects::Components::ObjectReference,
				Components::RootBoneInstanceReference,
				Components::BoneReference,
				Objects::Components::ObjectMeshReference
			>(entt::exclude<Components::HasVertexWeightInput>);
			view.each([this](
				entt::entity entity,
				const Objects::Components::ObjectReference& objectReference,
				Components::RootBoneInstanceReference& rootBoneInstanceReference,
				const Components::BoneReference& boneReference,
				const Objects::Components::ObjectMeshReference& objectMeshReference
				) {
					const auto& [bone, vertexWeights] = registry.get<
						Animations::Components::Bone,
						std::vector<Animations::Components::VertexWeight>
					>(boneReference.boneEntity);
					const auto& meshReference = registry.get<Meshes::Components::MeshReference>(objectMeshReference.meshInstanceEntity);
					for (const auto& vertexWeight : vertexWeights) {
						const auto inputEntity = registry.create();
						Components::VertexWeightInput input{
							.pVertexWeight = &vertexWeight
						};
						registry.emplace<Components::VertexWeightInput>(inputEntity, input);
						registry.emplace<Objects::Components::ObjectReference>(inputEntity, objectReference);
						registry.emplace<Components::RootBoneInstanceReference>(inputEntity, rootBoneInstanceReference);
						registry.emplace<Meshes::Components::MeshReference>(inputEntity, meshReference);
						registry.emplace<Objects::Components::ObjectMeshReference>(inputEntity, objectMeshReference);
					}
					registry.emplace<Components::HasVertexWeightInput>(entity);
				});
			registry.sort<Components::VertexWeightInput>([this](
				entt::entity leftEntity,
				entt::entity rightEntity
				) {
					const auto& [leftWertexWeightInput, leftRootBoneInstanceReference, leftMeshReference] = registry.get<
						Components::VertexWeightInput,
						Components::RootBoneInstanceReference,
						Meshes::Components::MeshReference
					>(leftEntity);
					const auto& [rightWertexWeightInput, rightRootBoneInstanceReference, rightMeshReference] = registry.get<
						Components::VertexWeightInput,
						Components::RootBoneInstanceReference,
						Meshes::Components::MeshReference
					>(rightEntity);

					if (leftWertexWeightInput.pVertexWeight->vertexIndex != rightWertexWeightInput.pVertexWeight->vertexIndex) {
						return leftWertexWeightInput.pVertexWeight->vertexIndex < rightWertexWeightInput.pVertexWeight->vertexIndex;
					}
					if (leftMeshReference.meshEntity != rightMeshReference.meshEntity) {
						return leftMeshReference.meshEntity < rightMeshReference.meshEntity;
					}
					return leftRootBoneInstanceReference.rootBoneInstanceEntity > rightRootBoneInstanceReference.rootBoneInstanceEntity;

				});


			auto inputView = registry.view<
				Components::VertexWeightInput,
				Components::RootBoneInstanceReference,
				Meshes::Components::MeshReference
			>();
			inputView.use<Components::VertexWeightInput>();

			entt::entity previousMeshEntity = entt::null;
			entt::entity previousRootBoneInstanceEntity = entt::null;
			uint32_t const* previousVertexIndexPtr = nullptr;
			std::vector<Components::VertexWeightInput*> previousVertexWeightInputs;
			uint32_t previousWeightInputCount = 0;
			inputView.each([this, &previousMeshEntity, &previousRootBoneInstanceEntity, &previousVertexIndexPtr, &previousVertexWeightInputs, &previousWeightInputCount](
				entt::entity entity,
				Components::VertexWeightInput& vertexWeightInput,
				const Components::RootBoneInstanceReference& rootBoneInstanceReference,
				const Meshes::Components::MeshReference& meshReference
				) {

					if (previousVertexIndexPtr != nullptr
						&& (vertexWeightInput.pVertexWeight->vertexIndex != *previousVertexIndexPtr
							|| (previousRootBoneInstanceEntity != entt::null && rootBoneInstanceReference.rootBoneInstanceEntity != previousRootBoneInstanceEntity)
							|| meshReference.meshEntity != previousMeshEntity)) {
						for (auto previousVertexWeightInput : previousVertexWeightInputs) {
							previousVertexWeightInput->vertexWeightCount = previousWeightInputCount;
						}
						previousWeightInputCount = 0;
						previousVertexWeightInputs.clear();
					}
					previousVertexWeightInputs.push_back(&vertexWeightInput);
					previousMeshEntity = meshReference.meshEntity;
					previousRootBoneInstanceEntity = rootBoneInstanceReference.rootBoneInstanceEntity;
					previousVertexIndexPtr = &vertexWeightInput.pVertexWeight->vertexIndex;
					previousWeightInputCount++;
				});
			for (auto previousVertexWeightInput : previousVertexWeightInputs) {
				previousVertexWeightInput->vertexWeightCount = previousWeightInputCount;
			}

			registry.sort<Components::VertexWeightInput>([this](
				entt::entity leftEntity,
				entt::entity rightEntity
				) {
					const auto& [leftWertexWeightInput, leftRootBoneInstanceReference, leftMeshReference] = registry.get<
						Components::VertexWeightInput,
						Components::RootBoneInstanceReference,
						Meshes::Components::MeshReference
					>(leftEntity);
					const auto& [rightWertexWeightInput, rightRootBoneInstanceReference, rightMeshReference] = registry.get<
						Components::VertexWeightInput,
						Components::RootBoneInstanceReference,
						Meshes::Components::MeshReference
					>(rightEntity);

					if (leftWertexWeightInput.vertexWeightCount != rightWertexWeightInput.vertexWeightCount) {
						return leftWertexWeightInput.vertexWeightCount < rightWertexWeightInput.vertexWeightCount;
					}
					if (leftWertexWeightInput.pVertexWeight->vertexIndex != rightWertexWeightInput.pVertexWeight->vertexIndex) {
						return leftWertexWeightInput.pVertexWeight->vertexIndex < rightWertexWeightInput.pVertexWeight->vertexIndex;
					}
					if (leftMeshReference.meshEntity != rightMeshReference.meshEntity) {
						return leftMeshReference.meshEntity < rightMeshReference.meshEntity;
					}
					return leftRootBoneInstanceReference.rootBoneInstanceEntity > rightRootBoneInstanceReference.rootBoneInstanceEntity;
				});

			auto view1 = registry.view <
				Components::VertexWeightInput,
				Components::RootBoneInstanceReference,
				Meshes::Components::MeshReference,
				Objects::Components::ObjectMeshReference
			>();
			view1.use<Components::VertexWeightInput>();
			std::vector<Components::VertexWeightInputMap> vertexWeightInputMapping;
			std::vector<Components::VertexWeightInputRange> vertexWeightInputRanges;

			previousMeshEntity = entt::null;
			previousRootBoneInstanceEntity = entt::null;
			previousVertexIndexPtr = nullptr;
			entt::entity previousMeshInstance = entt::null;
			auto previousInputCount = 0u;
			auto vertexWeightInputIndex = 0u;
			Components::VertexWeightInputRange range{ .offset = 0, .length = 0 };

			view1.each([this, &previousMeshEntity, &previousRootBoneInstanceEntity, &previousVertexIndexPtr, &previousInputCount, &vertexWeightInputMapping, &vertexWeightInputRanges, &vertexWeightInputIndex, &range, &previousMeshInstance](
				entt::entity entity,
				Components::VertexWeightInput& vertexWeightInput,
				const Components::RootBoneInstanceReference& rootBoneInstanceReference,
				const Meshes::Components::MeshReference& meshReference,
				const Objects::Components::ObjectMeshReference& meshInstanceReference
				) {
					if (previousInputCount > 0 && previousInputCount != vertexWeightInput.vertexWeightCount) {
						vertexWeightInputRanges.emplace_back(range.offset, range.length);
						range = { .offset = vertexWeightInputIndex, .length = 0 };
					}
					range.length++;
					if (previousVertexIndexPtr != nullptr
						&& (vertexWeightInput.pVertexWeight->vertexIndex != *previousVertexIndexPtr
							|| (previousRootBoneInstanceEntity != entt::null && rootBoneInstanceReference.rootBoneInstanceEntity != previousRootBoneInstanceEntity)
							|| meshReference.meshEntity != previousMeshEntity)) {
						auto skinnedBufferView = registry.get<Components::SkinnedBufferView>(previousMeshInstance);
						vertexWeightInputMapping.emplace_back(
							skinnedBufferView.bufferArrayElement,
							skinnedBufferView.bufferView.byteOffset / sizeof(Models::VertexWeight) + *previousVertexIndexPtr,
							skinnedBufferView.skinnedBufferArrayElement,
							skinnedBufferView.skinnedBufferView.byteOffset / sizeof(Models::VertexWeight) + *previousVertexIndexPtr
						);
					}
					previousMeshEntity = meshReference.meshEntity;
					previousRootBoneInstanceEntity = rootBoneInstanceReference.rootBoneInstanceEntity;
					previousVertexIndexPtr = &vertexWeightInput.pVertexWeight->vertexIndex;
					previousInputCount = vertexWeightInput.vertexWeightCount;
					previousMeshInstance = meshInstanceReference.meshInstanceEntity;
					vertexWeightInputIndex++;
				});
			if (vertexWeightInputIndex > 0) {
				vertexWeightInputRanges.emplace_back(range.offset, range.length);
				auto skinnedBufferView = registry.get<Components::SkinnedBufferView>(previousMeshInstance);
				vertexWeightInputMapping.emplace_back(
					skinnedBufferView.bufferArrayElement,
					skinnedBufferView.bufferView.byteOffset / sizeof(Models::VertexWeight) + *previousVertexIndexPtr,
					skinnedBufferView.skinnedBufferArrayElement,
					skinnedBufferView.skinnedBufferView.byteOffset / sizeof(Models::VertexWeight) + *previousVertexIndexPtr
				);
				auto x = 0;
			}
		}

		void uploadVertexWeights() {
			auto entities = registry.view<std::vector<Components::VertexWeight>>(entt::exclude<Components::VertexWeightBufferView>);
			std::vector<entt::entity> processedEntities;
			std::vector<std::vector<Components::VertexWeight>> vertexWeights;
			entities.each(
				[&vertexWeights, &processedEntities](const auto entity, const auto& weights) {
					processedEntities.push_back(entity);
					vertexWeights.push_back(weights);
				}
			);
			if (!vertexWeights.empty()) {
				const auto& result = animationResourceManager.createVertexWeightBuffers(vertexWeights);
				for (auto bufferIndex = 0u; bufferIndex < processedEntities.size(); bufferIndex++) {
					registry.emplace<Components::VertexWeightBufferView>(
						processedEntities[bufferIndex],
						result[bufferIndex]
					);
				}
			}
		}

		void updateSkins(vk::CommandBuffer commandBuffer) {
			auto skinnedObjectMeshView = registry.view<
				Objects::Components::ObjectReference,
				Components::RootBoneInstanceReference,
				Components::BoneReference,
				Objects::Components::ObjectMeshReference
			>(entt::exclude<Components::HasSkinInput>);
			std::vector<Models::SkinningInput> skinningInputs;
			uint32_t skinningInputCount = 0;
			skinnedObjectMeshView.each([this, &skinningInputCount](
				entt::entity objectMeshEntity,
				const Objects::Components::ObjectReference& objectReference,
				const Components::RootBoneInstanceReference& rootBoneInstanceReference,
				const Components::BoneReference& boneReference,
				const Objects::Components::ObjectMeshReference& objectMeshReference
				) {
					const auto& [bone, vertexWeights, vertexWeightBufferView, boneStoreItem] = registry.get<
						Components::Bone,
						std::vector<Animations::Components::VertexWeight>,
						Components::VertexWeightBufferView,
						Stores::StoreItem<Models::Bone>
					>(boneReference.boneEntity);
					const auto& [meshReference, skinnedBufferView] = registry.get<Meshes::Components::MeshReference, Components::SkinnedBufferView>(objectMeshReference.meshInstanceEntity);


					const auto& [boneNodeStoreItem, boneNodeSpatialStoreItem] = registry.get<
						Stores::StoreItem<Spatials::Models::Spatial>,
						Stores::StoreItem<Objects::Models::Object>
					>(objectReference.objectEntity);

					uint32_t weightIndex = 0;
					for (const auto vertexWeight : vertexWeights) {

						Stores::Models::StoreItemLocation vertexWeightItemLocation{
							.storeIndex = static_cast<uint32_t>(vertexWeightBufferView.bufferIndex),
							.itemIndex = static_cast<uint32_t>(vertexWeightBufferView.bufferView.byteOffset / sizeof(Models::VertexWeight)) + weightIndex
						};

						Stores::Models::StoreItemLocation vertexItemLocation{
							.storeIndex = static_cast<uint32_t>(skinnedBufferView.bufferArrayElement),
							.itemIndex = static_cast<uint32_t>(skinnedBufferView.bufferView.byteOffset / sizeof(Meshes::Vertex)) + vertexWeight.vertexIndex,
						};
						Stores::Models::StoreItemLocation skinnedVertexItemLocation{
							.storeIndex = static_cast<uint32_t>(skinnedBufferView.skinnedBufferArrayElement),
							.itemIndex = static_cast<uint32_t>(skinnedBufferView.skinnedBufferView.byteOffset / sizeof(Meshes::Vertex)) + vertexWeight.vertexIndex,
						};

						Models::SkinningInput skinningInput{
							.objectItemLocation = boneNodeSpatialStoreItem.frameStoreItems[engineState.getFrameIndex()],
							.vertexItemLocation = vertexItemLocation,
							.skinnedVertexItemLocation = skinnedVertexItemLocation,
							.vertexWeightItemLocation = vertexWeightItemLocation,
							.boneItemLocation = boneStoreItem.frameStoreItems[engineState.getFrameIndex()],
						};

						auto skinningInputStoreItem1 = engineState.frameStates[0].AddUniformStoreItem<Models::SkinningInput>();
						auto skinningInputStoreItem2 = engineState.frameStates[1].AddUniformStoreItem<Models::SkinningInput>();
						*skinningInputStoreItem1.pItem = skinningInput;
						*skinningInputStoreItem2.pItem = skinningInput;
						auto inputEntity = registry.create();
						registry.emplace<Stores::StoreItemLocation<Models::SkinningInput>>(inputEntity, skinningInputStoreItem1);
						skinningInputCount++;
						weightIndex++;
					}
					//skinningInputs.emplace_back(std::move(skinningInput));

					registry.emplace<Components::HasSkinInput>(objectMeshEntity);
				});
			skinningPipeline.bind(commandBuffer);
			auto view = registry.view<Stores::StoreItemLocation<Models::SkinningInput>>();
			view.each([](entt::entity entity, Stores::StoreItemLocation<Models::SkinningInput>& input) {
				auto x = input;
				});
			auto entityCount = view.size();
			auto count = static_cast<uint32_t>(std::ceil(entityCount / 32.0f));
			commandBuffer.dispatch(entityCount, 1, 1);
		}

		void updateAnimations() {
			auto skinnedObjectMeshView = registry.view<
				Objects::Components::ObjectReference,
				Animations::Components::AnimationReference,
				Animations::Components::AnimationState,
				Animations::Components::NodeAnimation
			>();
			skinnedObjectMeshView.each([this](
				entt::entity objectMeshEntity,
				const Objects::Components::ObjectReference& objectReference,
				const Animations::Components::AnimationReference& animationReference,
				Animations::Components::AnimationState& animationState,
				const Animations::Components::NodeAnimation& nodeAnimation
				) {
					const Components::Animation& animation = registry.get<Animations::Components::Animation>(animationReference.animationEntity);
					auto& nodeSpatial = registry.get_or_emplace<Spatials::Components::Spatial<Spatials::Components::Relative>>(objectReference.objectEntity);

					auto position = glm::make_vec3(nodeSpatial.position);
					auto scaling = glm::make_vec3(nodeSpatial.scale);
					auto rotation = nodeSpatial.rotation;
					auto time = std::chrono::duration<double, std::milli>(engineState.getDuration()).count();
					update(time, nodeAnimation, animation, position, rotation, scaling);

					nodeSpatial.position = glm::vec4(position, 1);
					nodeSpatial.rotation = rotation;
					nodeSpatial.scale = glm::vec4(scaling, 0);

					Spatials::Systems::SpatialSystem::makeDirty(registry, objectReference.objectEntity);
				});
		}

		void update(
			double time,
			const Components::NodeAnimation& nodeAnimation,
			const Components::Animation& animation,
			glm::vec3& position,
			glm::quat& rotation,
			glm::vec3& scaling
		) {
			auto isOver = time / animation.duration >= 1.0;
			time = std::fmod(time, animation.duration);

			if (nodeAnimation.positionKeys.size() > 1) {
				auto positionUpperBound = std::upper_bound(nodeAnimation.positionKeys.begin(), nodeAnimation.positionKeys.end(), time, [](double time, const auto& positionKey) { return time < positionKey.time; });
				if (positionUpperBound == nodeAnimation.positionKeys.end()) {
					position = nodeAnimation.positionKeys[nodeAnimation.positionKeys.size() - 1].vector;
				}
				else if (positionUpperBound != nodeAnimation.positionKeys.begin())
				{
					auto currentPositionIndex = std::distance(nodeAnimation.positionKeys.begin(), positionUpperBound) - 1;
					const auto& currentPosition = nodeAnimation.positionKeys[currentPositionIndex];
					const auto& nextPosition = nodeAnimation.positionKeys[currentPositionIndex + 1];
					auto positionTransitionDuration = nextPosition.time - currentPosition.time;
					auto positionTransitionTime = time - currentPosition.time;
					auto positionTransitionProgress = positionTransitionTime / positionTransitionDuration;
					position = currentPosition.vector + (nextPosition.vector - currentPosition.vector) * (float)positionTransitionProgress;
				}
			}
			else {
				position = nodeAnimation.positionKeys[0].vector;
			}

			if (nodeAnimation.rotationKeys.size() > 1) {
				auto rotationUpperBound = std::upper_bound(nodeAnimation.rotationKeys.begin(), nodeAnimation.rotationKeys.end(), time, [](double time, const auto& rotationKey) { return time < rotationKey.time; });
				if (rotationUpperBound == nodeAnimation.rotationKeys.end()) {
					rotation = nodeAnimation.rotationKeys[nodeAnimation.rotationKeys.size() - 1].quat;
				}
				else if (rotationUpperBound != nodeAnimation.rotationKeys.begin())
				{
					auto currentRotationIndex = std::distance(nodeAnimation.rotationKeys.begin(), rotationUpperBound) - 1;
					const auto& currentRotation = nodeAnimation.rotationKeys[currentRotationIndex];
					const auto& nextRotation = nodeAnimation.rotationKeys[currentRotationIndex + 1];
					auto rotationTransitionDuration = nextRotation.time - currentRotation.time;
					auto rotationTransitionTime = time - currentRotation.time;
					auto rotationTransitionProgress = rotationTransitionTime / rotationTransitionDuration;
					rotation = glm::slerp(currentRotation.quat, nextRotation.quat, (float)rotationTransitionProgress);
				}
			}
			else {
				rotation = nodeAnimation.rotationKeys[0].quat;
			}

			if (nodeAnimation.scalingKeys.size() > 1) {
				auto scaleUpperBound = std::upper_bound(nodeAnimation.scalingKeys.begin(), nodeAnimation.scalingKeys.end(), time, [](double time, const auto& scaleKey) { return time < scaleKey.time; });
				if (scaleUpperBound == nodeAnimation.scalingKeys.end()) {
					scaling = nodeAnimation.scalingKeys[nodeAnimation.scalingKeys.size() - 1].vector;
				}
				else if (scaleUpperBound != nodeAnimation.scalingKeys.begin())
				{
					auto currentScaleIndex = std::distance(nodeAnimation.scalingKeys.begin(), scaleUpperBound) - 1;
					const auto& currentScale = nodeAnimation.scalingKeys[currentScaleIndex];
					const auto& nextScale = nodeAnimation.scalingKeys[currentScaleIndex + 1];
					auto scaleTransitionDuration = nextScale.time - currentScale.time;
					auto scaleTransitionTime = time - currentScale.time;
					auto scaleTransitionProgress = scaleTransitionTime / scaleTransitionDuration;
					scaling = currentScale.vector + (nextScale.vector - currentScale.vector) * (float)scaleTransitionProgress;
				}
			}
			else {
				scaling = nodeAnimation.scalingKeys[0].vector;
			}
		}
	};
}