#pragma once
#include <type_traits>

namespace drk::Scenes::Renderers {
	enum class SceneRenderOperation {
		None = 0x00,
		Draw = 0x01,
		BindIndexBuffer = 0x02,
		BindVertexBuffer = 0x04,
		BindPipeline = 0x08,
		SetScissor = 0x10
	};

	inline SceneRenderOperation operator|(SceneRenderOperation lhs, SceneRenderOperation rhs) {
		using T = std::underlying_type_t<SceneRenderOperation>;
		return static_cast<SceneRenderOperation>(static_cast<T>(lhs) | static_cast<T>(rhs));
	}

	inline SceneRenderOperation& operator|=(SceneRenderOperation& lhs, SceneRenderOperation rhs) {
		lhs = lhs | rhs;
		return lhs;
	}
	inline SceneRenderOperation operator&(SceneRenderOperation lhs, SceneRenderOperation rhs) {
		using T = std::underlying_type_t<SceneRenderOperation>;
		return static_cast<SceneRenderOperation>(static_cast<T>(lhs) & static_cast<T>(rhs));
	}

	inline SceneRenderOperation& operator&=(SceneRenderOperation& lhs, SceneRenderOperation rhs) {
		lhs = lhs & rhs;
		return lhs;
	}
}
