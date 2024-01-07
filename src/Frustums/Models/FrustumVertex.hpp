#pragma once

#include <stdint.h>
#include <type_traits>

namespace drk::Frustums::Models {
	enum class FrustumVertex: uint32_t {
		Near = 0,
		Far = 1 << 0,
		Top = 1 << 1,
		Bottom = 1 << 2,
		Left = 1 << 3,
		Right = 1 << 4
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