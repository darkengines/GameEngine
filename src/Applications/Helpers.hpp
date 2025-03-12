#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <entt/entt.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include "../Nodes/Components/NodeMeshCollection.hpp"
#include "../Common/Components/Name.hpp"
#include "../Meshes/Components/Mesh.hpp"
#include "../Meshes/Vertex.hpp"
#include "../Physics/Components/Body.hpp"
#include "../Physics/ObjectLayerPairFilter.hpp"
#include "../Materials/Components/Material.hpp"
#include "../Materials/Components/MaterialReference.hpp"
#include "../BoundingVolumes/Components/AxisAlignedBoundingBox.hpp"
#include "../Nodes/Components/NodeMesh.hpp"
#include "../Nodes/Components/Node.hpp"

using namespace drk::Meshes;

namespace drk {
inline VertexIndex getMidpoint(VertexIndex v1, VertexIndex v2, std::unordered_map<uint64_t, VertexIndex> &cache, std::vector<Vertex> &vertices) {
	uint64_t key = ((uint64_t)std::min(v1, v2) << 32) | std::max(v1, v2);
	if (cache.find(key) != cache.end()) {
		return cache[key];
	}

	Vertex &vert1 = vertices[v1];
	Vertex &vert2 = vertices[v2];
	glm::vec3 midPosition = glm::normalize(glm::vec3(vert1.position) + glm::vec3(vert2.position));

	Vertex newVertex = {};
	newVertex.position = glm::vec4(midPosition, 1.0f);
	newVertex.normal = glm::vec4(midPosition, 0.0f);
	newVertex.diffuseColor = glm::vec4(1.0f);
	newVertex.textureCoordinates = glm::vec4(0.0f);

	VertexIndex index = static_cast<VertexIndex>(vertices.size());
	vertices.push_back(newVertex);
	cache[key] = index;
	return index;
}

// Generates an icosphere mesh with a given subdivision level
inline Meshes::Components::MeshResource generateIcosphere(int subdivisions) {
	Meshes::Components::MeshResource mesh;
	std::unordered_map<uint64_t, VertexIndex> midpointCache;

	float t = (1.0f + sqrt(5.0f)) / 2.0f;
	std::vector<glm::vec3> baseVertices = {
		{-1, t, 0}, {1, t, 0}, {-1, -t, 0}, {1, -t, 0}, {0, -1, t}, {0, 1, t}, {0, -1, -t}, {0, 1, -t}, {t, 0, -1}, {t, 0, 1}, {-t, 0, -1}, {-t, 0, 1}};

	for (auto &v : baseVertices) {
		v = glm::normalize(v);
		Vertex vertex = {};
		vertex.position = glm::vec4(v, 1.0f);
		vertex.normal = glm::vec4(v, 0.0f);
		vertex.diffuseColor = glm::vec4(1.0f);
		vertex.textureCoordinates = glm::vec4(0.0f);
		mesh.vertices.push_back(vertex);
	}

	std::vector<VertexIndex> baseIndices = {
		0, 11, 5, 0, 5, 1, 0, 1, 7, 0, 7, 10, 0, 10, 11,
		1, 5, 9, 5, 11, 4, 11, 10, 2, 10, 7, 6, 7, 1, 8,
		3, 9, 4, 3, 4, 2, 3, 2, 6, 3, 6, 8, 3, 8, 9,
		4, 9, 5, 2, 4, 11, 6, 2, 10, 8, 6, 7, 9, 8, 1};

	mesh.indices = baseIndices;

	for (int i = 0; i < subdivisions; ++i) {
		std::vector<VertexIndex> newIndices;
		for (size_t j = 0; j < mesh.indices.size(); j += 3) {
			VertexIndex a = mesh.indices[j];
			VertexIndex b = mesh.indices[j + 1];
			VertexIndex c = mesh.indices[j + 2];

			VertexIndex ab = getMidpoint(a, b, midpointCache, mesh.vertices);
			VertexIndex bc = getMidpoint(b, c, midpointCache, mesh.vertices);
			VertexIndex ca = getMidpoint(c, a, midpointCache, mesh.vertices);

			newIndices.insert(newIndices.end(), {a, ab, ca, b, bc, ab, c, ca, bc, ab, bc, ca});
		}
		mesh.indices = newIndices;
	}

	return mesh;
}

inline entt::entity createCube(entt::registry &registry, glm::vec3 position, glm::quat rotation, glm::vec3 size) {
	Meshes::Components::MeshResource mesh;

	// Define vertices directly in the vector
	mesh.vertices = {
		// Front Face
		{{-0.5f, -0.5f, 0.5f, 1.0f}, {0, 0, 1, 0}, {1, 0, 0, 0}, {0, 1, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}},
		{{0.5f, -0.5f, 0.5f, 1.0f}, {0, 0, 1, 0}, {1, 0, 0, 0}, {0, 1, 0, 0}, {1, 1, 1, 1}, {1, 0, 0, 0}},
		{{0.5f, 0.5f, 0.5f, 1.0f}, {0, 0, 1, 0}, {1, 0, 0, 0}, {0, 1, 0, 0}, {1, 1, 1, 1}, {1, 1, 0, 0}},
		{{-0.5f, 0.5f, 0.5f, 1.0f}, {0, 0, 1, 0}, {1, 0, 0, 0}, {0, 1, 0, 0}, {1, 1, 1, 1}, {0, 1, 0, 0}},

		// Back Face
		{{0.5f, -0.5f, -0.5f, 1.0f}, {0, 0, -1, 0}, {-1, 0, 0, 0}, {0, 1, 0, 0}, {1, 1, 1, 1}, {1, 0, 0, 0}},
		{{-0.5f, -0.5f, -0.5f, 1.0f}, {0, 0, -1, 0}, {-1, 0, 0, 0}, {0, 1, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}},
		{{-0.5f, 0.5f, -0.5f, 1.0f}, {0, 0, -1, 0}, {-1, 0, 0, 0}, {0, 1, 0, 0}, {1, 1, 1, 1}, {0, 1, 0, 0}},
		{{0.5f, 0.5f, -0.5f, 1.0f}, {0, 0, -1, 0}, {-1, 0, 0, 0}, {0, 1, 0, 0}, {1, 1, 1, 1}, {1, 1, 0, 0}},

		// Left Face
		{{-0.5f, -0.5f, -0.5f, 1.0f}, {-1, 0, 0, 0}, {0, 0, -1, 0}, {0, 1, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}},
		{{-0.5f, -0.5f, 0.5f, 1.0f}, {-1, 0, 0, 0}, {0, 0, -1, 0}, {0, 1, 0, 0}, {1, 1, 1, 1}, {1, 0, 0, 0}},
		{{-0.5f, 0.5f, 0.5f, 1.0f}, {-1, 0, 0, 0}, {0, 0, -1, 0}, {0, 1, 0, 0}, {1, 1, 1, 1}, {1, 1, 0, 0}},
		{{-0.5f, 0.5f, -0.5f, 1.0f}, {-1, 0, 0, 0}, {0, 0, -1, 0}, {0, 1, 0, 0}, {1, 1, 1, 1}, {0, 1, 0, 0}},

		// Right Face
		{{0.5f, -0.5f, 0.5f, 1.0f}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 1, 0, 0}, {1, 1, 1, 1}, {1, 0, 0, 0}},
		{{0.5f, -0.5f, -0.5f, 1.0f}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 1, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}},
		{{0.5f, 0.5f, -0.5f, 1.0f}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 1, 0, 0}, {1, 1, 1, 1}, {0, 1, 0, 0}},
		{{0.5f, 0.5f, 0.5f, 1.0f}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 1, 0, 0}, {1, 1, 1, 1}, {1, 1, 0, 0}},

		// Top Face
		{{-0.5f, 0.5f, 0.5f, 1.0f}, {0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, -1, 0}, {1, 1, 1, 1}, {0, 1, 0, 0}},
		{{0.5f, 0.5f, 0.5f, 1.0f}, {0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, -1, 0}, {1, 1, 1, 1}, {1, 1, 0, 0}},
		{{0.5f, 0.5f, -0.5f, 1.0f}, {0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, -1, 0}, {1, 1, 1, 1}, {1, 0, 0, 0}},
		{{-0.5f, 0.5f, -0.5f, 1.0f}, {0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, -1, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}},

		// Bottom Face
		{{-0.5f, -0.5f, -0.5f, 1.0f}, {0, -1, 0, 0}, {1, 0, 0, 0}, {0, 0, -1, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}},
		{{0.5f, -0.5f, -0.5f, 1.0f}, {0, -1, 0, 0}, {1, 0, 0, 0}, {0, 0, -1, 0}, {1, 1, 1, 1}, {1, 0, 0, 0}},
		{{0.5f, -0.5f, 0.5f, 1.0f}, {0, -1, 0, 0}, {1, 0, 0, 0}, {0, 0, -1, 0}, {1, 1, 1, 1}, {1, 1, 0, 0}},
		{{-0.5f, -0.5f, 0.5f, 1.0f}, {0, -1, 0, 0}, {1, 0, 0, 0}, {0, 0, -1, 0}, {1, 1, 1, 1}, {0, 1, 0, 0}}};

	// Define indices (two triangles per face)
	mesh.indices = {
		0, 1, 2, 2, 3, 0,		// Front
		4, 5, 6, 6, 7, 4,		// Back
		8, 9, 10, 10, 11, 8,	// Left
		12, 13, 14, 14, 15, 12, // Right
		16, 17, 18, 18, 19, 16,
		20, 21, 22, 22, 23, 20};

	Materials::Components::Material material = {
		.baseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
		.ambientColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
		.diffuseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
		.specularColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
		.hasTransparency = false};

	std::string meshName = "Cube";
	auto meshEntity = registry.create();

	registry.emplace<Common::Components::Name>(meshEntity, meshName);
	registry.emplace<Meshes::Components::MeshResource>(meshEntity, std::move(mesh));

	auto materialEntity = registry.create();
	std::string materialName = "Cube material";
	registry.emplace<Materials::Components::MaterialReference>(meshEntity, materialEntity);

	registry.emplace<Materials::Components::Material>(materialEntity, material);
	registry.emplace<Common::Components::Name>(materialEntity, materialName);

	auto nodeEntity = registry.create();

	Nodes::Components::Node cubeNode{};
	Nodes::Components::NodeMesh nodeMesh{
		.nodeEntity = nodeEntity,
		.meshEntity = meshEntity,
	};

	Spatials::Components::Spatial<Spatials::Components::Relative> cubeSpatial{
		.position = glm::vec4(position, 1.0f),
		.rotation = rotation,
		.scale = {size.x, size.y, size.z, 0.0f}};

	Spatials::Components::Spatial<Spatials::Components::Absolute> cubeAbsoluteSpatial{
		.position = glm::vec4(position, 1.0f),
		.rotation = rotation,
		.scale = {size.x, size.y, size.z, 0.0f}};

	registry.emplace<Nodes::Components::Node>(nodeEntity, cubeNode);
	auto nodeMeshEntity = registry.create();
	registry.emplace<Nodes::Components::NodeMesh>(nodeMeshEntity, nodeMesh);
	registry.emplace<Meshes::Components::Mesh>(nodeMeshEntity, false, false);
	registry.emplace<BoundingVolumes::Components::AxisAlignedBoundingBox>(
		nodeMeshEntity,
		BoundingVolumes::Components::AxisAlignedBoundingBox::fromMinMax(
			{-0.5f, -0.5f, -0.5f, 1.0f},
			{0.5f, 0.5f, 0.5f, 1.0f}));

	Nodes::Components::NodeMeshCollection objectMeshCollection;
	objectMeshCollection.nodeMeshes.push_back(nodeMeshEntity);
	registry.emplace<Nodes::Components::NodeMeshCollection>(nodeEntity, std::move(objectMeshCollection));

	registry.emplace<Spatials::Components::Spatial<Spatials::Components::Relative>>(nodeEntity, cubeSpatial);
	registry.emplace<Spatials::Components::Spatial<Spatials::Components::Absolute>>(nodeEntity, cubeAbsoluteSpatial);

	return nodeEntity;
}

