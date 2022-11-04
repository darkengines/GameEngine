#include "RelationshipSystem.hpp"
#include "../Spatials/Spatial.hpp"
#include "../Objects/Relationship.hpp"

namespace drk::Relationships {

	void RelationshipSystem::SortRelationships() {
		registry.sort<Spatials::Spatial>(
			[&](const entt::entity left, const entt::entity right) {
				const auto &rightRelation = registry.get<Objects::Relationship>(right);
				return left == rightRelation.parent || left == rightRelation.previousSibling;
			}
		);
	}
}