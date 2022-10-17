#include "TextureSystem.hpp"

namespace drk::Textures {

	TextureSystem::TextureSystem(
		drk::Devices::DeviceContext *pContext,
		drk::Graphics::EngineState *pState
	)
		: DeviceContext(pContext), EngineState(pState) {}

	void TextureSystem::UploadTextures() {
		auto imageEntities = EngineState->Registry.view<Textures::ImageInfo *>(entt::exclude<Devices::Texture>);
		for (const auto &imageEntity: imageEntities) {
			const auto[imageInfo, componentIndex] = EngineState->Registry.get<Textures::ImageInfo *, Common::ComponentIndex<ImageInfo>>(
				imageEntity
			);
			const auto &texture = EngineState->UploadTexture(imageInfo);
			EngineState->Registry.emplace<Devices::Texture>(imageEntity, texture);
		}
	}
}