#pragma once
#include "../Models/VertexWeightInput.hpp"
#include "../Models/SkinnedVertexRange.hpp"
#include "../Models/BoneInstanceWeight.hpp"
#include "../Models/BoneSpatial.hpp"
#include "../Components/VertexWeightInstance.hpp"
#include "../Components/BoneInstanceWeightBufferView.hpp"
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

		void createSkinnedMeshInstanceResources(uint32_t frameIndex) {
			auto skinnedMeshInstances = registry.view<
				Components::SkinnedMeshInstance,
				Meshes::Components::MeshReference,
				Components::SkinnedBufferView
			>(entt::exclude<Components::BoneInstanceWeightBufferView>);

			std::vector<entt::entity> entities;
			std::vector<std::vector<Models::BoneInstanceWeight>> boneInstanceWeightBuffers;
			std::vector<std::vector<Models::SkinnedVertexRange>> skinnedVertexRangeBuffers;
			std::vector<Devices::BufferView> vertexBufferViews;

			skinnedMeshInstances.each([&](
				entt::entity entity,
				const Components::SkinnedMeshInstance& skinnedMeshInstance,
				const Meshes::Components::MeshReference& meshReference,
				const Components::SkinnedBufferView& skinnedBufferView
				) {
					const auto& meshBufferView = registry.get<Meshes::Components::MeshBufferView>(meshReference.meshEntity);
					vertexBufferViews.emplace_back(meshBufferView.VertexBufferView);

					entities.emplace_back(entity);
					std::vector<Models::BoneInstanceWeight> boneInstanceWeights(skinnedMeshInstance.skinnedVertices.size());
					std::transform(
						skinnedMeshInstance.skinnedVertices.begin(),
						skinnedMeshInstance.skinnedVertices.end(),
						boneInstanceWeights.data(),
						[&](const Components::VertexWeightInstance& vertexWeightInstance) {
							/*auto boneInstanceNodeReference = registry.get<
								Objects::Components::ObjectReference
							>(vertexWeightInstance.boneInstanceEntity);*/
							const auto& [boneInstanceStoreItem, boneSpatialStoreItem] = registry.get<
								Stores::StoreItem<Objects::Models::Object>,
								Stores::StoreItem<Models::BoneSpatial>
							>(vertexWeightInstance.boneInstanceEntity);
							const auto& spatial = registry.get<
								Spatials::Components::Spatial<Spatials::Components::Relative>
							>(vertexWeightInstance.boneInstanceEntity);
							auto boneStoreItem = registry.get<
								Stores::StoreItem<Models::Bone>
							>(vertexWeightInstance.boneEntity);
							return Models::BoneInstanceWeight{
								.boneInstanceStoreItemLocation = boneInstanceStoreItem.frameStoreItems[frameIndex],
								.boneInstanceSpatialStoreItemLocation = boneSpatialStoreItem.frameStoreItems[frameIndex],
								.boneStoreItemLocation = boneStoreItem.frameStoreItems[frameIndex],
								.weight = vertexWeightInstance.weight
							};
						}
					);
					boneInstanceWeightBuffers.emplace_back(std::move(boneInstanceWeights));
				});

			if (!boneInstanceWeightBuffers.empty()) {
				std::vector<std::span<Models::BoneInstanceWeight>> boneInstanceWeightBufferSpans(boneInstanceWeightBuffers.size());
				std::transform(boneInstanceWeightBuffers.begin(), boneInstanceWeightBuffers.end(), boneInstanceWeightBufferSpans.data(), [](auto& boneInstanceWeights) {
					return std::span(boneInstanceWeights.begin(), boneInstanceWeights.end());
					});
				auto boneInstanceWeightBufferViews = animationResourceManager.createBoneInstanceWeightBufferViews(boneInstanceWeightBufferSpans);

				for (auto entityIndex = 0; entityIndex < entities.size(); entityIndex++) {
					registry.emplace<Components::BoneInstanceWeightBufferView>(
						entities[entityIndex],
						boneInstanceWeightBufferViews[entityIndex]
					);
				}

				auto skinnedWeightedMeshInstances = registry.view<
					Components::SkinnedMeshInstance,
					Meshes::Components::MeshReference,
					Components::SkinnedBufferView,
					Components::BoneInstanceWeightBufferView
				>();

				skinnedWeightedMeshInstances.each([&](
					entt::entity entity,
					const Components::SkinnedMeshInstance& skinnedMeshInstance,
					const Meshes::Components::MeshReference& meshReference,
					const Components::SkinnedBufferView& skinnedBufferView,
					const Components::BoneInstanceWeightBufferView& boneInstanceWeightBufferView
					) {
						const auto& meshBufferView = registry.get<Meshes::Components::MeshBufferView>(meshReference.meshEntity);
						vertexBufferViews.emplace_back(meshBufferView.VertexBufferView);

						std::vector<Models::SkinnedVertexRange> skinnedVertexRanges(skinnedMeshInstance.skinnedVertexRanges.size());
						std::transform(
							skinnedMeshInstance.skinnedVertexRanges.begin(),
							skinnedMeshInstance.skinnedVertexRanges.end(),
							skinnedVertexRanges.data(),
							[&](const Components::SkinnedVertexRange& skinnedVertexRange) {
								return Models::SkinnedVertexRange{
									.vertexBufferIndex = skinnedBufferView.bufferArrayElement,
									.vertexBufferItemOffset = static_cast<uint32_t>(skinnedBufferView.bufferView.byteOffset / sizeof(Meshes::Vertex)),
									.vertexIndex = skinnedVertexRange.vertexIndex,
									.skinnedVertexBufferIndex = skinnedBufferView.frameSkinnedBufferArrayElements[0],
									.skinnedVertexBufferItemOffset = static_cast<uint32_t>(skinnedBufferView.frameSkinnedBufferViews[0].byteOffset / sizeof(Meshes::Vertex)),
									.skinnedVertexIndex = skinnedVertexRange.vertexIndex,
									.vertexWeightBufferIndex = boneInstanceWeightBufferView.bufferIndex,
									.vertexWeightBufferItemOffset = static_cast<uint32_t>(boneInstanceWeightBufferView.bufferView.byteOffset / sizeof(Models::BoneInstanceWeight)),
									.vertexWeightIndex = skinnedVertexRange.weightOffset,
									.vertexWeightCount = skinnedVertexRange.weightCount
								};
							}
						);
						skinnedVertexRangeBuffers.emplace_back(std::move(skinnedVertexRanges));
					});
				std::vector<std::span<Models::SkinnedVertexRange>> skinnedVertexRangeBufferSpans(skinnedVertexRangeBuffers.size());
				std::transform(skinnedVertexRangeBuffers.begin(), skinnedVertexRangeBuffers.end(), skinnedVertexRangeBufferSpans.data(), [](auto& skinnedVertexRangeBuffer) {
					return std::span(skinnedVertexRangeBuffer.begin(), skinnedVertexRangeBuffer.end());
					});
				auto result = animationResourceManager.createSkinnedVertexRangeBufferViews(skinnedVertexRangeBufferSpans);
				for (auto entityIndex = 0; entityIndex < entities.size(); entityIndex++) {
					registry.emplace<Components::SkinnedVertexRangeBufferView>(
						entities[entityIndex],
						result[entityIndex].bufferView,
						result[entityIndex].bufferIndex
					);
				}
			}
		}

		void updateSkins(vk::CommandBuffer commandBuffer) {

			auto view = registry.view<Components::SkinnedVertexRangeBufferView>();
			skinningPipeline.bind(commandBuffer);
			view.each([&commandBuffer, this](entt::entity entity, const Components::SkinnedVertexRangeBufferView& skinnedVertexRangeBufferView) {
				auto rangeCount = static_cast<uint32_t>(skinnedVertexRangeBufferView.bufferView.byteLength / sizeof(Models::SkinnedVertexRange));
				auto itemOffset = static_cast<uint32_t>(skinnedVertexRangeBufferView.bufferView.byteOffset / sizeof(Models::SkinnedVertexRange));
				auto dispatchCount = (int)std::ceil(rangeCount / 32.0f);
				skinningPipeline.setOptions(commandBuffer, { skinnedVertexRangeBufferView.bufferIndex, itemOffset, rangeCount });
				commandBuffer.dispatch(dispatchCount, 1, 1);
				});
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