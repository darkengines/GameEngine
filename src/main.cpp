#define BOOST_DI_CFG_DIAGNOSTICS_LEVEL 2
#include <vulkan/vulkan.hpp>
#include "implemantations.hpp"
#include "Devices/Device.hpp"
#include "Windows/Window.hpp"
#include "Windows/Extensions.hpp"
#include "Graphics/Graphics.hpp"
#include <GLFW/glfw3.h>
#include "Applications/Application.hpp"
#include "Configuration/Extensions.hpp"
#include <nlohmann/json.hpp>
#include "Windows/Window.hpp"
#include "Devices/Extensions.hpp"
#include "Devices/DeviceContext.hpp"
#include "Graphics/Extentions.hpp"
#include "Applications/Extentions.hpp"
#include "Cameras/Extensions.hpp"
#include "Lights/Extensions.hpp"
#include "Loaders/Extensions.hpp"
#include "Materials/Extensions.hpp"
#include "Meshes/Extensions.hpp"
#include "Objects/Extensions.hpp"
#include "Relationships/Extensions.hpp"
#include "Textures/Extensions.hpp"
#include "Spatials/Extensions.hpp"
#include "Controllers/Extensions.hpp"
#include "UserInterfaces/Extensions.hpp"
#include "Draws/Extentions.hpp"
#include "Stores/Extensions.hpp"
#include "Scenes/Extensions.hpp"
#include "Points/Extensions.hpp"
#include "Lines/Extensions.hpp"

int main(int argc, char** argv) {

	auto currentPath = std::filesystem::current_path();

	auto injector = boost::di::make_injector(
		drk::Configuration::AddConfiguration(),
		drk::Windows::AddWindows(),
		drk::Devices::AddDevices(),
		//drk::Draws::AddDraws(),
		//drk::Stores::AddStores(),
		drk::Graphics::AddGraphics(),
		drk::Cameras::AddCameras(),
		drk::Lights::AddLights(),
		drk::Loaders::AddLoaders(),
		drk::Materials::AddMaterials(),
		drk::Meshes::AddMeshes(),
		drk::Objects::AddObjects(),
		drk::Relationships::AddRelationships(),
		drk::Spatials::AddSpatials(),
		drk::Textures::AddTextures(),
		drk::Controllers::AddControllers(),
		drk::UserInterfaces::AddUserInterfaces(),
		drk::Scenes::AddScenes(),
		drk::Points::AddPoints(),
		drk::Lines::AddLines(),
		boost::di::bind<entt::registry>.to<entt::registry>(),
		drk::Applications::AddApplications()
	);
	auto& app = injector.create<drk::Applications::Application&>();
	app.run();

//	drk::Applications::Application application{};
//	application.Run();
	return 0;
}