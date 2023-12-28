#include "TextureSystem.hpp"

namespace drk::Textures::Systems {

	TextureSystem::TextureSystem(
		const Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		entt::registry& registry
	)
		: DeviceContext(deviceContext), EngineState(engineState), registry(registry) {}

	void TextureSystem::UploadTextures() {
		auto imageEntities = registry.view<std::shared_ptr<Textures::ImageInfo>>(entt::exclude<Devices::Texture>);
		for (const auto& imageEntity: imageEntities) {
			const auto imageInfo = registry.get<std::shared_ptr<Textures::ImageInfo>>(
				imageEntity
			);
			if (imageInfo->pixels.size() > 0) {
				const auto& texture = EngineState.UploadTexture(imageInfo.get());
				registry.emplace<Devices::Texture>(imageEntity, texture);
			} else {
				//TODO: apply default texture
			}
		}
	}
	entt::entity
	TextureSystem::copyTextureEntity(
		const entt::registry& source,
		entt::registry& destination,
		entt::entity sourceEntity
	) {
		if (sourceEntity == entt::null) return entt::null;
		auto imageInfo = source.get<std::shared_ptr<ImageInfo>>(sourceEntity);
		auto texture = source.try_get<Devices::Texture>(sourceEntity);
		auto newEntity = destination.create();
		destination.emplace<std::shared_ptr<ImageInfo>>(newEntity, imageInfo);
		if (texture != nullptr) destination.emplace<Devices::Texture>(newEntity, *texture);
		return newEntity;
	}
}