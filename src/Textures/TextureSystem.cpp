#include "TextureSystem.hpp"

namespace drk::Textures {

	TextureSystem::TextureSystem(
		const Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		entt::registry& registry
	)
		: DeviceContext(deviceContext), EngineState(engineState), registry(registry) {}

	void TextureSystem::UploadTextures() {
		auto imageEntities = registry.view<Textures::ImageInfo*>(entt::exclude<Devices::Texture>);
		for (const auto& imageEntity: imageEntities) {
			const auto[imageInfo, componentIndex] = registry.get<Textures::ImageInfo*, Common::ComponentIndex<ImageInfo>>(
				imageEntity
			);
			const auto& texture = EngineState.UploadTexture(imageInfo);
			registry.emplace<Devices::Texture>(imageEntity, texture);
		}
	}
}