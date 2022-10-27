#define VULKAN_HPP_NO_CONSTRUCTORS

#include <vulkan/vulkan.hpp>
#include "EngineState.hpp"
#include <utility>
#include <algorithm>
#include <span>

namespace drk::Graphics {
	EngineState::EngineState(const Devices::DeviceContext& deviceContext, entt::registry& registry) :
		DeviceContext(deviceContext), Registry(registry), TextureSampler(CreateTextureSampler(DeviceContext)),
		DescriptorSetLayoutCache(std::make_unique<Graphics::DescriptorSetLayoutCache>(DeviceContext.Device)),
		DescriptorSetAllocator(std::make_unique<Graphics::DescriptorSetAllocator>(DeviceContext.Device)),
		StorageBufferDescriptorSetLayout(CreateStorageBufferDescriptorSetLayout(DescriptorSetLayoutCache.get())),
		DrawStorageBufferDescriptorSetLayout(CreateStorageBufferDescriptorSetLayout(DescriptorSetLayoutCache.get())),
		GlobalUniformBufferDescriptorSetLayout(CreateGlobalUniformBufferDescriptorSetLayout(DescriptorSetLayoutCache.get())) {

		CreateTextureDescriptorSet();
		TextureStore = std::make_unique<Stores::TextureStore>(DeviceContext, TextureDescriptorSet, TextureSampler);

		DescriptorSetLayouts.push_back(TextureDescriptorSetLayout);
		DescriptorSetLayouts.push_back(StorageBufferDescriptorSetLayout);
		DescriptorSetLayouts.push_back(DrawStorageBufferDescriptorSetLayout);
		DescriptorSetLayouts.push_back(GlobalUniformBufferDescriptorSetLayout);

		FrameStates.push_back(
			std::move<Graphics::FrameState>(
				{
					DeviceContext,
					StorageBufferDescriptorSetLayout,
					DrawStorageBufferDescriptorSetLayout,
					GlobalUniformBufferDescriptorSetLayout,
					DescriptorSetAllocator.get(),
					TextureDescriptorSet
				}
			));
		FrameStates.push_back(
			std::move<Graphics::FrameState>(
				{
					DeviceContext,
					StorageBufferDescriptorSetLayout,
					DrawStorageBufferDescriptorSetLayout,
					GlobalUniformBufferDescriptorSetLayout,
					DescriptorSetAllocator.get(),
					TextureDescriptorSet
				}
			));
	}

	Devices::Texture EngineState::UploadTexture(const Textures::ImageInfo* const imageInfo) {
		return TextureStore->UploadTextures({imageInfo})[0];
	}

	std::vector<Devices::Texture> EngineState::UploadTextures(std::vector<const Textures::ImageInfo*> imageInfos) {
		return TextureStore->UploadTextures(imageInfos);
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
		const auto sampler = deviceContext.Device.createSampler(samplerCreateInfo);
		return sampler;
	}

	EngineState::~EngineState() {
		for (const auto buffer: Buffers) {
			DeviceContext.DestroyBuffer(buffer);
		}
		DeviceContext.Device.destroySampler(TextureSampler);
	}

	void EngineState::CreateTextureDescriptorSet() {

		auto bindFlags =
			vk::DescriptorBindingFlagBitsEXT::ePartiallyBound | vk::DescriptorBindingFlagBitsEXT::eUpdateAfterBind;

		vk::DescriptorSetLayoutBindingFlagsCreateInfoEXT extendedInfo = {
			.bindingCount = 1u,
			.pBindingFlags = &bindFlags
		};

		vk::DescriptorSetLayoutBinding binding = {
			.binding = 0,
			.descriptorType = vk::DescriptorType::eCombinedImageSampler,
			//TODO: make descriptorCount configurable
			.descriptorCount = 2048,
			.stageFlags = vk::ShaderStageFlagBits::eFragment
		};
		vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
			.pNext = &extendedInfo,
			.flags = vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool,
			.bindingCount = 1,
			.pBindings = &binding,
		};

		TextureDescriptorSetLayout = DescriptorSetLayoutCache->get(descriptorSetLayoutCreateInfo);
		TextureDescriptorSet = DescriptorSetAllocator->AllocateDescriptorSets({TextureDescriptorSetLayout})[0];
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
			DeviceContext.PhysicalDevice,
			DeviceContext.Device,
			DeviceContext.GraphicQueue,
			DeviceContext.CommandPool,
			DeviceContext.Allocator,
			vertices,
			vk::BufferUsageFlagBits::eVertexBuffer
		);
		auto indexBufferUploadResult = Devices::Device::uploadBuffers(
			DeviceContext.PhysicalDevice,
			DeviceContext.Device,
			DeviceContext.GraphicQueue,
			DeviceContext.CommandPool,
			DeviceContext.Allocator,
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

	vk::DescriptorSetLayout
	EngineState::CreateStorageBufferDescriptorSetLayout(Graphics::DescriptorSetLayoutCache* const descriptorSetLayoutCache) {

		auto bindFlags =
			vk::DescriptorBindingFlagBitsEXT::ePartiallyBound | vk::DescriptorBindingFlagBitsEXT::eUpdateAfterBind;

		vk::DescriptorSetLayoutBindingFlagsCreateInfoEXT extendedInfo = {
			.bindingCount = 1u,
			.pBindingFlags = &bindFlags
		};

		vk::DescriptorSetLayoutBinding binding = {
			.binding = 0,
			.descriptorType = vk::DescriptorType::eStorageBuffer,
			.descriptorCount = 64,
			.stageFlags = vk::ShaderStageFlagBits::eAll
		};
		vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
			.pNext = &extendedInfo,
			.flags = vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool,
			.bindingCount = 1,
			.pBindings = &binding,
		};

		return descriptorSetLayoutCache->get(descriptorSetLayoutCreateInfo);
	}

	vk::DescriptorSetLayout
	EngineState::CreateGlobalUniformBufferDescriptorSetLayout(Graphics::DescriptorSetLayoutCache* const descriptorSetLayoutCache) {

		vk::DescriptorSetLayoutBinding binding = {
			.binding = 0,
			.descriptorType = vk::DescriptorType::eUniformBuffer,
			.descriptorCount = 1,
			.stageFlags = vk::ShaderStageFlagBits::eAll
		};
		vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
			.bindingCount = 1,
			.pBindings = &binding,
		};

		return descriptorSetLayoutCache->get(descriptorSetLayoutCreateInfo);
	}
	vk::Sampler EngineState::GetDefaultTextureSampler() const { return TextureSampler; }
}
