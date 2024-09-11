#pragma once

#include <unordered_map>
#include <vector>
#include <typeindex>
#include "../Devices/Buffer.hpp"
#include "../Devices/Texture.hpp"
#include "FrameState.hpp"
#include "../Common/IndexGenerator.hpp"
#include "DescriptorSetLayoutCache.hpp"
#include "DescriptorSetAllocator.hpp"
#include "../Meshes/Components/MeshResource.hpp"
#include "../Graphics/SynchronizationState.hpp"
#include "../Devices/Device.hpp"
#include <entt/entt.hpp>
#include <functional>
#include <memory>
#include "../Stores/StoreItem.hpp"
#include "../Stores/TextureStore.hpp"
#include "../Devices/DeviceContext.hpp"
#include "DescriptorSetLayoutCache.hpp"
#include "DescriptorSetLayouts.hpp"
#include <fmt/chrono.h>

#define FRAME_COUNT 2u

namespace drk::Engine {
	class EngineState {
	protected:
		const Devices::DeviceContext& deviceContext;
		entt::registry& registry;
		DescriptorSetLayoutCache& descriptorSetLayoutCache;
		DescriptorSetAllocator& descriptorSetAllocator;
		std::chrono::high_resolution_clock::time_point startedOn = std::chrono::high_resolution_clock::now();
		const DescriptorSetLayouts& descriptorSetLayouts;
		const vk::Sampler textureSampler;
		const vk::Sampler shadowTextureSampler;
		uint32_t frameCount = FRAME_COUNT;
		uint32_t frameIndex = 0;

		static vk::Sampler CreateTextureSampler(const Devices::DeviceContext& deviceContext);
		static vk::Sampler CreateShadowTextureSampler(const Devices::DeviceContext& deviceContext);
		static vk::DescriptorSet CreateTextureDescriptorSet(
			DescriptorSetAllocator& descriptorSetAllocator,
			vk::DescriptorSetLayout descriptorSetLayout
		);
	public:

		std::vector<FrameState> frameStates;
		const vk::DescriptorSet textureDescriptorSet;
		std::unique_ptr<Stores::TextureStore> textureStore;
		Common::IndexGenerator<uint32_t> IndexGenerator;
		entt::entity cameraEntity = entt::null;
		vk::DescriptorPool imGuiDescriptorPool;

		EngineState(
			const Devices::DeviceContext& deviceContext,
			entt::registry& registry,
			DescriptorSetLayoutCache& descriptorSetLayoutCache,
			DescriptorSetAllocator& descriptorSetAllocator,
			const DescriptorSetLayouts& descriptorSetLayouts
		);
		~EngineState();

		uint32_t getFrameIndex() const;
		uint32_t getFrameCount() const;
		double getTime() const;
		std::chrono::high_resolution_clock::duration getDuration() const;
		FrameState& getCurrentFrameState() const;
		void incrementFrameIndex();

		Devices::Texture UploadTexture(const Textures::ImageInfo& imageInfo);
		std::vector<Devices::Texture> UploadTextures(std::vector<const Textures::ImageInfo*> imageInfos);

		template<typename T>
		Stores::StoreItem<T> GetStoreItem() {
			std::vector<Stores::StoreItemLocation<T>> frameStoreItems(frameStates.size());
			for (auto frameStateIndex = 0u; frameStateIndex < frameStates.size(); frameStateIndex++) {
				auto storeItemLocation = frameStates[frameStateIndex].AddStoreItem<T>();
				frameStoreItems[frameStateIndex] = storeItemLocation;
			}
			Stores::StoreItem<T> storeItem = {
				.frameStoreItems = std::move(frameStoreItems)
			};

			return storeItem;
		}

		template<typename TModel, typename ...TComponents>
		void Store() {
			auto entities = registry.view<TComponents...>(entt::exclude<Stores::StoreItem<TModel>>);
			for (const auto entity: entities) {
				auto storeItem = GetStoreItem<TModel>();
				registry.emplace<Stores::StoreItem<TModel>>(
					entity,
					storeItem
				);
				registry.emplace<Graphics::SynchronizationState<TModel>>(entity, frameStates.size());
			}
		}
		vk::Sampler GetDefaultTextureSampler() const;
		void CreateImguiResources() {
			vk::DescriptorPoolSize poolSizes[] =
				{
					{vk::DescriptorType::eSampler,              1000},
					{vk::DescriptorType::eCombinedImageSampler, 1000},
					{vk::DescriptorType::eSampledImage,         1000},
					{vk::DescriptorType::eStorageImage,         1000},
					{vk::DescriptorType::eUniformTexelBuffer,   1000},
					{vk::DescriptorType::eStorageTexelBuffer,   1000},
					{vk::DescriptorType::eUniformBuffer,        1000},
					{vk::DescriptorType::eStorageBuffer,        1000},
					{vk::DescriptorType::eUniformBufferDynamic, 1000},
					{vk::DescriptorType::eStorageBufferDynamic, 1000},
					{vk::DescriptorType::eInputAttachment,      1000}
				};

			vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo = {
				.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
				.maxSets = 1000u,
				.poolSizeCount = (uint32_t) std::size(poolSizes),
				.pPoolSizes = poolSizes,
			};

			imGuiDescriptorPool = deviceContext.device.createDescriptorPool(descriptorPoolCreateInfo);
		}
	};
}