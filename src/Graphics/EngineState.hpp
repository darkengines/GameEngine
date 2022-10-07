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
#include "StoreItem.hpp"

namespace drk::Graphics {
	class EngineState {
	protected:
		const Devices::DeviceContext *DeviceContext;
		const vk::Sampler TextureSampler;
		std::unique_ptr<DescriptorSetLayoutCache> DescriptorSetLayoutCache;
		std::unique_ptr<DescriptorSetAllocator> DescriptorSetAllocator;
		static vk::Sampler CreateTextureSampler(const Devices::DeviceContext *const deviceContext);
		static vk::DescriptorPool CreateDescriptorPool(const Devices::DeviceContext *const deviceContext);
		vk::DescriptorSet CreateTextureDescriptorSet();
	public:
		EngineState(const Devices::DeviceContext *deviceContext);
		~EngineState();
		uint32_t FrameIndex = 0;
		std::unordered_map<std::type_index, GenericStoreBuffer> Stores;
		std::vector<FrameState> FrameStates;
		std::vector<Devices::Buffer> Buffers;
		std::vector<Devices::Texture> Textures;
		vk::DescriptorSetLayout TextureDescriptorSetLayout;
		vk::DescriptorSet TextureDescriptorSet;
		Common::IndexGenerator<uint32_t> IndexGenerator;
		entt::registry Registry;
		Devices::Texture UploadTexture(const Textures::ImageInfo *const imageInfo, Common::Index index);
		MeshUploadResult UploadMeshes(const std::vector<Meshes::MeshInfo *> &meshInfos);

		template<typename T>
		StoreItem<T> Store() {
			std::vector<StoreItemLocation<T>> frameStoreItems(FrameStates.size());
			for (auto frameStateIndex = 0u; frameStateIndex < FrameStates.size(); frameStateIndex++) {
				auto storeItemLocation = FrameStates[frameStateIndex].AddStoreItem<T>();
				frameStoreItems[frameStateIndex] = storeItemLocation;
			}
			StoreItem<T> storeItem = {
				.frameStoreItems = std::move(frameStoreItems)
			};

			return storeItem;
		}
	};
}