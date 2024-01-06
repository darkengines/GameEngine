#include "../../Graphics/GlobalSystem.hpp"
#include <entt/entt.hpp>
#include <imgui.h>

namespace drk::Cameras::Editors {
	class CameraEditor {
	public:
		inline static bool setActiveCamera(entt::entity cameraEntity, Graphics::GlobalSystem& globalSystem) {
			auto clicked = ImGui::Button("Set as active camera");
			if (clicked) {
				globalSystem.setCamera(cameraEntity);
			}
			return clicked;
		}
	};
}