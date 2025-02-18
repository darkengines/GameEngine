#include "../Models/LightPerspective.hpp"
#include "../Components/LightPerspective.hpp"
#include "PointLightSystem.hpp"
#include "../../Spatials/Components/Spatial.hpp"
#include "../../Common/Components/Dirty.hpp"
#include "../Components/LightPerspectiveCollection.hpp"
#include "../../Nodes/Components/Node.hpp"
#include "../../Common/Components/Name.hpp"
#include "../Components/Light.hpp"
#include "../../GlmExtensions.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace drk::Lights::Systems {
	PointLightSystem::PointLightSystem(
		const Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		entt::registry& registry,
		ShadowMappingSystem& shadowMappingSystem
	) : System(engineState, registry), deviceContext(deviceContext), shadowMappingSystem(shadowMappingSystem) {}
	void PointLightSystem::update(
		Models::PointLight& model,
		const Components::PointLight& component,
		const Stores::StoreItem<Models::Light>& lightStoreItem,
		const Stores::StoreItem<Spatials::Models::Spatial>& spatialStoreItem
	) {
		model.constantAttenuation = component.constantAttenuation;
		model.linearAttenuation = component.linearAttenuation;
		model.quadraticAttenuation = component.quadraticAttenuation;

		const auto& frontLightPerspectiveStoreItem = registry.get<Stores::StoreItem<Models::LightPerspective>>(component.frontLightPerspectiveEntity);
		const auto& backLightPerspectiveStoreItem = registry.get<Stores::StoreItem<Models::LightPerspective>>(component.backLightPerspectiveEntity);
		const auto& leftLightPerspectiveStoreItem = registry.get<Stores::StoreItem<Models::LightPerspective>>(component.leftLightPerspectiveEntity);
		const auto& rightLightPerspectiveStoreItem = registry.get<Stores::StoreItem<Models::LightPerspective>>(component.rightLightPerspectiveEntity);
		const auto& topLightPerspectiveStoreItem = registry.get<Stores::StoreItem<Models::LightPerspective>>(component.topLightPerspectiveEntity);
		const auto& downLightPerspectiveStoreItem = registry.get<Stores::StoreItem<Models::LightPerspective>>(component.downLightPerspectiveEntity);

		model.lightStoreItemLocation = lightStoreItem.frameStoreItems[engineState.getFrameIndex()];
		model.spatialStoreItemLocation = spatialStoreItem.frameStoreItems[engineState.getFrameIndex()];

		model.frontLightPerspectiveStoreItemLocation = frontLightPerspectiveStoreItem.frameStoreItems[engineState.getFrameIndex()];
		model.backLightPerspectiveStoreItemLocation = backLightPerspectiveStoreItem.frameStoreItems[engineState.getFrameIndex()];
		model.topLightPerspectiveStoreItemLocation = topLightPerspectiveStoreItem.frameStoreItems[engineState.getFrameIndex()];
		model.downLightPerspectiveStoreItemLocation = downLightPerspectiveStoreItem.frameStoreItems[engineState.getFrameIndex()];
		model.leftLightPerspectiveStoreItemLocation = leftLightPerspectiveStoreItem.frameStoreItems[engineState.getFrameIndex()];
		model.rightLightPerspectiveStoreItemLocation = rightLightPerspectiveStoreItem.frameStoreItems[engineState.getFrameIndex()];
	}

	void PointLightSystem::processDirtyItems() {
		auto dirtyPointLightView = registry.view<
			Components::PointLight,
			Spatials::Components::Spatial<Spatials::Components::Absolute>,
			Components::LightPerspectiveCollection,
			Common::Components::Dirty<Spatials::Components::Spatial<Spatials::Components::Absolute>>
		>();
		dirtyPointLightView.each(
			[&](
				entt::entity pointLightEntity,
				Components::PointLight& pointLight,
				Spatials::Components::Spatial<Spatials::Components::Absolute>& spatial,
				Components::LightPerspectiveCollection& lightPerspectiveCollection
			) {
				for (auto lightPerspectiveEntity: lightPerspectiveCollection.lightPerspectives) {
					auto& lightPerspective = registry.get<Components::LightPerspective>(lightPerspectiveEntity);
					if (lightPerspective.shadowMapRect.extent.width == 0) {
						auto allocation = shadowMappingSystem.shadowMapAllocator.allocate({1024, 1024});
						lightPerspective.shadowMapRect = allocation.scissor;
					}
					auto absoluteRotation = glm::toMat4(spatial.rotation);
					/*lightPerspective.absoluteFront = absoluteRotation * lightPerspective.relativeFront;
					lightPerspective.absoluteUp = absoluteRotation * lightPerspective.relativeUp;*/
					lightPerspective.absoluteFront = lightPerspective.relativeFront;
					lightPerspective.absoluteUp = lightPerspective.relativeUp;
					lightPerspective.view = glm::lookAt(
						glm::make_vec3(spatial.position),
						glm::make_vec3(spatial.position + lightPerspective.absoluteFront),
						glm::make_vec3(lightPerspective.absoluteUp));
					lightPerspective.perspective = glm::perspectiveZO(
						lightPerspective.verticalFov,
						lightPerspective.aspectRatio,
						lightPerspective.near,
						lightPerspective.far
					);
					lightPerspective.perspective[1][1] *= -1.0f;

					registry.emplace_or_replace<Graphics::SynchronizationState<Models::LightPerspective>>(
						lightPerspectiveEntity,
						static_cast<uint32_t>(engineState.getFrameCount())
					);
				}
			}
		);
	}
	entt::entity PointLightSystem::createPointLight() {
		Lights::Components::LightPerspective frontLightPerspective = {
			.relativeFront = GlmExtensions::front,
			.relativeUp = GlmExtensions::up,
			.verticalFov = glm::half_pi<float>(),
			.aspectRatio = 1.0f,
			.near = 0.001f,
			.far = 128.0f
		};

		Lights::Components::LightPerspective backLightPerspective = {
			.relativeFront = GlmExtensions::back,
			.relativeUp = GlmExtensions::up,
			.verticalFov = glm::half_pi<float>(),
			.aspectRatio = 1.0f,
			.near = 0.001f,
			.far = 128.0f
		};

		Lights::Components::LightPerspective leftLightPerspective = {
			.relativeFront = GlmExtensions::left,
			.relativeUp = GlmExtensions::up,
			.verticalFov = glm::half_pi<float>(),
			.aspectRatio = 1.0f,
			.near = 0.001f,
			.far = 128.0f
		};

		Lights::Components::LightPerspective rightLightPerspective = {
			.relativeFront = GlmExtensions::right,
			.relativeUp = GlmExtensions::up,
			.verticalFov = glm::half_pi<float>(),
			.aspectRatio = 1.0f,
			.near = 0.001f,
			.far = 128.0f
		};

		Lights::Components::LightPerspective upLightPerspective = {
			.relativeFront = GlmExtensions::up,
			.relativeUp = GlmExtensions::back,
			.verticalFov = glm::half_pi<float>(),
			.aspectRatio = 1.0f,
			.near = 0.001f,
			.far = 128.0f
		};

		Lights::Components::LightPerspective downLightPerspective = {
			.relativeFront = GlmExtensions::down,
			.relativeUp = GlmExtensions::front,
			.verticalFov = glm::half_pi<float>(),
			.aspectRatio = 1.0f,
			.near = 0.001f,
			.far = 128.0f
		};

		auto frontLightPerspectiveEntity = registry.create();
		registry.emplace<Lights::Components::LightPerspective>(
			frontLightPerspectiveEntity,
			std::move(
				frontLightPerspective
			));
		auto backLightPerspectiveEntity = registry.create();
		registry.emplace<Lights::Components::LightPerspective>(
			backLightPerspectiveEntity,
			std::move(
				backLightPerspective
			));
		auto leftLightPerspectiveEntity = registry.create();
		registry.emplace<Lights::Components::LightPerspective>(
			leftLightPerspectiveEntity,
			std::move(
				leftLightPerspective
			));
		auto rightLightPerspectiveEntity = registry.create();
		registry.emplace<Lights::Components::LightPerspective>(
			rightLightPerspectiveEntity,
			std::move(
				rightLightPerspective
			));
		auto upLightPerspectiveEntity = registry.create();
		registry.emplace<Lights::Components::LightPerspective>(
			upLightPerspectiveEntity,
			std::move(upLightPerspective));
		auto downLightPerspectiveEntity = registry.create();
		registry.emplace<Lights::Components::LightPerspective>(
			downLightPerspectiveEntity,
			std::move(
				downLightPerspective
			));

		auto lightNodeEntity = registry.create();
		registry.emplace<Spatials::Components::Spatial<Spatials::Components::Relative>>(
			lightNodeEntity,
				glm::vec4{0.0f, 0.0f, 0.0f, 0.0f},
				glm::quat{1.0f, 0.0f, 0.0f, 0.0f},
				glm::vec4{1.0f, 1.0f, 1.0f, 0.0f},
				glm::identity<glm::mat4>()
		);
		registry.emplace<Lights::Components::Light>(
			lightNodeEntity,
			glm::vec4{256.0f, 256.0f, 256.0f, 1.0f},
			glm::vec4{256.0f, 256.0f, 256.0f, 1.0f},
			glm::vec4{256.0f, 256.0f, 256.0f, 1.0f}
		);
		registry.emplace<Lights::Components::PointLight>(
			lightNodeEntity,
			1.0f,
			1.0f,
			1.0f,
			frontLightPerspectiveEntity,
			backLightPerspectiveEntity,
			leftLightPerspectiveEntity,
			rightLightPerspectiveEntity,
			upLightPerspectiveEntity,
			downLightPerspectiveEntity
		);
		registry.emplace<Common::Components::Name>(
			lightNodeEntity,
			"PointLight"
		);
		registry.emplace<Lights::Components::LightPerspectiveCollection>(
			lightNodeEntity,
			std::vector<entt::entity>{
				frontLightPerspectiveEntity,
				backLightPerspectiveEntity,
				leftLightPerspectiveEntity,
				rightLightPerspectiveEntity,
				upLightPerspectiveEntity,
				downLightPerspectiveEntity
			}
		);
		registry.emplace<Nodes::Components::Node>(lightNodeEntity);

		return lightNodeEntity;
	}
}