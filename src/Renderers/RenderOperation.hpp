#pragma once
#include <type_traits>

namespace drk::Renderers {
	enum class RenderOperation {
		None = 0x00,
		Draw = 0x01,
		BindIndexBuffer = 0x02,
		BindVertexBuffer = 0x04,
		BindPipeline = 0x08,
		SetScissor = 0x10
	};

	inline RenderOperation operator|(RenderOperation lhs, RenderOperation rhs) {
		using T = std::underlying_type_t<RenderOperation>;
		return static_cast<RenderOperation>(static_cast<T>(lhs) | static_cast<T>(rhs));
	}

	inline RenderOperation& operator|=(RenderOperation& lhs, RenderOperation rhs) {
		lhs = lhs | rhs;
		return lhs;
	}
	inline RenderOperation operator&(RenderOperation lhs, RenderOperation rhs) {
		using T = std::underlying_type_t<RenderOperation>;
		return static_cast<RenderOperation>(static_cast<T>(lhs) & static_cast<T>(rhs));
	}

	inline RenderOperation& operator&=(RenderOperation& lhs, RenderOperation rhs) {
		lhs = lhs & rhs;
		return lhs;
	}
}
