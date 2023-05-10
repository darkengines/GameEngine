#include "MaterialSystem.hpp"
#include "Components/Material.hpp"
#include "Models/Material.hpp"
#include "../Graphics/SynchronizationState.hpp"
#include <functional>

namespace drk::Materials {

	MaterialSystem::MaterialSystem(
		const drk::Devices::DeviceContext& deviceContext,
		entt::registry& registry,
		Engine::EngineState& engineState
	): System(engineState, registry), deviceContext(deviceContext) {}

	void MaterialSystem::Update(Models::Material& storedMaterial, Components::Material* const& material) {
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
		registry.on_construct<Components::Material*>().connect<MaterialSystem::OnMaterialConstruct>();
	}

	void MaterialSystem::RemoveMaterialSystem(entt::registry& registry) {
		registry.on_construct<Components::Material*>().disconnect<MaterialSystem::OnMaterialConstruct>();
	}

	void MaterialSystem::OnMaterialConstruct(entt::registry& registry, entt::entity materialEntity) {

	}
}