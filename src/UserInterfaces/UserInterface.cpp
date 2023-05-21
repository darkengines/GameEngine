
#include <stack>
#include "../Scenes/Draws/SceneDraw.hpp"
#include <imgui.h>
#include "../Objects/Relationship.hpp"
#include "UserInterface.hpp"
#include "../Objects/Object.hpp"
#include <entt/entt.hpp>

namespace drk::UserInterfaces {

	void UserInterface::RenderEntityTree(
		entt::registry& registry,
		const std::function<bool(entt::entity entity)>& renderNode,
		const std::function<void(entt::entity entity)>& renderLeaf,
		std::optional<const std::function<void(entt::entity entity)>> renderAfterNode
	) {
		const auto& view = registry.view<drk::Objects::Relationship>();
		std::stack<entt::entity> parents;
		view.each(
			[&parents, &renderNode, &renderLeaf, &renderAfterNode](
				entt::entity entity,
				drk::Objects::Relationship& relationship
			) {
				while (parents.size() > relationship.depth) {
					auto parent = parents.top();
					parents.pop();
					ImGui::TreePop();
					if (renderAfterNode.has_value()) (*renderAfterNode)(parent);
				}
				if (parents.size() == relationship.depth) {
					if (relationship.firstChild != entt::null) {
						if (renderNode(entity)) {
							parents.push(entity);
						}
					} else {
						renderLeaf(entity);
					}
				}
			}
		);
	}
}