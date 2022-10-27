#include "TextureSystem.hpp"

namespace drk::Textures {

	TextureSystem::TextureSystem(
		const Devices::DeviceContext& deviceContext,
		Graphics::EngineState& engineState,
		entt::registry& registry
	)
		: DeviceContext(deviceContext), EngineState(engineState), Registry(registry) {}

	void TextureSystem::UploadTextures() {
		auto imageEntities = Registry.view<Textures::ImageInfo*>(entt::exclude<Devices::Texture>);
		for (const auto& imageEntity: imageEntities) {
			const auto[imageInfo, componentIndex] = Registry.get<Textures::ImageInfo*, Common::ComponentIndex<ImageInfo>>(
				imageEntity
			);
			const auto& texture = EngineState.UploadTexture(imageInfo);
			EngineState.Registry.emplace<Devices::Texture>(imageEntity, texture);
		}
	}
}