inline void setup(entt::registry &registry) {

	auto floorEntity = createCube(registry, {0, 0, 0}, glm::identity<glm::quat>(), {10, 1, 10});
	auto cubeEntity = createCube(registry, {0, 10, 0}, glm::identity<glm::quat>(), {1, 1, 1});

	Physics::Components::Body cubeBody{
		.motionType = JPH::EMotionType::Dynamic,
		.mass = 1.0f,
		.collisionLayer = Physics::Layers::MOVING};
	JPH::BoxShapeSettings cubeBoxShapeSettings(JPH::Vec3(0.5f, 0.5f, 0.5f));
	cubeBoxShapeSettings.SetEmbedded(); // A ref counted object on the stack (base class RefTarget) should be marked as such
										// to prevent it from being freed when its reference count goes to 0.
	JPH::ShapeSettings::ShapeResult boxShapeResult = cubeBoxShapeSettings.Create();
	JPH::ShapeRefC boxShape =
		boxShapeResult
			.Get(); // We don't expect an error here, but you can check floor_shape_result for HasError() / GetError()
	cubeBody.shape = boxShape;

	registry.emplace<Physics::Components::Body>(cubeEntity, cubeBody);

	Physics::Components::Body floorBody{
		.motionType = JPH::EMotionType::Static,
		.mass = 1.0f,
		.collisionLayer = Physics::Layers::NON_MOVING};
	JPH::BoxShapeSettings floorBoxShapeSettings(JPH::Vec3(5.0f, 0.5f, 5.0f));
	floorBoxShapeSettings.SetEmbedded(); // A ref counted object on the stack (base class RefTarget) should be marked as such
										 // to prevent it from being freed when its reference count goes to 0.
	JPH::ShapeSettings::ShapeResult floorShapeResult = floorBoxShapeSettings.Create();
	JPH::ShapeRefC floorShape =
		floorShapeResult
			.Get(); // We don't expect an error here, but you can check floor_shape_result for HasError() / GetError()
	floorBody.shape = floorShape;

	registry.emplace<Physics::Components::Body>(floorEntity, floorBody);
}

