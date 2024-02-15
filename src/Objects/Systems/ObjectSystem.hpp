#pragma once

#include "../../Devices/DeviceContext.hpp"
#include "../../Engine/EngineState.hpp"
#include "../Models/Object.hpp"
#include "../../Nodes/Components/Node.hpp"
#include "../../Meshes/Models/Mesh.hpp"
#include "../../Spatials/Models/Spatial.hpp"
#include "../../Spatials/Models/RelativeSpatial.hpp"
#include "../../Systems/System.hpp"
#include "../../Loaders/LoadResult.hpp"

namespace drk::Nodes::Systems {
	class ObjectSystem : public drk::Systems::System<
		Models::Object,
		Stores::StoreItem<Spatials::Models::Spatial>,
		Stores::StoreItem<Spatials::Models::RelativeSpatial>
	> {
	protected:
		const Devices::DeviceContext& DeviceContext;
		void update(
			Models::Object& objectModel,
			const Stores::StoreItem<Spatials::Models::Spatial>& spatialStoreItem,
			const Stores::StoreItem<Spatials::Models::RelativeSpatial>& relativeSpatialStoreItem
		);

	public:
		ObjectSystem(
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			entt::registry& registry
		);
		static entt::entity copyObjectEntity(
			const entt::registry& source,
			entt::registry& destination,
			entt::entity sourceEntity,
			entt::entity parent = entt::null,
			entt::entity previousSibling = entt::null
		);
	};
}