#include "../../Systems/System.hpp"
#include "../Models/AxisAlignedBoundingBox.hpp"
#include "../Components/AxisAlignedBoundingBox.hpp"
#include "../../Objects/Components/Dirty.hpp"
#include "../../Spatials/Components/Spatial.hpp"

namespace drk::BoundingVolumes::Systems {
	class AxisAlignedBoundingBoxSystem : public drk::Systems::System<Models::AxisAlignedBoundingBox, Components::AxisAlignedBoundingBox> {
	public:
		void update(
			Models::AxisAlignedBoundingBox& axisAlignedBoundingBoxModel,
			const Components::AxisAlignedBoundingBox& axisAlignedBoundingBox
		);
		void ProcessDirty() {
			auto view = registry.view<
				Components::AxisAlignedBoundingBox,
				Objects::Components::Dirty<Spatials::Components::Spatial>
			>();
			view.each([](
				entt::entity entity,
				Components::AxisAlignedBoundingBox& axisAlignedBoundingBox
				) {

				});
		}
	};
}