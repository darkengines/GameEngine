#include "EngineState.hpp"

#include <chrono>
#include <vulkan/vulkan.hpp>

namespace drk::Engine {
EngineState::EngineState(
	const Devices::DeviceContext& deviceContext,
	entt::registry& registry,
	DescriptorSetLayoutCache& descriptorSetLayoutCache,
	DescriptorSetAllocator& descriptorSetAllocator,
	const DescriptorSetLayouts& descriptorSetLayouts
)
	: deviceContext(deviceContext),
	  descriptorSetLayoutCache(descriptorSetLayoutCache),
	  registry(registry),
	  textureSampler(CreateTextureSampler(deviceContext)),
	  shadowTextureSampler(CreateShadowTextureSampler(deviceContext)),
	  descriptorSetAllocator(descriptorSetAllocator),
	  descriptorSetLayouts(descriptorSetLayouts),
	  textureDescriptorSet(CreateTextureDescriptorSet(descriptorSetAllocator, descriptorSetLayouts.textureDescriptorSetLayout)) {
	CreateImguiResources();
	frameStates.reserve(frameCount);

	frameStates.emplace_back(deviceContext, descriptorSetLayouts, descriptorSetAllocator);
	frameStates.emplace_back(deviceContext, descriptorSetLayouts, descriptorSetAllocator);

	textureStore = std::make_unique<Stores::TextureStore>(deviceContext, textureDescriptorSet, textureSampler, shadowTextureSampler);
}

Devices::Texture EngineState::UploadTexture(const Textures::ImageInfo& imageInfo) { return textureStore->UploadTextures({&imageInfo})[0]; }

std::vector<Devices::Texture> EngineState::UploadTextures(std::vector<const Textures::ImageInfo*> imageInfos) { return textureStore->UploadTextures(imageInfos); }

vk::Sampler EngineState::CreateTextureSampler(const Devices::DeviceContext& deviceContext) {
	vk::SamplerCreateInfo samplerCreateInfo{
		.magFilter = vk::Filter::eLinear,
		.minFilter = vk::Filter::eLinear,
		.mipmapMode = vk::SamplerMipmapMode::eLinear,
		.addressModeU = vk::SamplerAddressMode::eRepeat,
		.addressModeV = vk::SamplerAddressMode::eRepeat,
		.addressModeW = vk::SamplerAddressMode::eRepeat,
		.mipLodBias = 0,
		// TODO: Make Anisotropy configurable
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

vk::Sampler EngineState::CreateShadowTextureSampler(const Devices::DeviceContext& deviceContext) {
	vk::SamplerCreateInfo samplerCreateInfo{
		.magFilter = vk::Filter::eNearest,
		.minFilter = vk::Filter::eNearest,
		.mipmapMode = vk::SamplerMipmapMode::eLinear,
		.addressModeU = vk::SamplerAddressMode::eRepeat,
		.addressModeV = vk::SamplerAddressMode::eRepeat,
		.addressModeW = vk::SamplerAddressMode::eRepeat,
		.mipLodBias = 0,
		// TODO: Make Anisotropy configurable
		.anisotropyEnable = VK_FALSE,
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
	deviceContext.device.destroyDescriptorPool(imGuiDescriptorPool);
	deviceContext.device.destroySampler(textureSampler);
	deviceContext.device.destroySampler(shadowTextureSampler);
}

vk::Sampler EngineState::GetDefaultTextureSampler() const { return textureSampler; }

vk::DescriptorSet EngineState::CreateTextureDescriptorSet(DescriptorSetAllocator& descriptorSetAllocator, vk::DescriptorSetLayout descriptorSetLayout) {
	return descriptorSetAllocator.allocateDescriptorSets({descriptorSetLayout})[0];
}
uint32_t EngineState::getFrameIndex() const { return frameIndex; }
FrameState& EngineState::getCurrentFrameState() const { return const_cast<FrameState&>(frameStates[frameIndex]); }
void EngineState::incrementFrameIndex() { frameIndex = (frameIndex + 1) % frameCount; }
uint32_t EngineState::getFrameCount() const { return frameCount; }
double EngineState::getTime() const {
	auto duration = std::chrono::high_resolution_clock::now() - startedOn;
	return std::chrono::duration<double>(duration).count();
}
std::chrono::high_resolution_clock::duration EngineState::getDuration() const {
	auto duration = std::chrono::high_resolution_clock::now() - startedOn;
	return duration;
}
}  // namespace drk::Engine
