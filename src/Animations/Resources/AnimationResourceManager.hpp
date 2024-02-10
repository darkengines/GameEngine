#pragma once

#include <vulkan/vulkan.hpp>
#include "../../Meshes/Components/MeshBufferView.hpp"
#include "../Models/BoneInstanceWeight.hpp"
#include "../Models/SkinnedVertexRange.hpp"
#include "../Components/BoneInstanceWeightBufferView.hpp"
#include "../Components/SkinnedVertexRangeBufferView.hpp"
#include "../Components/SkinnedBufferView.hpp"
#include "../Components/VertexWeightBufferView.hpp"
#include "../../Engine/EngineState.hpp"
#include "../../Resources/ResourceManager.hpp"
#include "../../Engine/DescriptorSetAllocator.hpp"
#include "../../Engine/DescriptorSetLayoutCache.hpp"
#include "../Components/VertexWeight.hpp"
#include "./AnimationFrameResource.hpp"
#include <vector>
#include <utility>

namespace drk::Animations::Resources {
	class AnimationResourceManager : public drk::Resources::ResourceManager {
	protected:
		Devices::DeviceContext& deviceContext;
		std::vector<Devices::Buffer> buffers;
		uint32_t vertexBufferDescriptorSetArrayElementOffset;
		uint32_t skinnedVertexBufferDescriptorSetArrayElementOffset;
		uint32_t vertexWeightDescriptorSetArrayElementOffset;
		uint32_t skinnedVertexRangeDescriptorSetArrayElementOffset;
		Engine::DescriptorSetAllocator& descriptorSetAllocator;
		Engine::DescriptorSetLayoutCache& descriptorSetLayoutCache;
		static std::vector<AnimationFrameResource> createFrameResources(
			const drk::Resources::Configuration::ResourcesConfiguration& resourceConfiguration,
			Engine::DescriptorSetAllocator& descriptorSetAllocator,
			vk::DescriptorSetLayout& vertexBufferDescriptorSetLayout
		);
		static vk::DescriptorSetLayout createVertexBufferDescriptorSetLayout(Engine::DescriptorSetLayoutCache& descriptorSetLayoutCache);
		static vk::DescriptorSet createVertexBufferDescriptorSet(
			Engine::DescriptorSetAllocator& descriptorSetAllocator,
			vk::DescriptorSetLayout& vertexBufferDescriptorSetLayout
		);
		static vk::DescriptorSetLayout createVertexWeightDescriptorSetLayout(Engine::DescriptorSetLayoutCache& descriptorSetLayoutCache);
		static vk::DescriptorSet createVertexWeightDescriptorSet(
			Engine::DescriptorSetAllocator& descriptorSetAllocator,
			vk::DescriptorSetLayout& vertexWeightDescriptorSetLayout
		);
		static vk::DescriptorSetLayout createSkinnedVertexRangeDescriptorSetLayout(Engine::DescriptorSetLayoutCache& descriptorSetLayoutCache);
		static vk::DescriptorSet createSkinnedVertexRangeDescriptorSet(
			Engine::DescriptorSetAllocator& descriptorSetAllocator,
			vk::DescriptorSetLayout& skinnedVertexRangeDescriptorSetLayout
		);
	public:
		vk::DescriptorSetLayout vertexBufferDescriptorSetLayout;
		vk::DescriptorSetLayout skinnedVertexBufferDescriptorSetLayout;
		vk::DescriptorSetLayout vertexWeightDescriptorSetLayout;
		vk::DescriptorSetLayout skinnedVertexRangeDescriptorSetLayout;
		vk::DescriptorSet vertexWeightDescriptorSet;
		vk::DescriptorSet vertexBufferDescriptorSet;
		vk::DescriptorSet skinnedVertexBufferDescriptorSet;
		vk::DescriptorSet skinnedVertexRangeDescriptorSet;
		std::vector<AnimationFrameResource> frameResources;
		AnimationResourceManager(
			const Configuration::Configuration& configuration,
			Devices::DeviceContext& deviceContext,
			Engine::DescriptorSetAllocator& descriptorSetAllocator,
			Engine::DescriptorSetLayoutCache& descriptorSetLayoutCache
		);
		template<typename TState>
		std::vector<std::pair<Components::SkinnedBufferView, TState>> createSkinnedMesh(
			const std::vector<TState>& states,
			const std::vector<Devices::BufferView>& sourceBufferViews
		) {
			std::vector<std::pair<Components::SkinnedBufferView, TState>> skinnedBufferViews(sourceBufferViews.size());
			uint32_t sourceBufferViewIndex = 0;
			std::transform(
				sourceBufferViews.begin(),
				sourceBufferViews.end(),
				skinnedBufferViews.data(),
				[&sourceBufferViewIndex, &states, this](const Devices::BufferView& meshBufferView) {
					return std::make_pair(
						Components::SkinnedBufferView{
							meshBufferView,
							std::vector<Devices::BufferView>(configuration.frameCount),
							0,
							std::vector<uint32_t>(configuration.frameCount)
						},
						states[sourceBufferViewIndex++]
					);
				}
			);


			std::vector<Devices::BufferView> vertexBufferViews(skinnedBufferViews.size());
			std::transform(
				skinnedBufferViews.begin(),
				skinnedBufferViews.end(),
				vertexBufferViews.data(),
				[](const auto& skinnedBufferView) {
					return skinnedBufferView.first.bufferView;
				}
			);

			uint32_t skinnedVertexBufferDescriptorSetArrayElementOffset = frameResources[0].skinnedVertexBufferDescriptorSetArrayElementOffset;

			uint32_t writeCount = 0;

			for (auto frameIndex = 0; frameIndex < configuration.frameCount; frameIndex++) {
				std::vector<Devices::BufferView> destinationBufferViews(skinnedBufferViews.size());
				std::vector<Devices::Buffer> destinationBuffers;
				Devices::Device::duplicateBufferViews(
					deviceContext.PhysicalDevice,
					deviceContext.device,
					deviceContext.GraphicQueue,
					deviceContext.CommandPool,
					deviceContext.Allocator,
					vertexBufferViews,
					destinationBufferViews,
					destinationBuffers,
					vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst
				);

				for (uint32_t skinnedBufferViewIndex = 0; skinnedBufferViewIndex < skinnedBufferViews.size(); skinnedBufferViewIndex++) {
					skinnedBufferViews[skinnedBufferViewIndex].first.frameSkinnedBufferViews[frameIndex] = destinationBufferViews[skinnedBufferViewIndex];
				}

				std::sort(
					skinnedBufferViews.begin(),
					skinnedBufferViews.end(),
					[frameIndex](const auto& left, const auto& right) {
						return left.first.frameSkinnedBufferViews[frameIndex].buffer.buffer < right.first.frameSkinnedBufferViews[frameIndex].buffer.buffer;
					}
				);

				vk::Buffer* previousBuffer = nullptr;
				std::vector<vk::DescriptorBufferInfo> descriptorBufferInfos;
				writeCount = 0;

				for (auto& skinnedBufferView : skinnedBufferViews) {
					if (previousBuffer == nullptr || skinnedBufferView.first.frameSkinnedBufferViews[frameIndex].buffer.buffer != *previousBuffer) {
						vk::DescriptorBufferInfo descriptorBufferInfo{
							.buffer = skinnedBufferView.first.frameSkinnedBufferViews[frameIndex].buffer.buffer,
							.offset = 0u,
							.range = VK_WHOLE_SIZE
						};
						descriptorBufferInfos.push_back(descriptorBufferInfo);
						if (previousBuffer != nullptr) writeCount++;
					}
					skinnedBufferView.first.frameSkinnedBufferArrayElements[frameIndex] = skinnedVertexBufferDescriptorSetArrayElementOffset + writeCount;
					previousBuffer = &skinnedBufferView.first.frameSkinnedBufferViews[frameIndex].buffer.buffer;
				}
				vk::WriteDescriptorSet writeDescriptorSet{
					.dstSet = frameResources[frameIndex].skinnedMeshDescriptorSet,
					.dstBinding = 0,
					.dstArrayElement = skinnedVertexBufferDescriptorSetArrayElementOffset,
					.descriptorCount = static_cast<uint32_t>(descriptorBufferInfos.size()),
					.descriptorType = vk::DescriptorType::eStorageBuffer,
					.pImageInfo = VK_NULL_HANDLE,
					.pBufferInfo = descriptorBufferInfos.data(),
					.pTexelBufferView = VK_NULL_HANDLE
				};
				deviceContext.device.updateDescriptorSets(1, &writeDescriptorSet, 0, nullptr);
				buffers.insert(buffers.end(), std::make_move_iterator(destinationBuffers.begin()), std::make_move_iterator(destinationBuffers.end()));
			}

			for (auto& frameResource : frameResources) {
				frameResource.skinnedVertexBufferDescriptorSetArrayElementOffset += writeCount + 1;
			}

			std::vector<vk::DescriptorBufferInfo> descriptorBufferInfos;

			std::sort(
				skinnedBufferViews.begin(),
				skinnedBufferViews.end(),
				[](const auto& left, const auto& right) {
					return left.first.bufferView.buffer.buffer < right.first.bufferView.buffer.buffer;
				}
			);

			vk::Buffer* previousBuffer = nullptr;
			writeCount = 0;

			for (auto& skinnedBufferView : skinnedBufferViews) {
				if (previousBuffer == nullptr || skinnedBufferView.first.bufferView.buffer.buffer != *previousBuffer) {
					vk::DescriptorBufferInfo descriptorBufferInfo{
						.buffer = skinnedBufferView.first.bufferView.buffer.buffer,
						.offset = 0u,
						.range = VK_WHOLE_SIZE
					};
					descriptorBufferInfos.push_back(descriptorBufferInfo);
					if(previousBuffer != nullptr) writeCount++;
				}
				skinnedBufferView.first.bufferArrayElement = vertexBufferDescriptorSetArrayElementOffset + writeCount;
				previousBuffer = &skinnedBufferView.first.bufferView.buffer.buffer;
			}

			vk::WriteDescriptorSet writeDescriptorSet{
				.dstSet = vertexBufferDescriptorSet,
				.dstBinding = 0,
				.dstArrayElement = vertexBufferDescriptorSetArrayElementOffset,
				.descriptorCount = static_cast<uint32_t>(descriptorBufferInfos.size()),
				.descriptorType = vk::DescriptorType::eStorageBuffer,
				.pImageInfo = VK_NULL_HANDLE,
				.pBufferInfo = descriptorBufferInfos.data(),
				.pTexelBufferView = VK_NULL_HANDLE
			};
			deviceContext.device.updateDescriptorSets(1, &writeDescriptorSet, 0, nullptr);

			vertexBufferDescriptorSetArrayElementOffset += writeCount + 1;

			return skinnedBufferViews;
		}

