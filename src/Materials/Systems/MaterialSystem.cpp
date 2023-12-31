#include "MaterialSystem.hpp"
#include "../Components/Material.hpp"
#include "../Models/Material.hpp"
#include "../../Graphics/SynchronizationState.hpp"
#include "../../Textures/Systems/TextureSystem.hpp"
#include <functional>

namespace drk::Materials::Systems {

	MaterialSystem::MaterialSystem(
		const drk::Devices::DeviceContext& deviceContext,
		entt::registry& registry,
		Engine::EngineState& engineState
	) : System(engineState, registry), deviceContext(deviceContext) {}

	void MaterialSystem::update(
		Models::Material& storedMaterial,
		const std::shared_ptr<Materials::Components::Material>& material
	) {
		storedMaterial.baseColor = material->baseColor;
		storedMaterial.ambientColor = material->ambientColor;
		storedMaterial.diffuseColor = material->diffuseColor;
		storedMaterial.specularColor = material->specularColor;

		auto hasBaseColorTexture = material->baseColorTexture != entt::null;
		auto hasAmbientColorTexture = material->ambientColorTexture != entt::null;
		auto hasDiffuseColorTexture = material->diffuseColorTexture != entt::null;
		auto hasSpecularColorTexture = material->specularColorTexture != entt::null;
		auto hasNormalMap = material->normalMap != entt::null;
		auto hasMetallicRoughnessMap = material->metallicRoughnessTexture != entt::null;

		storedMaterial.hasBaseColorTexture = hasBaseColorTexture;
		storedMaterial.hasAmbientColorTexture = hasAmbientColorTexture;
		storedMaterial.hasDiffuseColorTexture = hasDiffuseColorTexture;
		storedMaterial.hasSpecularColorTexture = hasSpecularColorTexture;
		storedMaterial.hasNormalMap = hasNormalMap;
		storedMaterial.hasMetallicRoughnessMap = hasMetallicRoughnessMap;

		if (hasBaseColorTexture) {
			storedMaterial.baseColorTextureIndex = registry.get<Devices::Texture>(
				material->baseColorTexture
			).index;
		}
		if (hasAmbientColorTexture) {
			storedMaterial.ambientColorTextureIndex = registry.get<Devices::Texture>(
				material->ambientColorTexture
			).index;
		}
		if (hasDiffuseColorTexture) {
			storedMaterial.diffuseColorTextureIndex = registry.get<Devices::Texture>(
				material->diffuseColorTexture
			).index;
		}
		if (hasSpecularColorTexture) {
			storedMaterial.specularColorTextureIndex = registry.get<Devices::Texture>(
				material->specularColorTexture
			).index;
		}
		if (hasNormalMap) {
			storedMaterial.normalMapIndex = registry.get<Devices::Texture>(
				material->normalMap
			).index;
		}
		if (hasMetallicRoughnessMap) {
			storedMaterial.metallicRoughnessTextureIndex = registry.get<Devices::Texture>(
				material->metallicRoughnessTexture
			).index;
		}
	}

	void MaterialSystem::AddMaterialSystem(entt::registry& registry) {
		registry.on_construct<std::shared_ptr<Components::Material>>().connect<MaterialSystem::OnMaterialConstruct>();
	}

	void MaterialSystem::RemoveMaterialSystem(entt::registry& registry) {
		registry.on_construct<std::shared_ptr<Components::Material>>().disconnect<MaterialSystem::OnMaterialConstruct>();
	}

	void MaterialSystem::OnMaterialConstruct(entt::registry& registry, entt::entity materialEntity) {

	}
	entt::entity
	MaterialSystem::copyMaterialEntity(const entt::registry& source, entt::registry& destination, entt::entity sourceEntity) {
		const auto& material = source.get<std::shared_ptr<Materials::Components::Material>>(sourceEntity);

		auto newEntity = destination.create();
		Components::Material newMaterial{
			.name = material->name,
			.source = material->source,
			.baseColor = material->baseColor,
			.ambientColor = material->ambientColor,
			.diffuseColor = material->diffuseColor,
			.specularColor = material->specularColor,

			.baseColorTexture = Textures::Systems::TextureSystem::copyTextureEntity(
				source,
				destination,
				material->baseColorTexture
			),
			.ambientColorTexture = Textures::Systems::TextureSystem::copyTextureEntity(
				source,
				destination,
				material->ambientColorTexture
			),
			.diffuseColorTexture = Textures::Systems::TextureSystem::copyTextureEntity(
				source,
				destination,
				material->diffuseColorTexture
			),
			.specularColorTexture = Textures::Systems::TextureSystem::copyTextureEntity(
				source,
				destination,
				material->specularColorTexture
			),
			.normalMap = Textures::Systems::TextureSystem::copyTextureEntity(source, destination, material->normalMap),
			.metallicRoughnessTexture = Textures::Systems::TextureSystem::copyTextureEntity(
				source,
				destination,
				material->metallicRoughnessTexture
			),

			.hasTransparency = material->hasTransparency
		};

		destination.emplace<std::shared_ptr<Materials::Components::Material>>(
			newEntity,
			std::make_shared<Materials::Components::Material>(
				newMaterial
			));

		return newEntity;
	}
}