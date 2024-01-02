#include "../../Systems/System.hpp"
#include "../Models/AxisAlignedBoundingBox.hpp"
#include "../Components/AxisAlignedBoundingBox.hpp"
#include "../../Objects/Components/Dirty.hpp"
#include "../../Spatials/Components/Spatial.hpp"
#include "../../Draws/Systems/DrawSystem.hpp"

namespace drk::BoundingVolumes::Systems {
	class AxisAlignedBoundingBoxSystem :
		public Draws::Systems::DrawSystem,
		public drk::Systems::System<
		Models::AxisAlignedBoundingBox,
		Components::AxisAlignedBoundingBox
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
			const Components::AxisAlignedBoundingBox& axisAlignedBoundingBox
		);
		void createResources();
		void processDirty();

		void emitDraws();

	protected:
		Devices::DeviceContext deviceContext;
		Devices::BufferView vertexBufferView;
		Devices::BufferView indexBufferView;
	};
}