		std::vector<Components::BoneInstanceWeightBufferView> createBoneInstanceWeightBufferViews(
			std::vector<std::span<Models::BoneInstanceWeight>> sourceBoneInstanceWeightSpans
		) {
			const auto& bufferUploadResult = Devices::Device::uploadBuffers(
				deviceContext.PhysicalDevice,
				deviceContext.device,
				deviceContext.GraphicQueue,
				deviceContext.CommandPool,
				deviceContext.Allocator,
				sourceBoneInstanceWeightSpans,
				vk::BufferUsageFlagBits::eStorageBuffer
			);

			buffers.push_back(bufferUploadResult.buffer);

			vk::DescriptorBufferInfo descriptorBufferInfo{
				.buffer = bufferUploadResult.buffer.buffer,
				.offset = 0u,
				.range = VK_WHOLE_SIZE
			};

			vk::WriteDescriptorSet writeDescriptorSet{
				.dstSet = vertexWeightDescriptorSet,
				.dstBinding = 0,
				.dstArrayElement = vertexWeightDescriptorSetArrayElementOffset,
				.descriptorCount = 1,
				.descriptorType = vk::DescriptorType::eStorageBuffer,
				.pImageInfo = VK_NULL_HANDLE,
				.pBufferInfo = &descriptorBufferInfo,
				.pTexelBufferView = VK_NULL_HANDLE
			};
			deviceContext.device.updateDescriptorSets(1, &writeDescriptorSet, 0, nullptr);

			std::vector<Components::BoneInstanceWeightBufferView> vertexWeightBufferViews(bufferUploadResult.bufferViews.size());
			std::transform(
				bufferUploadResult.bufferViews.begin(),
				bufferUploadResult.bufferViews.end(),
				vertexWeightBufferViews.data(),
				[this](auto bufferView) {
					return Components::BoneInstanceWeightBufferView{
						.bufferView = bufferView,
						.bufferIndex = vertexWeightDescriptorSetArrayElementOffset
					};
				}
			);
			vertexWeightDescriptorSetArrayElementOffset++;

			return vertexWeightBufferViews;
		}

