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

	public:
		TextureStore(
			const Devices::DeviceContext& deviceContext,
			const vk::DescriptorSet& descriptorSet,
			const vk::Sampler& textureSampler
		);

		~TextureStore() {
			for (const auto& texture: Textures) {
				DeviceContext.DestroyTexture(texture);
			}
		}

		std::vector<Devices::Texture> UploadTextures(std::vector<const Textures::ImageInfo*> imageInfos);
	};
}