#pragma once

#include <vulkan/vulkan.hpp>
#include "../../Meshes/Components/MeshBufferView.hpp"
#include "../Components/SkinnedBufferView.hpp"
#include "../Components/VertexWeightBufferView.hpp"
#include "../../Engine/EngineState.hpp"
#include "../../Resources/ResourceManager.hpp"
#include "../../Engine/DescriptorSetAllocator.hpp"
#include "../../Engine/DescriptorSetLayoutCache.hpp"
#include "../Components/VertexWeight.hpp"
#include <vector>
#include <utility>

namespace drk::Animations::Resources {
	class AnimationResourceManager : public drk::Resources::ResourceManager {
	protected:
		Devices::DeviceContext& deviceContext;
		std::vector<Devices::Buffer> buffers;
		uint32_t skinnedMeshDescriptorSetArrayElementOffset;
		uint32_t vertexWeightDescriptorSetArrayElementOffset;
		Engine::DescriptorSetAllocator& descriptorSetAllocator;
		Engine::DescriptorSetLayoutCache& descriptorSetLayoutCache;
		vk::DescriptorSetLayout skinnedMeshDescriptorSetLayout;
		vk::DescriptorSet skinnedMeshDescriptorSet;
		vk::DescriptorSetLayout vertexWeightDescriptorSetLayout;
		vk::DescriptorSet vertexWeightDescriptorSet;
		static vk::DescriptorSetLayout createSkinnedMeshDescriptorSetLayout(Engine::DescriptorSetLayoutCache& descriptorSetLayoutCache);
		static vk::DescriptorSet createSkinnedMeshDescriptorSet(
			Engine::DescriptorSetAllocator& descriptorSetAllocator,
			vk::DescriptorSetLayout& skinnedMeshDescriptorSetLayoutCache
		);
		static vk::DescriptorSetLayout createVertexWeightDescriptorSetLayout(Engine::DescriptorSetLayoutCache& descriptorSetLayoutCache);
		static vk::DescriptorSet createVertexWeightDescriptorSet(
			Engine::DescriptorSetAllocator& descriptorSetAllocator,
			vk::DescriptorSetLayout& vertexWeightDescriptorSetLayoutCache
		);
	public:
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
				[&sourceBufferViewIndex, &states](const Devices::BufferView& meshBufferView) {
					return std::make_pair(
						Components::SkinnedBufferView{
							meshBufferView,
							{},
							0,
							0
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
				skinnedBufferViews[skinnedBufferViewIndex].first.skinnedBufferView = destinationBufferViews[skinnedBufferViewIndex];
			}


			uint32_t descriptorSetArrayElementOffset = buffers.size();
			uint32_t arrayElement = descriptorSetArrayElementOffset;
			std::vector<vk::DescriptorBufferInfo> descriptorBufferInfos;

			std::sort(
				skinnedBufferViews.begin(),
				skinnedBufferViews.end(),
				[](const auto& left, const auto& right) {
					return left.first.bufferView.buffer.buffer < right.first.bufferView.buffer.buffer;
				}
			);

			vk::Buffer* previousBuffer = nullptr;
			for (auto& skinnedBufferView : skinnedBufferViews) {
				if (&skinnedBufferView.first.bufferView.buffer.buffer != previousBuffer) {
					vk::DescriptorBufferInfo descriptorBufferInfo{
						.buffer = skinnedBufferView.first.bufferView.buffer.buffer,
						.offset = 0u,
						.range = VK_WHOLE_SIZE
					};
					skinnedBufferView.first.bufferArrayElement = arrayElement++;
					descriptorBufferInfos.push_back(descriptorBufferInfo);
				}

				previousBuffer = &skinnedBufferView.first.bufferView.buffer.buffer;
			}

			std::sort(
				skinnedBufferViews.begin(),
				skinnedBufferViews.end(),
				[](const auto& left, const auto& right) {
					return left.first.skinnedBufferView.buffer.buffer < right.first.skinnedBufferView.buffer.buffer;
				}
			);

			previousBuffer = nullptr;
			for (auto& skinnedBufferView : skinnedBufferViews) {
				if (&skinnedBufferView.first.skinnedBufferView.buffer.buffer != previousBuffer) {
					vk::DescriptorBufferInfo descriptorBufferInfo{
						.buffer = skinnedBufferView.first.skinnedBufferView.buffer.buffer,
						.offset = 0u,
						.range = VK_WHOLE_SIZE
					};
					skinnedBufferView.first.skinnedBufferArrayElement = arrayElement++;
					descriptorBufferInfos.push_back(descriptorBufferInfo);
				}

				previousBuffer = &skinnedBufferView.first.skinnedBufferView.buffer.buffer;
			}

			vk::WriteDescriptorSet writeDescriptorSet{
				.dstSet = skinnedMeshDescriptorSet,
				.dstBinding = 0,
				.dstArrayElement = descriptorSetArrayElementOffset,
				.descriptorCount = static_cast<uint32_t>(descriptorBufferInfos.size()),
				.descriptorType = vk::DescriptorType::eStorageBuffer,
				.pImageInfo = VK_NULL_HANDLE,
				.pBufferInfo = descriptorBufferInfos.data(),
				.pTexelBufferView = VK_NULL_HANDLE
			};
			deviceContext.device.updateDescriptorSets(1, &writeDescriptorSet, 0, nullptr);
			descriptorSetArrayElementOffset += descriptorBufferInfos.size();
			buffers.insert(buffers.end(), std::make_move_iterator(destinationBuffers.begin()), std::make_move_iterator(destinationBuffers.end()));

			return skinnedBufferViews;
		}

		std::vector<Components::VertexWeightBufferView> createVertexWeightBuffers(std::vector<std::vector<Components::VertexWeight>> sourceVertexWeightBuffers) {
			std::vector<std::span<Components::VertexWeight>> sourceVertexWeightBufferSpans(sourceVertexWeightBuffers.size());
			std::transform(
				sourceVertexWeightBuffers.begin(),
				sourceVertexWeightBuffers.end(),
				sourceVertexWeightBufferSpans.data(),
				[](auto& sourceVertexWeightBuffer) {
					return std::span(sourceVertexWeightBuffer.begin(), sourceVertexWeightBuffer.end());
				}
			);
			const auto& bufferUploadResult = Devices::Device::uploadBuffers(
				deviceContext.PhysicalDevice,
				deviceContext.device,
				deviceContext.GraphicQueue,
				deviceContext.CommandPool,
				deviceContext.Allocator,
				sourceVertexWeightBufferSpans,
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

			std::vector<Components::VertexWeightBufferView> vertexWeightBufferViews(bufferUploadResult.bufferViews.size());
			std::transform(
				bufferUploadResult.bufferViews.begin(),
				bufferUploadResult.bufferViews.end(),
				vertexWeightBufferViews.data(),
				[this](auto& bufferView) {
					return Components::VertexWeightBufferView{
						.bufferView = std::move(bufferView),
						.bufferIndex = vertexWeightDescriptorSetArrayElementOffset
					};
				}
			);
			vertexWeightDescriptorSetArrayElementOffset++;

			return vertexWeightBufferViews;
		}
	};
}