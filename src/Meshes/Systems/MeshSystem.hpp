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
#include "../../Graphics/GlobalSystem.hpp"
#include "../Resources/MeshResourceManager.hpp"
#include "MeshSystemOperation.hpp"

namespace drk::Meshes::Systems {
	class MeshSystem : public Draws::Systems::DrawSystem, public drk::Systems::System<Models::Mesh, Components::Mesh> {
	protected:
		Graphics::GlobalSystem& globalSystem;
		MeshSystemOperation operations;
		boost::signals2::connection cameraChangedConnection;
		Resources::MeshResourceManager& meshResourceManager;
	public:
		void update(Models::Mesh& model, const Components::Mesh& mesh) override;
	protected:
		const Devices::DeviceContext& deviceContext;
	public:
		MeshSystem(
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			entt::registry& registry,
			Graphics::GlobalSystem& globalSystem,
			Resources::MeshResourceManager& meshResourceManager
		);
		~MeshSystem();
		void onCameraChanged(entt::entity cameraEntity);
		void uploadMeshes();
		void emitDraws();
		void doOperations(MeshSystemOperation operations);
		void updateDraw(entt::entity drawEntity, int drawIndex);
		Draws::Components::DrawVertexBufferInfo getVertexBufferInfo(entt::entity drawEntity);
		static entt::entity
			copyMeshEntity(const entt::registry& source, entt::registry& destination, entt::entity sourceEntity);
		void processDirtyDraws();
	};
}