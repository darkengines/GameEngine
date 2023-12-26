#pragma once

#include "../Devices/DeviceContext.hpp"
#include "../Devices/Device.hpp"

namespace drk::Stores {
	class TextureStore {
	protected:
		const Devices::DeviceContext& DeviceContext;
		std::vector<Devices::Texture> Textures;
		vk::DescriptorSet DescriptorSet;
		vk::Sampler TextureSampler;
		vk::Sampler shadowTextureSampler;

	public:
		TextureStore(
			const Devices::DeviceContext& deviceContext,
			const vk::DescriptorSet& descriptorSet,
			const vk::Sampler& textureSampler,
			const vk::Sampler& shadowTextureSampler
		);

		~TextureStore() {
			for (const auto& texture : Textures) {
				DeviceContext.destroyTexture(texture);
			}
		}

		std::vector<Devices::Texture> UploadTextures(std::vector<const Textures::ImageInfo*> imageInfos);
		void registerTextures(Devices::Texture* pTextures, uint32_t textureCount);
		void registerTexture(Devices::Texture& texture);
	};
}