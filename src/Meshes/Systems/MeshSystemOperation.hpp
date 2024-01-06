namespace drk::Meshes::Systems {
	enum class MeshSystemOperation {
		None = 0,
		SetCamera = 1 << 0
	};
	inline MeshSystemOperation operator|(MeshSystemOperation lhs, MeshSystemOperation rhs) {
		using T = std::underlying_type_t<MeshSystemOperation>;
		return static_cast<MeshSystemOperation>(static_cast<T>(lhs) | static_cast<T>(rhs));
	}

	inline MeshSystemOperation& operator|=(MeshSystemOperation& lhs, MeshSystemOperation rhs) {
		lhs = lhs | rhs;
		return lhs;
	}
	inline MeshSystemOperation operator&(MeshSystemOperation lhs, MeshSystemOperation rhs) {
		using T = std::underlying_type_t<MeshSystemOperation>;
		return static_cast<MeshSystemOperation>(static_cast<T>(lhs) & static_cast<T>(rhs));
	}

	inline MeshSystemOperation& operator&=(MeshSystemOperation& lhs, MeshSystemOperation rhs) {
		lhs = lhs & rhs;
		return lhs;
	}
}