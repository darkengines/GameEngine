#pragma once

#include "../../Systems/StorageSystem.hpp"
#include "../Models/AxisAlignedBoundingBox.hpp"
#include "../Components/AxisAlignedBoundingBox.hpp"
#include "../../Common/Components/Dirty.hpp"
#include "../../Spatials/Components/Spatial.hpp"
#include "../../Draws/Systems/IDrawSystem.hpp"
#include "../../Nodes/Components/NodeMesh.hpp"

namespace drk::BoundingVolumes::Systems {
	class AxisAlignedBoundingBoxSystem :
		public Draws::Systems::IDrawSystem,
		public drk::Systems::StorageSystem<
			Models::AxisAlignedBoundingBox,
			Components::AxisAlignedBoundingBox,
			Nodes::Components::NodeMesh
		> {
	public:
		AxisAlignedBoundingBoxSystem(
			Engine::EngineState& engineState,
			entt::registry& registry, 
			const Devices::DeviceContext& deviceContext
		);
		~AxisAlignedBoundingBoxSystem();
		void update(
			Models::AxisAlignedBoundingBox& axisAlignedBoundingBoxModel,
			const Components::AxisAlignedBoundingBox& axisAlignedBoundingBox,
			const Nodes::Components::NodeMesh& nodeMesh
		) override;
		void createResources();
		void processDirty();

		void emitDraws() override;

	protected:
		const Devices::DeviceContext& deviceContext;
		Devices::BufferView vertexBufferView;
		Devices::BufferView indexBufferView;

		// H�rit� via DrawSystem
		void updateDraw(entt::entity drawEntity, int drawIndex) override;
	};
}