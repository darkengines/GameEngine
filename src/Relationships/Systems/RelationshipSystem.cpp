#include "RelationshipSystem.hpp"
#include "../../Spatials/Components/Spatial.hpp"
#include "../../Nodes/Components/Node.hpp"

namespace drk::Relationships::Systems {

	void RelationshipSystem::SortRelationships() {
//		registry.sort<Spatials::Components::Spatial>(
//			[&](const entt::entity left, const entt::entity right) {
//				const auto &rightRelation = registry.get<Objects::Relationship>(right);
//				return left == rightRelation.parent || left == rightRelation.previousSibling;
//			}
//		);
	}
}