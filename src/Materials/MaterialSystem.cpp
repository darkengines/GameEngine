#include "MaterialSystem.hpp"
#include "Material.hpp"
#include <functional>

namespace drk::Materials {

	MaterialSystem::MaterialSystem(
		drk::Devices::DeviceContext *pContext,
		drk::Graphics::EngineState *pState
	)
		: DeviceContext(pContext), EngineState(pState) {}

	void MaterialSystem::UpdateStoreItem(const Material *material, Models::Material &storedMaterial) {
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
			storedMaterial.baseColorTextureIndex = EngineState->Registry.get<Common::ComponentIndex<Textures::ImageInfo>>(
				material->baseColorTexture
			);
		}
		if (hasAmbientColorTexture) {
			storedMaterial.ambientColorTextureIndex = EngineState->Registry.get<Common::ComponentIndex<Textures::ImageInfo>>(
				material->ambientColorTexture
			);
		}
		if (hasDiffuseColorTexture) {
			storedMaterial.diffuseColorTextureIndex = EngineState->Registry.get<Common::ComponentIndex<Textures::ImageInfo>>(
				material->diffuseColorTexture
			);
		}
		if (hasSpecularColorTexture) {
			storedMaterial.specularColorTextureIndex = EngineState->Registry.get<Common::ComponentIndex<Textures::ImageInfo>>(
				material->specularColorTexture
			);
		}
		if (hasNormalMap) {
			storedMaterial.normalMapIndex = EngineState->Registry.get<Common::ComponentIndex<Textures::ImageInfo>>(
				material->normalMap
			);
		}
		if (hasMetallicRoughnessMap) {
			storedMaterial.metallicRoughnessTextureIndex = EngineState->Registry.get<Common::ComponentIndex<Textures::ImageInfo>>(
				material->metallicRoughnessTexture
			);
		}
	}


	void MaterialSystem::StoreMaterials() {
		EngineState->Store<Models::Material, Material *>();
	}

	void MaterialSystem::UpdateMaterials() {
		Graphics::SynchronizationState<Models::Material>::Update<Material *>(
			EngineState->Registry,
			EngineState->FrameIndex,
			[=](
				Models::Material &model,
				const Material *component
			) { UpdateStoreItem(component, model); }
		);
	}

	void MaterialSystem::AddMaterialSystem(entt::registry &registry) {
		registry.on_construct<Material *>().connect<MaterialSystem::OnMaterialConstruct>();
	}

	void MaterialSystem::RemoveMaterialSystem(entt::registry &registry) {
		registry.on_construct<Material *>().disconnect<MaterialSystem::OnMaterialConstruct>();
	}

	void MaterialSystem::OnMaterialConstruct(entt::registry &registry, entt::entity materialEntity) {
		registry.emplace<Graphics::SynchronizationState<Models::Material>>(materialEntity, 2u);
	}
}