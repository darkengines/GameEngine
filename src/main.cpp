#define BOOST_DI_CFG_DIAGNOSTICS_LEVEL 2
#include <nlohmann/json.hpp>
#include <taskflow/taskflow.hpp>

#include "Animations/Extensions.hpp"
#include "Applications/Application.hpp"
#include "Applications/Extentions.hpp"
#include "BoundingVolumes/Extensions.hpp"
#include "Cameras/Extensions.hpp"
#include "Configuration/Extensions.hpp"
#include "Controllers/Extensions.hpp"
#include "Devices/Extensions.hpp"
#include "FreeList/FreeList.hpp"
#include "Frustums/Extensions.hpp"
#include "Graphics/Extentions.hpp"
#include "Graphics/Graphics.hpp"
#include "Lights/Extensions.hpp"
#include "Lines/Extensions.hpp"
#include "Loaders/Extensions.hpp"
#include "Materials/Extensions.hpp"
#include "Meshes/Extensions.hpp"
#include "Nodes/Extensions.hpp"
#include "Points/Extensions.hpp"
#include "Relationships/Extensions.hpp"
#include "Scenes/Extensions.hpp"
#include "Spatials/Extensions.hpp"
#include "Spatials/Systems/SpatialSystem.hpp"
#include "Stores/Extensions.hpp"
#include "Textures/Extensions.hpp"
#include "UserInterfaces/Extensions.hpp"
#include "Windows/Extensions.hpp"
#include "implementations.hpp"

int main(int argc, char** argv) {
	drk::FreeList freeList = drk::FreeList::create(1024);
	auto a512 = freeList.allocate(512);
	auto a256 = freeList.allocate(256);
	auto b256 = freeList.allocate(256);
	drk::FreeBlock aFreeBlock = {0, 64};
	drk::FreeBlock bFreeBlock = {128, 64};
	drk::FreeBlock cFreeBlock = {64, 64};
	freeList.free(aFreeBlock);
	freeList.free(bFreeBlock);
	freeList.free(cFreeBlock);
	freeList.allocate(150);

	auto currentPath = std::filesystem::current_path();
	std::optional<std::string> modelPath;
	if (argc > 1)
		modelPath = std::string(argv[1]);

	auto injector = boost::di::make_injector(
		drk::Configuration::AddConfiguration(),
		drk::Windows::AddWindows(),
		drk::Devices::AddDevices(),
		drk::Graphics::AddGraphics(),
		drk::Cameras::AddCameras(),
		drk::Lights::AddLights(),
		drk::Loaders::AddLoaders(),
		drk::Materials::AddMaterials(),
		drk::Meshes::AddMeshes(),
		drk::Nodes::AddObjects(),
		drk::Relationships::AddRelationships(),
		drk::Spatials::AddSpatials(),
		drk::Textures::AddTextures(),
		drk::Controllers::AddControllers(),
		drk::UserInterfaces::AddUserInterfaces(),
		drk::Scenes::AddScenes(),
		drk::Points::AddPoints(),
		drk::Lines::AddLines(),
		drk::BoundingVolumes::AddBoundingVolumes(),
		drk::Frustums::AddFrustums(),
		drk::Animations::AddAnimations(),
		boost::di::bind<entt::registry>.to<entt::registry>(),
		drk::Applications::AddApplications()
	);

	auto& app = injector.create<drk::Applications::Application&>();
	auto& registry = injector.create<entt::registry&>();
	auto& spatialSystem = injector.create<drk::Spatials::Systems::SpatialSystem&>();

	spatialSystem.AddSpatialSystem(registry);
	if (modelPath.has_value()) {
		auto& loader = injector.create<drk::Loaders::AssimpLoader&>();
		auto loadResult = loader.Load(*modelPath, registry);
		app.applicationState.loadResults.emplace_back(std::move(loadResult));
	}

	auto taskflow = tf::Taskflow("Test");
	auto semaphore = tf::Semaphore(1);
	auto taskStart = taskflow.emplace([](tf::Runtime& rt) { std::cout << "\r\nSTART"; }).name("START");
	auto taskA = taskflow.emplace([]() { std::cout << "\r\nA"; }).name("A");
	auto taskB = taskflow.emplace([]() { std::cout << "\r\nB"; }).name("B");
	auto taskC = taskflow.emplace([]() { std::cout << "\r\nC"; }).name("C");
	auto taskEnd = taskflow.emplace([]() { 
		std::cout << "\r\END"; 
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}).name("END");
	taskEnd.succeed(taskA, taskB, taskC);

	auto executor = tf::Executor();
	//executor.run_until(taskflow, []() { return false; });
	//executor.wait_for_all();

	app.run();

	return 0;
}