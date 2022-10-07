#define VULKAN_HPP_NO_CONSTRUCTORS

#include <vulkan/vulkan.hpp>
#include "EngineState.hpp"
#include "../Devices/Device.hpp"
#include <utility>
#include <algorithm>
#include <span>

namespace drk::Graphics {
	EngineState::EngineState(const Devices::DeviceContext *deviceContext) :
		DeviceContext(deviceContext), Registry(entt::registry()), TextureSampler(CreateTextureSampler(DeviceContext)),
		DescriptorSetLayoutCache(std::make_unique<Graphics::DescriptorSetLayoutCache>(DeviceContext->Device)),
		DescriptorSetAllocator(std::make_unique<Graphics::DescriptorSetAllocator>(DeviceContext->Device)) {
		FrameStates.push_back(
			std::move<Graphics::FrameState>(
				{
					DeviceContext,
					DescriptorSetLayoutCache.get(),
					DescriptorSetAllocator.get()
				}
			));
		FrameStates.push_back(
			std::move<Graphics::FrameState>(
				{
					DeviceContext,
					DescriptorSetLayoutCache.get(),
					DescriptorSetAllocator.get()
				}
			));
		CreateTextureDescriptorSet();
	}

	Devices::Texture EngineState::UploadTexture(const Textures::ImageInfo *const imageInfo, Common::Index index) {
		VmaAllocationCreateInfo stagingAllocationCreationInfo = {
			.flags = VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
			.usage = VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO,
			.requiredFlags = (VkMemoryPropertyFlags) (vk::MemoryPropertyFlagBits::eHostVisible |
													  vk::MemoryPropertyFlagBits::eHostCoherent),
		};
		const auto stagingBuffer = DeviceContext->CreateBuffer(
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
			vk::BufferUsageFlagBits::eTransferSrc,
			stagingAllocationCreationInfo,
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
				.depth = 1
			},
			.mipLevels = mipLevels,
			.arrayLayers = 1,
			//TODO: Use configurable sample count
			.samples = vk::SampleCountFlagBits::e1,
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
				.mipLevel = 0,
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

		Devices::Device::endSingleTimeCommands(
			DeviceContext->Device,
			DeviceContext->GraphicQueue,
			DeviceContext->CommandPool,
			commandBuffer
		);
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
		Textures.push_back(texture);

		vk::DescriptorImageInfo descriptorImageInfo = {
			.sampler = TextureSampler,
			.imageView = texture.imageView,
			.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal
		};

		vk::WriteDescriptorSet write = {
			.dstSet = TextureDescriptorSet,
			.dstBinding = 0,
			.dstArrayElement = index,
			.descriptorCount = 1,
			.descriptorType = vk::DescriptorType::eCombinedImageSampler,
			.pImageInfo = &descriptorImageInfo,
			.pBufferInfo = nullptr,
		};

		DeviceContext->Device.updateDescriptorSets(std::array<vk::WriteDescriptorSet, 1>{write}, {});

		return texture;
	}

	vk::Sampler EngineState::CreateTextureSampler(const Devices::DeviceContext *const deviceContext) {
		vk::SamplerCreateInfo samplerCreateInfo{
			.magFilter = vk::Filter::eLinear,
			.minFilter = vk::Filter::eLinear,
			.mipmapMode = vk::SamplerMipmapMode::eLinear,
			.addressModeU = vk::SamplerAddressMode::eRepeat,
			.addressModeV = vk::SamplerAddressMode::eRepeat,
			.addressModeW = vk::SamplerAddressMode::eRepeat,
			.mipLodBias = 0,
			//TODO: Make Anisotropy configurable
			.anisotropyEnable = VK_TRUE,
			.maxAnisotropy = 16,
			.compareEnable = VK_FALSE,
			.compareOp = vk::CompareOp::eAlways,
			.minLod = 0,
			.maxLod = 16,
			.borderColor = vk::BorderColor::eIntOpaqueBlack,
			.unnormalizedCoordinates = VK_FALSE
		};
		const auto sampler = deviceContext->Device.createSampler(samplerCreateInfo);
		return sampler;
	}

	EngineState::~EngineState() {
		for (const auto texture: Textures) {
			DeviceContext->DestroyTexture(texture);
		}
		for (const auto buffer: Buffers) {
			DeviceContext->DestroyBuffer(buffer);
		}
		DeviceContext->Device.destroySampler(TextureSampler);
	}

	vk::DescriptorSet EngineState::CreateTextureDescriptorSet() {
		vk::DescriptorSetLayoutBinding binding = {
			.binding = 0,
			.descriptorType = vk::DescriptorType::eCombinedImageSampler,
			//TODO: make descriptorCount configurable
			.descriptorCount = 2048,
			.stageFlags = vk::ShaderStageFlagBits::eFragment
		};
		vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
			.bindingCount = 1,
			.pBindings = &binding,
		};

		TextureDescriptorSetLayout = DescriptorSetLayoutCache->get(descriptorSetLayoutCreateInfo);
		TextureDescriptorSet = DescriptorSetAllocator->AllocateDescriptorSets({TextureDescriptorSetLayout})[0];
	}

	MeshUploadResult EngineState::UploadMeshes(const std::vector<Meshes::MeshInfo *> &meshInfos) {
		std::vector<std::span<Meshes::Vertex>> vertices(meshInfos.size());
		std::vector<std::span<Meshes::VertexIndex>> indices(meshInfos.size());
		std::transform(
			meshInfos.begin(), meshInfos.end(), vertices.data(), [](Meshes::MeshInfo *mesh) {
				return std::span{mesh->vertices.data(), mesh->vertices.size()};
			}
		);
		std::transform(
			meshInfos.begin(), meshInfos.end(), indices.data(), [](Meshes::MeshInfo *mesh) {
				return std::span{mesh->indices.data(), mesh->indices.size()};
			}
		);
		auto vertexBufferUploadResult = Devices::Device::uploadBuffers(
			DeviceContext->PhysicalDevice,
			DeviceContext->Device,
			DeviceContext->GraphicQueue,
			DeviceContext->CommandPool,
			DeviceContext->Allocator,
			vertices,
			vk::BufferUsageFlagBits::eVertexBuffer
		);
		auto indexBufferUploadResult = Devices::Device::uploadBuffers(
			DeviceContext->PhysicalDevice,
			DeviceContext->Device,
			DeviceContext->GraphicQueue,
			DeviceContext->CommandPool,
			DeviceContext->Allocator,
			indices,
			vk::BufferUsageFlagBits::eIndexBuffer
		);

		MeshUploadResult result = {
			.indexBuffer = indexBufferUploadResult.buffer,
			.vertexBuffer = vertexBufferUploadResult.buffer
		};

		for (auto meshInfoIndex = 0u; meshInfoIndex < meshInfos.size(); meshInfoIndex++) {
			result.meshes.push_back(
				Meshes::Mesh{
					.IndexBufferView = indexBufferUploadResult.bufferViews[meshInfoIndex],
					.VertexBufferView = vertexBufferUploadResult.bufferViews[meshInfoIndex],
				}
			);
		}

		return result;
	}
}
