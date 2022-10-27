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
#include "../Meshes/MeshInfo.hpp"
#include "MeshUploadResult.hpp"
#include "SynchronizationState.hpp"
#include "../Devices/Device.hpp"
#include <entt/entt.hpp>
#include <functional>
#include <memory>
#include "../Stores/StoreItem.hpp"
#include "../Stores/TextureStore.hpp"
#include "../Devices/DeviceContext.hpp"

namespace drk::Graphics {
	class EngineState {
	protected:
		const Devices::DeviceContext& DeviceContext;
		const vk::Sampler TextureSampler;
		std::unique_ptr<DescriptorSetLayoutCache> DescriptorSetLayoutCache;
		std::unique_ptr<DescriptorSetAllocator> DescriptorSetAllocator;

		static vk::Sampler CreateTextureSampler(const Devices::DeviceContext& deviceContext);
		static vk::DescriptorSetLayout
		CreateStorageBufferDescriptorSetLayout(Graphics::DescriptorSetLayoutCache* const descriptorSetLayoutCache);
		static vk::DescriptorSetLayout
		CreateGlobalUniformBufferDescriptorSetLayout(Graphics::DescriptorSetLayoutCache* const descriptorSetLayoutCache);
		void CreateTextureDescriptorSet();
	public:
		EngineState(const Devices::DeviceContext& deviceContext, entt::registry& registry);
		~EngineState();
		uint32_t FrameIndex = 0;
		std::vector<FrameState> FrameStates;
		std::vector<Devices::Buffer> Buffers;
		std::vector<vk::DescriptorSetLayout> DescriptorSetLayouts;
		vk::DescriptorSetLayout TextureDescriptorSetLayout;
		vk::DescriptorSetLayout StorageBufferDescriptorSetLayout;
		vk::DescriptorSetLayout DrawStorageBufferDescriptorSetLayout;
		vk::DescriptorSetLayout GlobalUniformBufferDescriptorSetLayout;
		vk::DescriptorSet TextureDescriptorSet;
		entt::registry& Registry;
		std::unique_ptr<Stores::TextureStore> TextureStore;
		Common::IndexGenerator<uint32_t> IndexGenerator;
		entt::entity CameraEntity = entt::null;
		Devices::Texture UploadTexture(const Textures::ImageInfo* const imageInfo);
		std::vector<Devices::Texture> UploadTextures(std::vector<const Textures::ImageInfo*> imageInfos);
		MeshUploadResult UploadMeshes(const std::vector<Meshes::MeshInfo*>& meshInfos);

		template<typename T>
		Stores::StoreItem<T> GetStoreItem() {
			std::vector<Stores::StoreItemLocation<T>> frameStoreItems(FrameStates.size());
			for (auto frameStateIndex = 0u; frameStateIndex < FrameStates.size(); frameStateIndex++) {
				auto storeItemLocation = FrameStates[frameStateIndex].AddStoreItem<T>();
				frameStoreItems[frameStateIndex] = storeItemLocation;
			}
			Stores::StoreItem<T> storeItem = {
				.frameStoreItems = std::move(frameStoreItems)
			};

			return storeItem;
		}

		template<typename TModel, typename ...TComponents>
		void Store() {
			auto entities = Registry.view<TComponents...>(entt::exclude<Stores::StoreItem<TModel>>);
			for (const auto entity : entities) {
				auto storeItem = GetStoreItem<TModel>();
				Registry.emplace<Stores::StoreItem<TModel>>(
					entity,
					storeItem
				);
				Registry.emplace<Graphics::SynchronizationState<TModel>>(entity, FrameStates.size());
			}
		}
		vk::Sampler GetDefaultTextureSampler() const;
	};
}