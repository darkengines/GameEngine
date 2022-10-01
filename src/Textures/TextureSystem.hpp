#pragma once
#define VULKAN_HPP_NO_CONSTRUCTORS

#include <vulkan/vulkan.hpp>
#include "../Graphics/EngineState.hpp"
#include "../Devices/Device.hpp"

namespace drk::Textures {
	class TextureSystem {
	protected:
		Devices::DeviceContext *DeviceContext;
		Graphics::EngineState *EngineState;

	public:
		void UploadTextures() {
			auto imageEntities = EngineState->Registry.view<Textures::ImageInfo *>(entt::exclude<Devices::Texture>);
			for (const auto &imageEntity: imageEntities) {
				const auto &imageInfo = EngineState->Registry.get<Textures::ImageInfo *>(imageEntity);

				const auto stagingBuffer = DeviceContext->CreateBuffer(
					vk::MemoryPropertyFlagBits::eHostVisible,
					vk::BufferUsageFlagBits::eTransferSrc,
					imageInfo->width * imageInfo->height * 4 * sizeof(unsigned char));

				unsigned char *stagingMemory;
				Devices::Device::mapBuffer(DeviceContext->Allocator, stagingBuffer, &stagingMemory);
				const auto textureFormat = Textures::ImageInfo::TextureTypeFormatMap[imageInfo->type];
				//TODO: Use configurable mipLevels
				const auto mipLevels = 1u;
				vk::ImageCreateInfo imageCreationInfo = {
					.imageType = vk::ImageType::e2D,
					.format = textureFormat,
					.extent = {
						.width = imageInfo->width,
						.height = imageInfo->height,
						.depth = imageInfo->depth
					},
					.mipLevels = mipLevels,
					.arrayLayers = 1,
					//TODO: Use configurable sample count
					.samples = vk::SampleCountFlagBits::e8,
					.usage = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst |
							 vk::ImageUsageFlagBits::eSampled
				};
				const auto image = DeviceContext->CreateImage(
					imageCreationInfo,
					vk::MemoryPropertyFlagBits::eDeviceLocal
				);

				vk::BufferImageCopy region = {
					.imageSubresource = {
						.aspectMask = vk::ImageAspectFlagBits::eColor,
						.mipLevel = mipLevels,
						.baseArrayLayer = 0,
						.layerCount = 1,
					},
					.imageOffset = {0, 0, 0},
					.imageExtent = {imageInfo->width, imageInfo->height, 1},
				};

				auto commandBuffer = Devices::Device::beginSingleTimeCommands(
					DeviceContext->Device,
					DeviceContext->CommandPool
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
				Devices::Device::unmapBuffer(DeviceContext->Allocator, stagingBuffer);
				DeviceContext->DestroyBuffer(stagingBuffer);

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
				const auto imageView = DeviceContext->Device.createImageView(imageViewCreateInfo);
				const Devices::Texture texture = {
					.image = image,
					.imageView = imageView
				};
				EngineState->Textures.push_back(texture);
			}
		}
	};
}