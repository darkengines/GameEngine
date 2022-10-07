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

		storedMaterial.hasBaseColorTexture = material->baseColorTexture != entt::null;
		storedMaterial.hasAmbientColorTexture = material->ambientColorTexture != entt::null;
		storedMaterial.hasDiffuseColorTexture = material->diffuseColorTexture != entt::null;
		storedMaterial.hasSpecularColorTexture = material->specularColorTexture != entt::null;
		storedMaterial.hasNormalMap = material->normalMap != entt::null;
		storedMaterial.hasMetallicRoughnessMap = material->metallicRoughnessTexture != entt::null;

		storedMaterial.baseColorTextureIndex = EngineState->Registry.get<Common::ComponentIndex<Textures::ImageInfo>>(
			material->baseColorTexture
		);
		storedMaterial.ambientColorTextureIndex = EngineState->Registry.get<Common::ComponentIndex<Textures::ImageInfo>>(
			material->ambientColorTexture
		);
		storedMaterial.diffuseColorTextureIndex = EngineState->Registry.get<Common::ComponentIndex<Textures::ImageInfo>>(
			material->diffuseColorTexture
		);
		storedMaterial.specularColorTextureIndex = EngineState->Registry.get<Common::ComponentIndex<Textures::ImageInfo>>(
			material->specularColorTexture
		);
		storedMaterial.normalMapIndex = EngineState->Registry.get<Common::ComponentIndex<Textures::ImageInfo>>(
			material->normalMap
		);
		storedMaterial.metallicRoughnessTextureIndex = EngineState->Registry.get<Common::ComponentIndex<Textures::ImageInfo>>(
			material->metallicRoughnessTexture
		);
	}


	void MaterialSystem::StoreMaterials() {
		auto materialEntities = EngineState->Registry.view<Material *>(entt::exclude<Graphics::StoreItem<Models::Material>>);
		for (const auto materialEntity : materialEntities) {
			auto storeItem = EngineState->Store<Models::Material>();
			EngineState->Registry.emplace<Graphics::StoreItem<Models::Material>>(
				materialEntity,
				storeItem.frameStoreItems
			);
		}
	}

	void MaterialSystem::UpdateMaterials() {
		auto func = std::function < void(
		const Material *component, Models::Material
		&storeItem)>(
			[=](const Material *material, Models::Material &model) { UpdateStoreItem(material, model); }
		);
		//EngineState->UpdateStore<Material *, Models::Material>(func);
	}
}