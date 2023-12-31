#include "../../Systems/System.hpp"
#include "../Components/Frustum.hpp"
#include "../Models/Frustum.hpp"

namespace drk::Frustums::Systems {
	class FrustumSystem: public drk::Systems::System<Models::Frustum, Components::Frustum> {
		void update(Models::Frustum& frustumModel, const Components::Frustum& frustumComponent);
	};
}