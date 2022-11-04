#define VULKAN_HPP_NO_CONSTRUCTORS

#include <vulkan/vulkan.hpp>
#include "EngineState.hpp"
#include <utility>
#include <algorithm>
#include <span>

namespace drk::Engine {
	EngineState::EngineState(
		const Devices::DeviceContext& deviceContext,
		entt::registry& registry,
		DescriptorSetLayoutCache& descriptorSetLayoutCache,
		DescriptorSetAllocator& descriptorSetAllocator,
		const DescriptorSetLayouts& descriptorSetLayouts
	) :
		deviceContext(deviceContext),
		descriptorSetLayoutCache(descriptorSetLayoutCache),
		registry(registry),
		textureSampler(CreateTextureSampler(deviceContext)),
		descriptorSetAllocator(descriptorSetAllocator),
		descriptorSetLayouts(descriptorSetLayouts),
		textureDescriptorSet(
			CreateTextureDescriptorSet(
				descriptorSetAllocator,
				descriptorSetLayouts.textureDescriptorSetLayout
			)) {
		frameStates.reserve(frameCount);

		frameStates.emplace_back(deviceContext, descriptorSetLayouts, descriptorSetAllocator);
		frameStates.emplace_back(deviceContext, descriptorSetLayouts, descriptorSetAllocator);

		textureStore = std::make_unique<Stores::TextureStore>(deviceContext, textureDescriptorSet, textureSampler);
	}

	Devices::Texture EngineState::UploadTexture(const Textures::ImageInfo* const imageInfo) {
		return textureStore->UploadTextures({imageInfo})[0];
	}

	std::vector<Devices::Texture> EngineState::UploadTextures(std::vector<const Textures::ImageInfo*> imageInfos) {
		return textureStore->UploadTextures(imageInfos);
	}

	vk::Sampler EngineState::CreateTextureSampler(const Devices::DeviceContext& deviceContext) {
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
		const auto sampler = deviceContext.device.createSampler(samplerCreateInfo);
		return sampler;
	}

	EngineState::~EngineState() {
		for (const auto buffer: buffers) {
			deviceContext.DestroyBuffer(buffer);
		}
		deviceContext.device.destroySampler(textureSampler);
	}

	MeshUploadResult EngineState::UploadMeshes(const std::vector<Meshes::MeshInfo*>& meshInfos) {
		std::vector<std::span<Meshes::Vertex>> vertices(meshInfos.size());
		std::vector<std::span<Meshes::VertexIndex>> indices(meshInfos.size());
		std::transform(
			meshInfos.begin(), meshInfos.end(), vertices.data(), [](Meshes::MeshInfo* mesh) {
				return std::span{mesh->vertices.data(), mesh->vertices.size()};
			}
		);
		std::transform(
			meshInfos.begin(), meshInfos.end(), indices.data(), [](Meshes::MeshInfo* mesh) {
				return std::span{mesh->indices.data(), mesh->indices.size()};
			}
		);
		auto vertexBufferUploadResult = Devices::Device::uploadBuffers(
			deviceContext.PhysicalDevice,
			deviceContext.device,
			deviceContext.GraphicQueue,
			deviceContext.CommandPool,
			deviceContext.Allocator,
			vertices,
			vk::BufferUsageFlagBits::eVertexBuffer
		);
		auto indexBufferUploadResult = Devices::Device::uploadBuffers(
			deviceContext.PhysicalDevice,
			deviceContext.device,
			deviceContext.GraphicQueue,
			deviceContext.CommandPool,
			deviceContext.Allocator,
			indices,
			vk::BufferUsageFlagBits::eIndexBuffer
		);

		MeshUploadResult result = {
			.indexBuffer = indexBufferUploadResult.buffer,
			.vertexBuffer = vertexBufferUploadResult.buffer
		};

		buffers.push_back(indexBufferUploadResult.buffer);
		buffers.push_back(vertexBufferUploadResult.buffer);

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

	vk::Sampler EngineState::GetDefaultTextureSampler() const { return textureSampler; }

	vk::DescriptorSet EngineState::CreateTextureDescriptorSet(
		DescriptorSetAllocator& descriptorSetAllocator,
		vk::DescriptorSetLayout descriptorSetLayout
	) {
		return descriptorSetAllocator.allocateDescriptorSets({descriptorSetLayout})[0];
	}
	uint32_t EngineState::getFrameIndex() const {
		return frameIndex;
	}
	FrameState& EngineState::getCurrentFrameState() const {
		return const_cast<FrameState&>(frameStates[frameIndex]);
	}
	void EngineState::incrementFrameIndex() {
		frameIndex = (frameIndex + 1) % frameCount;
	}
	uint32_t EngineState::getFrameCount() const {
		return frameCount;
	}
}
