#pragma once
#include "../../Spatials/Systems/SpatialSystem.hpp"
#include "../Components/Animation.hpp"
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
	public:
		AnimationSystem(
			entt::registry& registry,
			Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			Animations::Resources::AnimationResourceManager& animationResourceManager
		) :
			registry(registry),
			deviceContext(deviceContext),
			engineState(engineState),
			descriptorSetAllocator(descriptorSetAllocator),
			descriptorSetLayoutCache(descriptorSetLayoutCache),
			animationResourceManager(animationResourceManager) {}
		~AnimationSystem() {
			for (const auto buffer : buffers) {
				Devices::Device::destroyBuffer(deviceContext.Allocator, buffer);
			}
		}
		void storeMeshes() {
			auto view = registry.view<
				Objects::Components::ObjectReference,
				Meshes::Components::MeshReference,
				Components::AnimationState
			>(entt::exclude<Components::SkinnedBufferView>);
			std::vector<Devices::BufferView> vertexBufferViews;
			std::vector<entt::entity> entities;
			view.each([&](
				entt::entity objectMeshEntity,
				const Objects::Components::ObjectReference& objectReference,
				const Meshes::Components::MeshReference& meshReference,
				const Components::AnimationState& animationState
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
					Spatials::Components::Spatial<Spatials::Components::Relative>& nodeSpatial = registry.get<Spatials::Components::Spatial<Spatials::Components::Relative>>(objectReference.objectEntity);

					glm::vec3 position{ 0, 0, 0 };
					glm::vec3 scaling{ 0, 0, 0 };
					glm::quat rotation{ 0, 0, 0, 0 };
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
				auto positionLowerBound = std::lower_bound(nodeAnimation.positionKeys.begin(), nodeAnimation.positionKeys.end(), time, [](const auto& positionKey, double time) { return positionKey.time < time; });
				auto currentPositionIndex = std::distance(nodeAnimation.positionKeys.begin(), positionLowerBound) % (nodeAnimation.positionKeys.size() - 1);
				const auto& currentPosition = nodeAnimation.positionKeys[currentPositionIndex];
				const auto& nextPosition = nodeAnimation.positionKeys[currentPositionIndex + 1];
				auto positionTransitionDuration = nextPosition.time - currentPosition.time;
				auto positionTransitionTime = time - currentPosition.time;
				auto positionTransitionProgress = positionTransitionTime / positionTransitionDuration;
				position = currentPosition.vector + (nextPosition.vector - currentPosition.vector) * (float)positionTransitionProgress;
			}
			else {
				position = nodeAnimation.positionKeys[0].vector;
			}

			if (nodeAnimation.rotationKeys.size() > 1) {
				auto rotationLowerBound = std::lower_bound(nodeAnimation.rotationKeys.begin(), nodeAnimation.rotationKeys.end(), time, [](const auto& rotationKey, double time) { return rotationKey.time < time; });
				auto currentRotationIndex = std::distance(nodeAnimation.rotationKeys.begin(), rotationLowerBound) % (nodeAnimation.rotationKeys.size() - 1);
				const auto& currentRotation = nodeAnimation.rotationKeys[currentRotationIndex];
				const auto& nextRotation = nodeAnimation.rotationKeys[currentRotationIndex + 1];
				auto rotationTransitionDuration = nextRotation.time - currentRotation.time;
				auto rotationTransitionTime = time - currentRotation.time;
				auto rotationTransitionProgress = rotationTransitionTime / rotationTransitionDuration;
				rotation = glm::slerp(currentRotation.quat, nextRotation.quat, (float)rotationTransitionProgress);
			}
			else {
				rotation = nodeAnimation.rotationKeys[0].quat;
			}

			if (nodeAnimation.scalingKeys.size() > 1) {
				auto scalingLowerBound = std::lower_bound(nodeAnimation.scalingKeys.begin(), nodeAnimation.scalingKeys.end(), time, [](const auto& scalingKey, double time) { return scalingKey.time < time; });
				auto currentScalingIndex = std::distance(nodeAnimation.scalingKeys.begin(), scalingLowerBound) % (nodeAnimation.scalingKeys.size() - 1);
				const auto& currentScaling = nodeAnimation.scalingKeys[currentScalingIndex];
				const auto& nextScaling = nodeAnimation.scalingKeys[currentScalingIndex + 1];
				auto scalingTransitionDuration = nextScaling.time - currentScaling.time;
				auto scalingTransitionTime = time - currentScaling.time;
				auto scalingTransitionProgress = scalingTransitionTime / scalingTransitionDuration;
				scaling = currentScaling.vector + (nextScaling.vector - currentScaling.vector) * (float)scalingTransitionProgress;
			}
			else {
				scaling = nodeAnimation.scalingKeys[0].vector;
			}
		}
	};
}