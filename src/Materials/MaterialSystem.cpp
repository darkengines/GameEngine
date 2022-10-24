#include "MaterialSystem.hpp"
#include "Material.hpp"
#include "Models/Material.hpp"
#include "../Graphics/SynchronizationState.hpp"
#include <functional>

namespace drk::Materials {

	MaterialSystem::MaterialSystem(
		drk::Devices::DeviceContext* pContext,
		drk::Graphics::EngineState* pState
	)
		: DeviceContext(pContext), EngineState(pState) {}

	void MaterialSystem::UpdateStoreItem(const Material* material, Models::Material& storedMaterial) {
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
			storedMaterial.baseColorTextureIndex = EngineState->Registry.get<Devices::Texture>(
				material->baseColorTexture
			).index;
		}
		if (hasAmbientColorTexture) {
			storedMaterial.ambientColorTextureIndex = EngineState->Registry.get<Devices::Texture>(
				material->ambientColorTexture
			).index;
		}
		if (hasDiffuseColorTexture) {
			storedMaterial.diffuseColorTextureIndex = EngineState->Registry.get<Devices::Texture>(
				material->diffuseColorTexture
			).index;
		}
		if (hasSpecularColorTexture) {
			storedMaterial.specularColorTextureIndex = EngineState->Registry.get<Devices::Texture>(
				material->specularColorTexture
			).index;
		}
		if (hasNormalMap) {
			storedMaterial.normalMapIndex = EngineState->Registry.get<Devices::Texture>(
				material->normalMap
			).index;
		}
		if (hasMetallicRoughnessMap) {
			storedMaterial.metallicRoughnessTextureIndex = EngineState->Registry.get<Devices::Texture>(
				material->metallicRoughnessTexture
			).index;
		}
	}


	void MaterialSystem::StoreMaterials() {
		EngineState->Store<Models::Material, Material*>();
	}

	void MaterialSystem::UpdateMaterials() {
		Graphics::SynchronizationState<Models::Material>::Update<Material*>(
			EngineState->Registry,
			EngineState->FrameIndex,
			std::function<void(Models::Material&, Material *const&)>([&](
				Models::Material& model,
				Material *const& component
			) { UpdateStoreItem(component, model); })
		);
	}

	void MaterialSystem::AddMaterialSystem(entt::registry& registry) {
		registry.on_construct<Material*>().connect<MaterialSystem::OnMaterialConstruct>();
	}

	void MaterialSystem::RemoveMaterialSystem(entt::registry& registry) {
		registry.on_construct<Material*>().disconnect<MaterialSystem::OnMaterialConstruct>();
	}

	void MaterialSystem::OnMaterialConstruct(entt::registry& registry, entt::entity materialEntity) {

	}
}