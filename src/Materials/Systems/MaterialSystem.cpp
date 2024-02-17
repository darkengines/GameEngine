#include "MaterialSystem.hpp"
#include "../../Common/Components/Name.hpp"
#include "../../Textures/Systems/TextureSystem.hpp"

namespace drk::Materials::Systems {

	MaterialSystem::MaterialSystem(
		const drk::Devices::DeviceContext& deviceContext,
		entt::registry& registry,
		Engine::EngineState& engineState
	) : System(engineState, registry), deviceContext(deviceContext) {}

	void MaterialSystem::update(
		Models::Material& storedMaterial,
		const Materials::Components::Material& material
	) {
		storedMaterial.baseColor = material.baseColor;
		storedMaterial.ambientColor = material.ambientColor;
		storedMaterial.diffuseColor = material.diffuseColor;
		storedMaterial.specularColor = material.specularColor;

		auto hasBaseColorTexture = material.baseColorTexture != entt::null;
		auto hasAmbientColorTexture = material.ambientColorTexture != entt::null;
		auto hasDiffuseColorTexture = material.diffuseColorTexture != entt::null;
		auto hasSpecularColorTexture = material.specularColorTexture != entt::null;
		auto hasNormalMap = material.normalMap != entt::null;
		auto hasMetallicRoughnessMap = material.metallicRoughnessTexture != entt::null;

		storedMaterial.hasBaseColorTexture = hasBaseColorTexture;
		storedMaterial.hasAmbientColorTexture = hasAmbientColorTexture;
		storedMaterial.hasDiffuseColorTexture = hasDiffuseColorTexture;
		storedMaterial.hasSpecularColorTexture = hasSpecularColorTexture;
		storedMaterial.hasNormalMap = hasNormalMap;
		storedMaterial.hasMetallicRoughnessMap = hasMetallicRoughnessMap;

		if (hasBaseColorTexture) {
			storedMaterial.baseColorTextureIndex = registry.get<Devices::Texture>(
				material.baseColorTexture
			).index;
		}
		if (hasAmbientColorTexture) {
			storedMaterial.ambientColorTextureIndex = registry.get<Devices::Texture>(
				material.ambientColorTexture
			).index;
		}
		if (hasDiffuseColorTexture) {
			storedMaterial.diffuseColorTextureIndex = registry.get<Devices::Texture>(
				material.diffuseColorTexture
			).index;
		}
		if (hasSpecularColorTexture) {
			storedMaterial.specularColorTextureIndex = registry.get<Devices::Texture>(
				material.specularColorTexture
			).index;
		}
		if (hasNormalMap) {
			storedMaterial.normalMapIndex = registry.get<Devices::Texture>(
				material.normalMap
			).index;
		}
		if (hasMetallicRoughnessMap) {
			storedMaterial.metallicRoughnessTextureIndex = registry.get<Devices::Texture>(
				material.metallicRoughnessTexture
			).index;
		}
	}

	entt::entity
	MaterialSystem::copyMaterialEntity(
		const entt::registry& source,
		entt::registry& destination,
		entt::entity sourceEntity
	) {
		const auto& material = source.get<Materials::Components::Material>(sourceEntity);

		auto newEntity = destination.create();
		Components::Material newMaterial{
			.source = material.source,
			.baseColor = material.baseColor,
			.ambientColor = material.ambientColor,
			.diffuseColor = material.diffuseColor,
			.specularColor = material.specularColor,

			.baseColorTexture = Textures::Systems::TextureSystem::copyTextureEntity(
				source,
				destination,
				material.baseColorTexture
			),
			.ambientColorTexture = Textures::Systems::TextureSystem::copyTextureEntity(
				source,
				destination,
				material.ambientColorTexture
			),
			.diffuseColorTexture = Textures::Systems::TextureSystem::copyTextureEntity(
				source,
				destination,
				material.diffuseColorTexture
			),
			.specularColorTexture = Textures::Systems::TextureSystem::copyTextureEntity(
				source,
				destination,
				material.specularColorTexture
			),
			.normalMap = Textures::Systems::TextureSystem::copyTextureEntity(source, destination, material.normalMap),
			.metallicRoughnessTexture = Textures::Systems::TextureSystem::copyTextureEntity(
				source,
				destination,
				material.metallicRoughnessTexture
			),

			.hasTransparency = material.hasTransparency
		};
		auto name = source.get<Common::Components::Name>(sourceEntity);
		destination.emplace<Common::Components::Name>(newEntity, name);
		destination.emplace<Materials::Components::Material>(
			newEntity,
			newMaterial
		);

		return newEntity;
	}
}