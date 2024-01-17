#pragma once
#include "../Resources/AnimationResourceManager.hpp"
#include <entt/entt.hpp>
#include <utility>
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
			auto boneEntities = registry.view<Components::Bone>(entt::exclude<Components::VertexWeightBufferView>);
			std::vector<entt::entity> processedBoneEntities;
			std::vector<std::vector<Components::VertexWeight>> vertexWeights;
			boneEntities.each(
				[&vertexWeights, &processedBoneEntities](const auto boneEntities, const auto& bone) {
					processedBoneEntities.push_back(boneEntities);
					vertexWeights.push_back(bone.weights);
				}
			);
			if (!vertexWeights.empty()) {
				const auto& result = animationResourceManager.createVertexWeightBuffers(vertexWeights);
				for (auto boneIndex = 0u; boneIndex < processedBoneEntities.size(); boneIndex++) {
					registry.emplace<Components::VertexWeightBufferView>(
						processedBoneEntities[boneIndex],
						result[boneIndex]
					);
				}
			}
		}

		void mamadou() {
			auto skinnedObjectMeshView = registry.view<
				Objects::Components::ObjectReference,
				Meshes::Components::MeshReference,
				Animations::Components::SkinnedBufferView,
				Animations::Components::AnimationState
			>();
			skinnedObjectMeshView.each([](
				entt::entity objectMeshEntity,
				const Objects::Components::ObjectReference& objectReference,
				const Meshes::Components::MeshReference& meshReference,
				const Animations::Components::SkinnedBufferView& skinnedBufferView,
				const Animations::Components::AnimationState& animationState
			) {
				//const auto& [] = registry.get<>
			});
		}
	};
}