inline void setupCubeOnly(entt::registry &registry) {

	auto cubeEntity = createCube(registry, {0, 10, 0}, glm::identity<glm::quat>(), {1, 1, 1});

	Physics::Components::Body cubeBody{
		.motionType = JPH::EMotionType::Dynamic,
		.mass = 1.0f,
		.collisionLayer = Physics::Layers::MOVING};
	JPH::BoxShapeSettings cubeBoxShapeSettings(JPH::Vec3(0.5f, 0.5f, 0.5f));
	cubeBoxShapeSettings.SetEmbedded(); // A ref counted object on the stack (base class RefTarget) should be marked as such
										// to prevent it from being freed when its reference count goes to 0.
	JPH::ShapeSettings::ShapeResult boxShapeResult = cubeBoxShapeSettings.Create();
	JPH::ShapeRefC boxShape = boxShapeResult.Get(); // We don't expect an error here, but you can check floor_shape_result for HasError() / GetError()
	cubeBody.shape = boxShape;

	registry.emplace<Physics::Components::Body>(cubeEntity, cubeBody);
}
inline void test(entt::registry &registry) {
	registry.view<
				Nodes::Components::NodeMeshCollection,
				Spatials::Components::Spatial<Spatials::Components::Relative>>()
		.each([&registry](
				  entt::entity nodeEntity,
				  Nodes::Components::NodeMeshCollection &nodeMeshCollection,
				  Spatials::Components::Spatial<Spatials::Components::Relative> &relativeSpatial) {
			JPH::StaticCompoundShapeSettings compoundSettings;
			for (const auto &nodeMeshEntity : nodeMeshCollection.nodeMeshes) {
				const auto &nodeMesh = registry.get<Nodes::Components::NodeMesh>(nodeMeshEntity);
				auto meshResource = registry.get<Meshes::Components::MeshResource>(nodeMesh.meshEntity);
				JPH::VertexList vertexList;
				for (auto &vertex : meshResource.vertices) {
					vertexList.emplace_back(vertex.position.x, vertex.position.y, vertex.position.z);
				}

				JPH::IndexedTriangleList indexList;
				auto indexIndex = 0;
				while (indexIndex < meshResource.indices.size()) {
					indexList.emplace_back(
						meshResource.indices[indexIndex],
						meshResource.indices[indexIndex + 1],
						meshResource.indices[indexIndex + 2],
						0);
					indexIndex += 3;
				}

				Physics::Components::Body meshBody{
					.motionType = JPH::EMotionType::Static,
					.mass = 1.0f,
					.collisionLayer = Physics::Layers::NON_MOVING};

				auto meshShapeSettings = std::make_unique<JPH::MeshShapeSettings>(vertexList, indexList);
				compoundSettings.AddShape({relativeSpatial.position.x,
										   relativeSpatial.position.y,
										   relativeSpatial.position.z},
										  {relativeSpatial.rotation.x,
										   relativeSpatial.rotation.y,
										   relativeSpatial.rotation.z,
										   relativeSpatial.rotation.w},
										  meshShapeSettings.release());
			}
			JPH::ShapeSettings::ShapeResult compoundShapeResult = compoundSettings.Create();
			JPH::ShapeRefC compoundShape = compoundShapeResult.Get();

			Physics::Components::Body nodeBody{
				.motionType = JPH::EMotionType::Static,
				.mass = 1.0f,
				.collisionLayer = Physics::Layers::NON_MOVING};
			nodeBody.shape = compoundShape;

			registry.emplace<Physics::Components::Body>(nodeEntity, nodeBody);
		});
	/*registry.view<Nodes::Components::NodeMesh>().each([&registry](entt::entity nodeMeshEntity, Nodes::Components::NodeMesh &nodeMesh) {
		auto meshResource = registry.get<Meshes::Components::MeshResource>(nodeMesh.meshEntity);
		JPH::VertexList vertexList;
		for (auto &vertex : meshResource.vertices) {
			vertexList.emplace_back(vertex.position.x, vertex.position.y, vertex.position.z);
		}

		JPH::IndexedTriangleList indexList;
		auto indexIndex = 0;
		while (indexIndex < meshResource.indices.size()) {
			indexList.emplace_back(
				meshResource.indices[indexIndex],
				meshResource.indices[indexIndex + 1],
				meshResource.indices[indexIndex + 2],
				0);
			indexIndex += 3;
		}

		Physics::Components::Body meshBody{
			.motionType = JPH::EMotionType::Static,
			.mass = 1.0f,
			.collisionLayer = Physics::Layers::NON_MOVING};

		JPH::MeshShapeSettings meshShapeSettings(vertexList, indexList);
		meshShapeSettings.SetEmbedded();
		JPH::ShapeSettings::ShapeResult meshShapeResult = meshShapeSettings.Create();
		JPH::ShapeRefC meshShape = meshShapeResult.Get();

		meshBody.shape = meshShape;

		registry.emplace<Physics::Components::Body>(nodeMesh.nodeEntity, meshBody);*/
	//});
}
} // namespace drk
