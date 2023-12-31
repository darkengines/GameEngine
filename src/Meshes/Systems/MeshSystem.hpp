#pragma once

#include "../../Devices/DeviceContext.hpp"
#include "../../Engine/EngineState.hpp"
#include "../Models/Mesh.hpp"
#include "../../Draws/Systems/DrawSystem.hpp"
#include "../../Systems/System.hpp"
#include "../Components/Mesh.hpp"
#include "../../Objects/Models/Object.hpp"
#include "../../Objects/Components/ObjectMesh.hpp"
#include "../../Cameras/Components/Camera.hpp"
#include "../../Cameras/Models/Camera.hpp"
#include "../../Lights/Components/LightPerspective.hpp"
#include "../../Lights/Models/LightPerspective.hpp"
#include "../../Spatials/Components/Spatial.hpp"
#include "../../Spatials/Models/Spatial.hpp"
#include "../../Stores/Models/StoreItemLocation.hpp"

namespace drk::Meshes::Systems {
	class MeshSystem : public Draws::Systems::DrawSystem, public drk::Systems::System<Models::Mesh, Components::Mesh> {
	protected:
	public:
		void update(Models::Mesh& model, const Components::Mesh& mesh) override;
	protected:
		const Devices::DeviceContext& deviceContext;
		entt::entity ProcessObjectEntity(
			entt::entity objectMeshEntity,
			entt::entity lightEntity,
			entt::entity lightPerspectiveEntity,
			const Lights::Components::LightPerspective& perspective,
			const Cameras::Components::Camera& camera,
			const Spatials::Components::Spatial& spatial,
			const Materials::Components::Material& material,
			const Meshes::Components::MeshBufferView& meshBufferView,
			std::shared_ptr<Meshes::Components::MeshResource> pMeshResource,
			const Stores::Models::StoreItemLocation& objectStoreItemLocation,
			const Stores::Models::StoreItemLocation& meshStoreItemLocation,
			const Stores::Models::StoreItemLocation& cameraStoreItemLocation,
			const Stores::Models::StoreItemLocation& lightPerspectiveStoreItemLocation,
			const Stores::Models::StoreItemLocation& lightPerspectiveSpatialStoreItemLocation
		);
	public:
		MeshSystem(
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			entt::registry& registry
		);
		static void AddMeshSystem(entt::registry& registry);
		static void RemoveMeshSystem(entt::registry& registry);
		static void OnMeshConstruct(entt::registry& registry, entt::entity meshEntity);
		void UploadMeshes();
		bool emitDraws();
		bool emitShadowDraws();
		void UpdateDraw(entt::entity drawEntity, int drawIndex);
		void UpdateShadowDraw(entt::entity shadowDrawEntity, int drawIndex);
		void UpdateDraws() { throw std::runtime_error("Not supported"); }
		Draws::Components::DrawVertexBufferInfo GetVertexBufferInfo(entt::entity drawEntity);
		static entt::entity
			copyMeshEntity(const entt::registry& source, entt::registry& destination, entt::entity sourceEntity);
		void ProcessDirtyDraws();
		void ProcessShadowDirtyDraws();
	};
}