#pragma once

#include <assimp/mesh.h>

#include <entt/entt.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

#include "../Animations/Components/Animation.hpp"
#include "../Lights/Components/DirectionalLight.hpp"
#include "../Lights/Components/PointLight.hpp"
#include "../Lights/Components/Spotlight.hpp"
#include "../Materials/Components/Material.hpp"
#include "../Meshes/Components/MeshResource.hpp"
#include "../Textures/ImageInfo.hpp"
#include <fmt/format.h>

namespace drk::Loaders
{
  struct LoadResult
  {
    LoadResult() = default;
    LoadResult(LoadResult&& x) = default;
    LoadResult(const LoadResult& x) = delete;
    LoadResult& operator=(const LoadResult& other) = delete;

    std::vector<Lights::Components::PointLight> pointLights;
    std::vector<Lights::Components::DirectionalLight> directionalLights;
    std::vector<Lights::Components::Spotlight> spotlights;
    std::vector<Animations::Components::Animation> animations;
    std::unordered_map<uint32_t, entt::entity> materialIdEntityMap;
    std::unordered_map<uint32_t, entt::entity> meshIdEntityMap;
    entt::entity rootEntity = entt::null;
    std::filesystem::path filePath;
    std::string getEntityExternalId(const std::string& entityName)
    {
      return fmt::format("{0}/{1}", filePath.string(), entityName);
    }
  };
}  // namespace drk::Loaders
