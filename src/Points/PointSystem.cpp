
#include "PointSystem.hpp"
#include "../Materials/Models/Material.hpp"

namespace drk::Points {
	void PointSystem::Update(Models::Point& model, const Components::Point& point) {
		const auto& materialModel = registry.get<Stores::StoreItem<Materials::Models::Material>>(point.materialEntity);
		model.materialItemLocation = materialModel.frameStoreItems[engineState.getFrameIndex()];
	}
}
