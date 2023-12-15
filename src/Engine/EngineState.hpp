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
#include "MeshUploadResult.hpp"
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

#define FRAME_COUNT 2u

namespace drk::Engine {
	class EngineState {
	protected:
		const Devices::DeviceContext& deviceContext;
		entt::registry& registry;
		DescriptorSetLayoutCache& descriptorSetLayoutCache;
		DescriptorSetAllocator& descriptorSetAllocator;
		const DescriptorSetLayouts& descriptorSetLayouts;
		const vk::Sampler textureSampler;
		uint32_t frameCount = FRAME_COUNT;
		uint32_t frameIndex = 0;
		std::vector<Devices::Buffer> buffers;

		static vk::Sampler CreateTextureSampler(const Devices::DeviceContext& deviceContext);
		static vk::DescriptorSet CreateTextureDescriptorSet(
			DescriptorSetAllocator& descriptorSetAllocator,
			vk::DescriptorSetLayout descriptorSetLayout
		);
	public:

		std::vector<FrameState> frameStates;
		const vk::DescriptorSet textureDescriptorSet;
		std::unique_ptr<Stores::TextureStore> textureStore;
		Common::IndexGenerator<uint32_t> IndexGenerator;
		entt::entity CameraEntity = entt::null;

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
		FrameState& getCurrentFrameState() const;
		void incrementFrameIndex();

		Devices::Texture UploadTexture(const Textures::ImageInfo* const imageInfo);
		std::vector<Devices::Texture> UploadTextures(std::vector<const Textures::ImageInfo*> imageInfos);
		MeshUploadResult UploadMeshes(const std::vector<std::shared_ptr<Meshes::Components::MeshResource>>& meshInfos);

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
			for (const auto entity : entities) {
				auto storeItem = GetStoreItem<TModel>();
				registry.emplace<Stores::StoreItem<TModel>>(
					entity,
					storeItem
				);
				registry.emplace<Graphics::SynchronizationState<TModel>>(entity, frameStates.size());
			}
		}
		vk::Sampler GetDefaultTextureSampler() const;

	};
}