#include "PointDrawSystem.hpp"
#include "../Objects/Object.hpp"

namespace drk::Points {
	void PointDrawSystem::Update(
		Models::PointDraw& model,
		const Stores::StoreItem<Models::Point>& pointStoreItem,
		const Stores::StoreItem<Objects::Object>& objectStoreItem
	) {
		const auto& frameStoreItem = pointStoreItem.frameStoreItems[engineState.getFrameIndex()];
		model.pointItemLocation = frameStoreItem;
		model.objectItemLocation = frameStoreItem;
	}
}