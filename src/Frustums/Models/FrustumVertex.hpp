#pragma once

#include <stdint.h>
#include <type_traits>

namespace drk::Frustums::Models {
	enum class FrustumVertex: uint32_t {
		Near = 1 << 0,
		Far = 1 << 1,
		Top = 1 << 2,
		Bottom = 1 << 3,
		Left = 1 << 4,
		Right = 1 << 5
	};
	inline FrustumVertex operator|(FrustumVertex lhs, FrustumVertex rhs) {
		using T = std::underlying_type_t<FrustumVertex>;
		return static_cast<FrustumVertex>(static_cast<T>(lhs) | static_cast<T>(rhs));
	}

	inline FrustumVertex& operator|=(FrustumVertex& lhs, FrustumVertex rhs) {
		lhs = lhs | rhs;
		return lhs;
	}
	inline FrustumVertex operator&(FrustumVertex lhs, FrustumVertex rhs) {
		using T = std::underlying_type_t<FrustumVertex>;
		return static_cast<FrustumVertex>(static_cast<T>(lhs) & static_cast<T>(rhs));
	}

	inline FrustumVertex& operator&=(FrustumVertex& lhs, FrustumVertex rhs) {
		lhs = lhs & rhs;
		return lhs;
	}
}