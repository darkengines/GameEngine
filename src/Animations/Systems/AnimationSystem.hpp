#pragma once
#include "../Models/VertexWeightInput.hpp"
#include "../Components/VertexWeightInstance.hpp"
#include "../Components/HasVertexWeightRange.hpp"
#include "../Components/SkinnedMeshInstance.hpp"
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
#include <vector>

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

		void createSkinnedMeshInstanceResources() {
			auto skinnedMeshInstances = registry.view<Components::SkinnedMeshInstance>();
			std::vector<entt::entity> entities(skinnedMeshInstances.size());
			std::vector<std::span<const Components::VertexWeightInstance>> vertexWeightInstanceBuffers(skinnedMeshInstances.size());
			std::vector<std::span<const Components::SkinnedVertexRange>> skinnedVertexRangeBuffers(skinnedMeshInstances.size());
			uint32_t skinnedMeshInstanceIndex = 0u;

			skinnedMeshInstances.each([&](
				entt::entity entity,
				const Components::SkinnedMeshInstance& skinnedMeshInstance
				) {
					entities[skinnedMeshInstanceIndex] = entity;
					vertexWeightInstanceBuffers[skinnedMeshInstanceIndex] = std::span(
						skinnedMeshInstance.skinnedVertices.begin(), 
						skinnedMeshInstance.skinnedVertices.end()
					);
					skinnedVertexRangeBuffers[skinnedMeshInstanceIndex] = std::span(
						skinnedMeshInstance.skinnedVertexRanges.begin(),
						skinnedMeshInstance.skinnedVertexRanges.end()
					);
					skinnedMeshInstanceIndex++;
				});

			const auto& vertexWeightInstanceBufferUploadResult = Devices::Device::uploadBuffers(
				deviceContext.PhysicalDevice,
				deviceContext.device,
				deviceContext.GraphicQueue,
				deviceContext.CommandPool,
				deviceContext.Allocator,
				vertexWeightInstanceBuffers,
				vk::BufferUsageFlagBits::eStorageBuffer
			);
			const auto& skinnedVertexRangeBufferUploadResult = Devices::Device::uploadBuffers(
				deviceContext.PhysicalDevice,
				deviceContext.device,
				deviceContext.GraphicQueue,
				deviceContext.CommandPool,
				deviceContext.Allocator,
				skinnedVertexRangeBuffers,
				vk::BufferUsageFlagBits::eStorageBuffer
			);
			for (skinnedMeshInstanceIndex = 0u; skinnedMeshInstanceIndex < entities.size(); skinnedMeshInstanceIndex++) {
				auto entity = entities[skinnedMeshInstanceIndex];
				auto 
			}
		}

		void updateSkins(vk::CommandBuffer commandBuffer) {
			registry.sort<Objects::Components::ObjectMeshReference>([](const Objects::Components::ObjectMeshReference& left, const Objects::Components::ObjectMeshReference& right) {
				return left.meshInstanceEntity < right.meshInstanceEntity;
				});
			auto meshBoneInstances = registry.view<
				Objects::Components::ObjectReference,
				Components::RootBoneInstanceReference,
				Components::BoneReference,
				Objects::Components::ObjectMeshReference
			>(entt::exclude<Components::HasVertexWeightRange>);
			meshBoneInstances.use<Objects::Components::ObjectMeshReference>();
			std::vector<std::pair<entt::entity, std::vector<entt::entity>>> skinnedMeshInstances;
			entt::entity previousMeshInstanceEntity = entt::null;
			meshBoneInstances.each([this, &previousMeshInstanceEntity, &skinnedMeshInstances](
				entt::entity objectMeshEntity,
				const Objects::Components::ObjectReference& objectReference,
				const Components::RootBoneInstanceReference& rootBoneInstanceReference,
				const Components::BoneReference& boneReference,
				const Objects::Components::ObjectMeshReference& objectMeshReference
				) {
					if (previousMeshInstanceEntity != objectMeshReference.meshInstanceEntity) {
						skinnedMeshInstances.emplace_back(objectMeshReference.meshInstanceEntity, std::vector<entt::entity>());
					}
					skinnedMeshInstances.back().second.emplace_back(objectMeshEntity);
					previousMeshInstanceEntity = objectMeshReference.meshInstanceEntity;
					registry.emplace<Components::HasVertexWeightRange>(objectMeshEntity);
				});

			for (const auto& skinnedMeshInstance : skinnedMeshInstances) {
				const auto& [meshReference] = registry.get<Meshes::Components::MeshReference>(skinnedMeshInstance.first);
				std::vector<Components::VertexWeightInstance> instanceSkinnedVertices;
				for (const auto& instance : skinnedMeshInstance.second) {
					const auto& [objectReference, rootBoneInstanceReference, boneReference] = registry.get<
						Objects::Components::ObjectReference,
						Components::RootBoneInstanceReference,
						Components::BoneReference
					>(instance);
					const auto& [boneNodeSpatialStoreItem, boneNodeStoreItem] = registry.get<
						Stores::StoreItem<Spatials::Models::Spatial>,
						Stores::StoreItem<Objects::Models::Object>
					>(objectReference.objectEntity);
					const auto& [bone, vertexWeights, boneStoreItem] = registry.get<
						Components::Bone,
						std::vector<Components::VertexWeight>,
						Stores::StoreItem<Models::Bone>
					>(boneReference.boneEntity);
					uint32_t vertexWeightIndex = 0;
					for (const auto& vertexWeight : vertexWeights) {
						instanceSkinnedVertices.emplace_back(vertexWeight.vertexIndex, vertexWeight.weight, instance);
						vertexWeightIndex++;
					}
				}
				std::sort(instanceSkinnedVertices.begin(), instanceSkinnedVertices.end(), [](const Components::VertexWeightInstance& left, const Components::VertexWeightInstance& right) {
					return left.vertexIndex < right.vertexIndex;
					});
				uint32_t const* previousInstanceVertexWeightVertexIndexPtr = nullptr;
				std::vector<Components::VertexWeightInputRange> instanceVertexWeightRanges;
				uint32_t instanceVertexWeightOffset = 0;
				for (const auto& instanceVertexWeight : instanceSkinnedVertices) {
					if (previousInstanceVertexWeightVertexIndexPtr == nullptr
						|| *previousInstanceVertexWeightVertexIndexPtr != instanceVertexWeight.vertexIndex) {
						instanceVertexWeightRanges.emplace_back(instanceVertexWeight.vertexIndex, instanceVertexWeightOffset, 0);
					}
					instanceVertexWeightRanges.back().length++;
					previousInstanceVertexWeightVertexIndexPtr = &instanceVertexWeight.vertexIndex;
					instanceVertexWeightOffset++;
				}
				registry.emplace<Components::SkinnedMeshInstance>(skinnedMeshInstance.first, std::move(instanceVertexWeightRanges), std::move(instanceSkinnedVertices));
			}

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