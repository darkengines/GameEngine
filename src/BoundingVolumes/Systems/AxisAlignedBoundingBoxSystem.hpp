#pragma once

#include "../../Systems/System.hpp"
#include "../Models/AxisAlignedBoundingBox.hpp"
#include "../Components/AxisAlignedBoundingBox.hpp"
#include "../../Objects/Components/Dirty.hpp"
#include "../../Spatials/Components/Spatial.hpp"
#include "../../Draws/Systems/DrawSystem.hpp"
#include "../../Objects/Components/ObjectReference.hpp"
#include "../../Meshes/Components/MeshReference.hpp"

namespace drk::BoundingVolumes::Systems {
	class AxisAlignedBoundingBoxSystem :
		public Draws::Systems::DrawSystem,
		public drk::Systems::System<
			Models::AxisAlignedBoundingBox,
			Components::AxisAlignedBoundingBox,
			Objects::Components::ObjectReference,
			Meshes::Components::MeshReference
		> {
	public:
		AxisAlignedBoundingBoxSystem(
			Engine::EngineState& engineState,
			entt::registry& registry,
			Devices::DeviceContext& deviceContext
		);
		~AxisAlignedBoundingBoxSystem();
		void update(
			Models::AxisAlignedBoundingBox& axisAlignedBoundingBoxModel,
			const Components::AxisAlignedBoundingBox& axisAlignedBoundingBox,
			const Objects::Components::ObjectReference& objectReference,
			const Meshes::Components::MeshReference& meshReference
		) override;
		void createResources();
		void processDirty();

		void emitDraws() override;

	protected:
		Devices::DeviceContext deviceContext;
		Devices::BufferView vertexBufferView;
		Devices::BufferView indexBufferView;

		// Hérité via DrawSystem
		void updateDraw(entt::entity drawEntity, int drawIndex) override;
	};
}