#include "TextureStore.hpp"

namespace drk::Stores {

	TextureStore::TextureStore(
		const Devices::DeviceContext& deviceContext,
		const vk::DescriptorSet& descriptorSet,
		const vk::Sampler& textureSampler,
		const vk::Sampler& shadowTextureSampler
	) :
		DeviceContext(deviceContext),
		DescriptorSet(descriptorSet),
		TextureSampler(textureSampler),
		shadowTextureSampler(shadowTextureSampler) {

	}

	std::vector<Devices::Texture>
	TextureStore::UploadTextures(std::vector<const Textures::ImageInfo*> imageInfos) {
		const auto textureIndexOffset = Textures.size();
		std::vector<Devices::Texture> uploadedTextures(imageInfos.size());
		std::vector<vk::DescriptorImageInfo> descriptorImageInfos(imageInfos.size());

		for (auto imageInfoIndex = 0u; imageInfoIndex < imageInfos.size(); imageInfoIndex++) {
			const auto& imageInfo = imageInfos[imageInfoIndex];
			VmaAllocationCreateInfo stagingAllocationCreationInfo = {
				.flags = VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
				.usage = VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO,
				.requiredFlags = (VkMemoryPropertyFlags) (vk::MemoryPropertyFlagBits::eHostVisible |
														  vk::MemoryPropertyFlagBits::eHostCoherent),
			};
			const auto imageByteLength = imageInfo->width * imageInfo->height * 4 * sizeof(unsigned char);
			const auto stagingBuffer = DeviceContext.CreateBuffer(
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
				vk::BufferUsageFlagBits::eTransferSrc,
				stagingAllocationCreationInfo,
				imageByteLength, 
				fmt::format("Staging {0}.ImageBuffer", typeid(TextureStore).name()).c_str()
			);

			unsigned char* stagingMemory;
			Devices::Device::mapBuffer(DeviceContext.Allocator, stagingBuffer, (void**) &stagingMemory);

			memcpy(stagingMemory, imageInfo->pixels.data(), imageByteLength);

			const auto textureFormat = Textures::ImageInfo::TextureTypeFormatMap[imageInfo->type];
			//TODO: Use configurable mipLevels
			const auto mipLevels = imageInfo->mipLevels();
			vk::ImageCreateInfo imageCreationInfo = {
				.imageType = vk::ImageType::e2D,
				.format = textureFormat,
				.extent = {
					.width = imageInfo->width,
					.height = imageInfo->height,
					.depth = 1
				},
				.mipLevels = mipLevels,
				.arrayLayers = 1,
				//TODO: Use configurable sample count
				.samples = vk::SampleCountFlagBits::e1,
				.usage = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst |
						 vk::ImageUsageFlagBits::eSampled
			};
			const auto image = DeviceContext.createImage(
				imageCreationInfo,
				vk::MemoryPropertyFlagBits::eDeviceLocal
			);

			vk::BufferImageCopy region = {
				.imageSubresource = {
					.aspectMask = vk::ImageAspectFlagBits::eColor,
					.mipLevel = 0,
					.baseArrayLayer = 0,
					.layerCount = 1,
				},
				.imageOffset = {0, 0, 0},
				.imageExtent = {imageInfo->width, imageInfo->height, 1},
			};

			auto commandBuffer = Devices::Device::beginSingleTimeCommands(
				DeviceContext.device,
				DeviceContext.CommandPool
			);
			Devices::Device::transitionLayout(
				commandBuffer,
				image.image,
				textureFormat,
				vk::ImageLayout::eUndefined,
				vk::ImageLayout::eTransferDstOptimal,
				mipLevels
			);
			Devices::Device::copyBufferToImage(
				commandBuffer,
				stagingBuffer,
				image.image,
				region
			);
			if (mipLevels > 1) {
				Devices::Device::generatedMipmaps(
					commandBuffer,
					image.image,
					imageInfo->width,
					imageInfo->height,
					mipLevels
				);
			} else {
				Devices::Device::transitionLayout(
					commandBuffer,
					image.image,
					textureFormat,
					vk::ImageLayout::eTransferDstOptimal,
					vk::ImageLayout::eShaderReadOnlyOptimal,
					mipLevels
				);
			}

			Devices::Device::endSingleTimeCommands(
				DeviceContext.device,
				DeviceContext.GraphicQueue,
				DeviceContext.CommandPool,
				commandBuffer
			);
			Devices::Device::unmapBuffer(DeviceContext.Allocator, stagingBuffer);
			DeviceContext.DestroyBuffer(stagingBuffer);

			vk::ImageViewCreateInfo imageViewCreateInfo = {
				.image = image.image,
				.viewType = vk::ImageViewType::e2D,
				.format = textureFormat,
				.subresourceRange = {
					.aspectMask = vk::ImageAspectFlagBits::eColor,
					.baseMipLevel = 0,
					.levelCount = mipLevels,
					.baseArrayLayer = 0,
					.layerCount = 1
				}
			};
			const auto imageView = DeviceContext.device.createImageView(imageViewCreateInfo);
			const Devices::Texture texture = {
				.image = image,
				.imageView = imageView,
				.index = static_cast<Common::Index>(textureIndexOffset + imageInfoIndex)
			};
			Textures.push_back(texture);
			uploadedTextures[imageInfoIndex] = texture;
			vk::DescriptorImageInfo descriptorImageInfo = {
				.sampler = TextureSampler,
				.imageView = texture.imageView,
				.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal
			};
			descriptorImageInfos[imageInfoIndex] = descriptorImageInfo;
		}

		vk::WriteDescriptorSet write = {
			.dstSet = DescriptorSet,
			.dstBinding = 0,
			.dstArrayElement = static_cast<uint32_t>(textureIndexOffset),
			.descriptorCount = static_cast<uint32_t>(descriptorImageInfos.size()),
			.descriptorType = vk::DescriptorType::eCombinedImageSampler,
			.pImageInfo = descriptorImageInfos.data(),
			.pBufferInfo = nullptr,
		};

		DeviceContext.device.updateDescriptorSets(std::array<vk::WriteDescriptorSet, 1>{write}, {});

		return uploadedTextures;
	}

	void TextureStore::registerTextures(Devices::Texture* pTextures, uint32_t textureCount) {
		const auto textureIndexOffset = static_cast<uint32_t>(Textures.size());
		std::vector<vk::DescriptorImageInfo> descriptorImageInfos(textureCount);
		for (auto textureIndex = 0u; textureIndex < textureCount; textureIndex++) {
			auto& texture = pTextures[textureIndex];
			Textures.push_back(texture);
			vk::DescriptorImageInfo descriptorImageInfo = {
				.sampler = shadowTextureSampler,
				.imageView = texture.imageView,
				.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal
			};
			texture.index = textureIndex + textureIndexOffset;
			descriptorImageInfos[textureIndex] = descriptorImageInfo;
		}
		vk::WriteDescriptorSet write = {
			.dstSet = DescriptorSet,
			.dstBinding = 0,
			.dstArrayElement = static_cast<uint32_t>(textureIndexOffset),
			.descriptorCount = static_cast<uint32_t>(textureCount),
			.descriptorType = vk::DescriptorType::eCombinedImageSampler,
			.pImageInfo = descriptorImageInfos.data(),
			.pBufferInfo = nullptr,
		};

		DeviceContext.device.updateDescriptorSets(std::array<vk::WriteDescriptorSet, 1>{write}, {});
	}
	void TextureStore::registerTexture(Devices::Texture& texture) {
		registerTextures(&texture, 1);
	}
}