		std::vector<Components::SkinnedVertexRangeBufferView> createSkinnedVertexRangeBufferViews(
			std::vector<std::span<Models::SkinnedVertexRange>> sourceSkinnedVertexRangeSpans
		) {
			const auto& bufferUploadResult = Devices::Device::uploadBuffers(
				deviceContext.PhysicalDevice,
				deviceContext.device,
				deviceContext.GraphicQueue,
				deviceContext.CommandPool,
				deviceContext.Allocator,
				sourceSkinnedVertexRangeSpans,
				vk::BufferUsageFlagBits::eStorageBuffer
			);

			buffers.push_back(bufferUploadResult.buffer);

			vk::DescriptorBufferInfo descriptorBufferInfo{
				.buffer = bufferUploadResult.buffer.buffer,
				.offset = 0u,
				.range = VK_WHOLE_SIZE
			};

			vk::WriteDescriptorSet writeDescriptorSet{
				.dstSet = skinnedVertexRangeDescriptorSet,
				.dstBinding = 0,
				.dstArrayElement = skinnedVertexRangeDescriptorSetArrayElementOffset,
				.descriptorCount = 1,
				.descriptorType = vk::DescriptorType::eStorageBuffer,
				.pImageInfo = VK_NULL_HANDLE,
				.pBufferInfo = &descriptorBufferInfo,
				.pTexelBufferView = VK_NULL_HANDLE
			};
			deviceContext.device.updateDescriptorSets(1, &writeDescriptorSet, 0, nullptr);

			std::vector<Components::SkinnedVertexRangeBufferView> skinnedVertexRangeBufferViews(bufferUploadResult.bufferViews.size());
			std::transform(
				bufferUploadResult.bufferViews.begin(),
				bufferUploadResult.bufferViews.end(),
				skinnedVertexRangeBufferViews.data(),
				[this](auto bufferView) {
					return Components::SkinnedVertexRangeBufferView{
						.bufferView = bufferView,
						.bufferIndex = skinnedVertexRangeDescriptorSetArrayElementOffset
					};
				}
			);
			skinnedVertexRangeDescriptorSetArrayElementOffset++;

			return skinnedVertexRangeBufferViews;
		}
	};
}