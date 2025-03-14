#pragma once

#include "../../Devices/DeviceContext.hpp"
#include "../../Engine/EngineState.hpp"
#include "../Models/Mesh.hpp"
#include "../../Draws/Systems/IDrawSystem.hpp"
#include "../../Systems/StorageSystem.hpp"
#include "../Components/Mesh.hpp"
#include "../../Nodes/Models/Node.hpp"
#include "../../Nodes/Components/NodeMesh.hpp"
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
#include "../../Materials/Components/MaterialReference.hpp"

namespace drk::Meshes::Systems {
	class MeshSystem :
		public Draws::Systems::IDrawSystem,
		public drk::Systems::StorageSystem<
			Models::Mesh,
			Materials::Components::MaterialReference
		> {
	protected:
		Graphics::GlobalSystem& globalSystem;
		MeshSystemOperation operations;
		boost::signals2::connection cameraChangedConnection;
		Resources::MeshResourceManager& meshResourceManager;
	public:
		void update(
			Models::Mesh& model,
			const Materials::Components::MaterialReference&
		) override;
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
		void emitDraws() override;
		void doOperations(MeshSystemOperation operations);
		void updateDraw(entt::entity drawEntity, int drawIndex) override;
		Draws::Components::DrawVertexBufferInfo getVertexBufferInfo(entt::entity drawEntity);
		static entt::entity
		copyMeshEntity(const entt::registry& source, entt::registry& destination, entt::entity sourceEntity);
		void processDirtyDraws();
	};
}
