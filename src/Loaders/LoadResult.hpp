#pragma once

#include <vector>
#include <memory>
#include "../Meshes/MeshInfo.hpp"
#include "../Materials/Material.hpp"
#include "../Devices/Texture.hpp"

namespace drk::Loaders {
	struct LoadResult {
		std::vector<std::unique_ptr<Meshes::MeshInfo>> meshes;
		std::vector<std::unique_ptr<Materials::Material>> materials;
		std::vector<std::unique_ptr<Textures::ImageInfo>> images;
	};
}
