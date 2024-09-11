#pragma once

#include "../../Buffers/Resources/BufferResourceManager.hpp"
#include "./MeshUploadResult.hpp"
#include "../Components/MeshResource.hpp"

namespace drk::Meshes::Resources {
	class MeshResourceManager {
	protected:
		Buffers::Resources::BufferResourceManager& bufferResourceManager;
	public:
		MeshResourceManager(Buffers::Resources::BufferResourceManager& bufferResourceManager) : bufferResourceManager(
			bufferResourceManager
		) {}
		MeshUploadResult uploadMeshes(const std::vector<Components::MeshResource*>& meshInfos) {
			std::vector<std::span<Vertex>> vertices(meshInfos.size());
			std::vector<std::span<VertexIndex>> indices(meshInfos.size());
			std::transform(
				meshInfos.begin(),
				meshInfos.end(),
				vertices.data(),
				[](Components::MeshResource* mesh) {
					return std::span{mesh->vertices.data(), mesh->vertices.size()};
				}
			);
			std::transform(
				meshInfos.begin(), meshInfos.end(), indices.data(), [](Components::MeshResource* mesh) {
					return std::span{mesh->indices.data(), mesh->indices.size()};
				}
			);
			auto vertexBufferUploadResult = bufferResourceManager.upload(
				vertices,
				vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eStorageBuffer |
				vk::BufferUsageFlagBits::eTransferSrc,
				fmt::format("{0}.Vertex", typeid(MeshResourceManager).name()).c_str()
			);
			auto indexBufferUploadResult = bufferResourceManager.upload(
				indices,
				vk::BufferUsageFlagBits::eIndexBuffer, fmt::format("{0}.Index", typeid(MeshResourceManager).name()).c_str()
			);

			MeshUploadResult result = {
				.indexBuffer = indexBufferUploadResult.buffer,
				.vertexBuffer = vertexBufferUploadResult.buffer
			};

			for (auto meshInfoIndex = 0u; meshInfoIndex < meshInfos.size(); meshInfoIndex++) {
				result.meshes.push_back(
					Components::MeshBufferView{
						.IndexBufferView = indexBufferUploadResult.bufferViews[meshInfoIndex],
						.VertexBufferView = vertexBufferUploadResult.bufferViews[meshInfoIndex],
					}
				);
			}

			return result;
		}
	};
}