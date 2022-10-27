#include "RelationshipSystem.hpp"
#include "../Spatials/Spatial.hpp"
#include "../Objects/Relationship.hpp"

namespace drk::Relationships {

	void RelationshipSystem::SortRelationships() {
		Registry.sort<Spatials::Spatial>(
			[&](const entt::entity left, const entt::entity right) {
				const auto &rightRelation = EngineState.Registry.get<Objects::Relationship>(right);
				return left == rightRelation.parent || left == rightRelation.previousSibling;
			}
		);
	}
}