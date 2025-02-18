#include "Application.hpp"

#include <ImGuizmo.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <chrono>
#include <entt/entt.hpp>
#include <stack>
#include <taskflow/taskflow.hpp>

#include "../Common/Components/Name.hpp"
#include "../GlmExtensions.hpp"
#include "../Lights/Components/LightPerspectiveCollection.hpp"

#undef near
#undef far

namespace drk::Applications {
Application::Application(
	const Windows::Window& window,
	Engine::EngineState& engineState,
	const Devices::DeviceContext& deviceContext,
	Textures::Systems::TextureSystem& textureSystem,
	Materials::Systems::MaterialSystem& materialSystem,
	Meshes::Systems::MeshSystem& meshSystem,
	Meshes::Systems::MeshShadowSystem& meshShadowSystem,
	Spatials::Systems::SpatialSystem& spatialSystem,
	Spatials::Systems::RelativeSpatialSystem& relativeSpatialSystem,
	Nodes::Systems::NodeSystem& objectSystem,
	Cameras::Systems::CameraSystem& cameraSystem,
	Graphics::GlobalSystem& globalSystem,
	const Loaders::AssimpLoader& loader,
	Graphics::Graphics& graphics,
	Controllers::FlyCamController& flyCamController,
	UserInterfaces::UserInterface& userInterface,
	entt::registry& registry,
	UserInterfaces::Renderers::UserInterfaceRenderer& userInterfaceRenderer,
	Scenes::Renderers::SceneRenderer& sceneRenderer,
	Scenes::Systems::SceneSystem& sceneSystem,
	Points::Systems::PointSystem& pointSystem,
	BoundingVolumes::Systems::AxisAlignedBoundingBoxSystem& axisAlignedBoundingBoxSystem,
	Frustums::Systems::FrustumSystem& frustumSystem,
	Lines::Systems::LineSystem& lineSystem,
	Lights::Systems::LightSystem& lightSystem,
	Lights::Systems::PointLightSystem& pointLightSystem,
	Lights::Systems::DirectionalLightSystem& directionalLightSystem,
	Lights::Systems::SpotlightSystem& spotlightSystem,
	Lights::Systems::LightPerspectiveSystem& lightPerspectiveSystem,
	Animations::Systems::AnimationSystem& animationSystem,
	Animations::Systems::BoneMeshSystem& boneSystem,
	Animations::Systems::BoneSpatialSystem& boneSpatialSystem,
	UserInterfaces::AssetExplorer& assetExplorer
)
	: window(window),
	  engineState(engineState),
	  deviceContext(deviceContext),
	  textureSystem(textureSystem),
	  materialSystem(materialSystem),
	  meshSystem(meshSystem),
	  meshShadowSystem(meshShadowSystem),
	  spatialSystem(spatialSystem),
	  relativeSpatialSystem(relativeSpatialSystem),
	  objectSystem(objectSystem),
	  cameraSystem(cameraSystem),
	  globalSystem(globalSystem),
	  loader(loader),
	  graphics(graphics),
	  flyCamController(flyCamController),
	  userInterface(userInterface),
	  registry(registry),
	  userInterfaceRenderer(userInterfaceRenderer),
	  sceneRenderer(sceneRenderer),
	  sceneSystem(sceneSystem),
	  pointSystem(pointSystem),
	  axisAlignedBoundingBoxSystem(axisAlignedBoundingBoxSystem),
	  frustumSystem(frustumSystem),
	  lineSystem(lineSystem),
	  lightSystem(lightSystem),
	  pointLightSystem(pointLightSystem),
	  directionalLightSystem(directionalLightSystem),
	  spotlightSystem(spotlightSystem),
	  lightPerspectiveSystem(lightPerspectiveSystem),
	  animationSystem(animationSystem),
	  boneSystem(boneSystem),
	  boneSpatialSystem(boneSpatialSystem),
	  assetExplorer(assetExplorer),
	  windowExtent(window.GetExtent()) {
	// ImGui::GetIO().IniFilename = NULL;
	const auto& glfwWindow = window.GetWindow();
	glfwSetCursorPosCallback(glfwWindow, CursorPosCallback);
	glfwSetWindowUserPointer(glfwWindow, this);
	glfwSetWindowSizeCallback(glfwWindow, [](GLFWwindow* window, int width, int height) {
		auto application = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
		application->OnWindowSizeChanged(width, height);
	});
	ImGui_ImplGlfw_InitForVulkan(glfwWindow, true);
	auto io = ImGui::GetIO();
	glfwMakeContextCurrent(glfwWindow);
}
void Application::taskflow() {
	const auto& frameState = engineState.getCurrentFrameState();
	auto taskflow = tf::Taskflow("Application");

	// Resources to GPU
	auto textureUploadTask = taskflow.emplace([&]() { textureSystem.UploadTextures(); }).name("TextureUploads");
	auto meshUploadTask = taskflow.emplace([&]() { meshSystem.uploadMeshes(); }).name("MeshUploads");

	auto animationMeshStorageTask = taskflow.emplace([&]() { animationSystem.storeMeshes(); }).name("AnimationMeshStorages");
	animationMeshStorageTask.succeed(meshUploadTask);

	auto controllerStepTask = taskflow.emplace([&]() { flyCamController.Step(applicationState.frameTime); }).name("ControllerSteps");
	auto animationUpdateTask = taskflow.emplace([&]() { animationSystem.updateAnimations(); }).name("AnimationUpdates");
	auto spatialPropagationTask = taskflow.emplace([&]() { spatialSystem.PropagateChanges(); }).name("SpatialPropagation");
	auto cameraDirtyTask = taskflow.emplace([&]() { cameraSystem.processDirtyItems(); }).name("CameraDirty");
	auto lightPerspectiveDirtyTask = taskflow.emplace([&]() { lightPerspectiveSystem.processDirtyItems(); }).name("LightPerspectiveDirty");
	auto directionalLightDirtyTask = taskflow.emplace([&]() { directionalLightSystem.processDirtyItems(); }).name("DirectionalLightDirty");
	auto pointLightDirtyTask = taskflow.emplace([&]() { pointLightSystem.processDirtyItems(); }).name("PointLightDirty");
	auto spotlighDirtyTask = taskflow.emplace([&]() { spotlightSystem.processDirtyItems(); }).name("SpotlightDirty");
	auto axisAlignedBoundingBoxDirtyTask = taskflow.emplace([&]() { axisAlignedBoundingBoxSystem.processDirty(); }).name("AxisAlignedBoundingBoxDirty");
	auto meshDrawDirtyTask = taskflow.emplace([&]() { meshSystem.processDirtyDraws(); }).name("MeshDrawDirty");

	// Resources to GPU
	auto materialStorageTask = taskflow.emplace([&]() { materialSystem.store(); }).name("MaterialStorage");
	auto meshStorageTask = taskflow.emplace([&]() { meshSystem.store(); }).name("MeshStorage");
	auto pointStorageTask = taskflow.emplace([&]() { pointSystem.store(); }).name("PointStorage");
	auto lineStorageTask = taskflow.emplace([&]() { lineSystem.store(); }).name("LineStorage");
	auto spatialStorageTask = taskflow.emplace([&]() { spatialSystem.store(); }).name("SpatialStorage");
	auto relativeSpatialStorageTask = taskflow.emplace([&]() { relativeSpatialSystem.store(); }).name("RelativeSpatialStorage");
	auto objectStorageTask = taskflow.emplace([&]() { objectSystem.store(); }).name("ObjectStorage");
	auto cameraStorageTask = taskflow.emplace([&]() { cameraSystem.store(); }).name("CameraStorage");
	auto lightStorageTask = taskflow.emplace([&]() { lightSystem.store(); }).name("LightStorage");
	auto lightPerspectiveStorageTask = taskflow.emplace([&]() { lightPerspectiveSystem.store(); }).name("LightPerspectiveStorage");
	auto pointLightStorageTask = taskflow.emplace([&]() { pointLightSystem.store(); }).name("PointLightStorage");
	auto directionalLightStorageTask = taskflow.emplace([&]() { directionalLightSystem.store(); }).name("DirectionalLightStorage");
	auto spotlightStorageTask = taskflow.emplace([&]() { spotlightSystem.store(); }).name("SpotlightStorage");
	auto axisAlignedBoundingBoxStorageTask = taskflow.emplace([&]() { axisAlignedBoundingBoxSystem.store(); }).name("AxisAlignedBoundingBoxStorage");
	auto frustumStorageTask = taskflow.emplace([&]() { frustumSystem.store(); }).name("FrustumStorage");
	auto boneStorageTask = taskflow.emplace([&]() { boneSystem.store(); }).name("BoneStorage");

	// Store updates to GPU
	auto materialStoreUpdateTask = taskflow.emplace([&]() { materialSystem.store(); }).name("MaterialStoreUpdate");
	auto meshStoreUpdateTask = taskflow.emplace([&]() { meshSystem.store(); }).name("MeshStoreUpdate");
	auto pointStoreUpdateTask = taskflow.emplace([&]() { pointSystem.store(); }).name("PointStoreUpdate");
	auto lineStoreUpdateTask = taskflow.emplace([&]() { lineSystem.store(); }).name("LineStoreUpdate");
	auto spatialStoreUpdateTask = taskflow.emplace([&]() { spatialSystem.store(); }).name("SpatialStoreUpdate");
	auto relativeSpatialStoreUpdateTask = taskflow.emplace([&]() { relativeSpatialSystem.store(); }).name("RelativeSpatialStoreUpdate");
	auto objectStoreUpdateTask = taskflow.emplace([&]() { objectSystem.store(); }).name("ObjectStoreUpdate");
	auto cameraStoreUpdateTask = taskflow.emplace([&]() { cameraSystem.store(); }).name("CameraStoreUpdate");
	auto lightStoreUpdateTask = taskflow.emplace([&]() { lightSystem.store(); }).name("LightStoreUpdate");
	auto lightPerspectiveStoreUpdateTask = taskflow.emplace([&]() { lightPerspectiveSystem.store(); }).name("LightPerspectiveStoreUpdate");
	auto pointLightStoreUpdateTask = taskflow.emplace([&]() { pointLightSystem.store(); }).name("PointLightStoreUpdate");
	auto directionalLightStoreUpdateTask = taskflow.emplace([&]() { directionalLightSystem.store(); }).name("DirectionalLightStoreUpdate");
	auto spotlightStoreUpdateTask = taskflow.emplace([&]() { spotlightSystem.store(); }).name("SpotlightStoreUpdate");
	auto axisAlignedBoundingBoxStoreUpdateTask = taskflow.emplace([&]() { axisAlignedBoundingBoxSystem.store(); }).name("AxisAlignedBoundingBoxStoreUpdate");
	auto frustumStoreUpdateTask = taskflow.emplace([&]() { frustumSystem.store(); }).name("FrustumStoreUpdate");
	auto boneStoreUpdateTask = taskflow.emplace([&]() { boneSystem.store(); }).name("BoneStoreUpdate");

	auto boneSpatialPropagationTask = taskflow.emplace([&]() { boneSpatialSystem.propagateChanges(); }).name("BoneSpatialPropagation");
	auto boneSpatialStorageTask = taskflow.emplace([&]() { boneSpatialSystem.store(); }).name("BoneSpatialStorage");
	auto boneSpatialStoreUpdateTask = taskflow.emplace([&]() { boneSpatialSystem.updateStore(); }).name("BoneSpatialStoreUpdate");

	auto skinnedMeshInstanceResourceCreationTask =
		taskflow.emplace([&]() { animationSystem.createSkinnedMeshInstanceResources(engineState.getFrameIndex()); }).name("SkinnedMeshInstanceResourceCreation");
	auto skinUpdateTask = taskflow.emplace([&]() { animationSystem.updateSkins(frameState.commandBuffer); }).name("SkinUpdates");

	// Emit draws
	auto meshDrawsTask = taskflow.emplace([&]() { meshSystem.emitDraws(); }).name("MeshDraws");
	auto meshShadowDrawsTask = taskflow.emplace([&]() { meshShadowSystem.emitDraws(); }).name("MeshShadowDraws");
	auto pointDrawsTask = taskflow.emplace([&]() { pointSystem.emitDraws(); }).name("PointDraws");
	auto lineDrawsTask = taskflow.emplace([&]() { lineSystem.emitDraws(); }).name("LineDraws");
	auto axisAlignedBoundingBoxDrawsTask = taskflow.emplace([&]() { axisAlignedBoundingBoxSystem.emitDraws(); }).name("AxisAlignedBoundingBoxDraws");
	auto frustumDrawsTask = taskflow.emplace([&]() { frustumSystem.emitDraws(); }).name("FrustumDraws");

	// Stores draws to GPU

	auto shadowDrawUpdateTask = taskflow.emplace([&]() { sceneSystem.updateShadowDraws(); }).name("ShadowDrawUpdate");
	auto drawUpdateTask = taskflow.emplace([&]() { sceneSystem.updateDraws(); }).name("DrawUpdate");



	tf::Executor executor;
	executor.run(taskflow);
}
void Application::renderGui(ApplicationState& applicationState) {
	auto io = ImGui::GetIO();
	io.MouseDrawCursor = false;
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	auto mainViewport = ImGui::GetMainViewport();
	auto mainViewportSize = mainViewport->Size;
	ImGui::SetNextWindowSize(mainViewportSize, ImGuiCond_FirstUseEver);
	ImGui::NewFrame();

	if (userInterface.IsExplorationMode()) {
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
	}
	if (true || userInterface.IsVisible()) {
		auto open = true;

		ImGui::DockSpaceOverViewport();
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Open", "ctrl + o")) {
					applicationState.fileBrowser.Open();
				}
				if (ImGui::MenuItem("Save", "ctrl + s")) {
				}
				if (ImGui::MenuItem("Close", "alt + f4")) {
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Add")) {
				if (ImGui::BeginMenu("Light")) {
					if (ImGui::MenuItem("Spot")) {
					}
					if (ImGui::MenuItem("Point")) {
						auto pointLightEntity = pointLightSystem.createPointLight();
						selectedEntity = pointLightEntity;
					}
					if (ImGui::MenuItem("Directional")) {
					}

					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Render")) {
				if (ImGui::BeginMenu("Mesh")) {
					if (ImGui::MenuItem("Material")) {
						globalSystem.setRenderStyle(1 << 4);
					}
					if (ImGui::MenuItem("UVs")) {
						globalSystem.setRenderStyle(1 << 3);
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Texture")) {
					if (ImGui::MenuItem("Normal")) {
						globalSystem.setRenderStyle(1 << 2);
					}
					if (ImGui::MenuItem("Albedo")) {
						globalSystem.setRenderStyle(1 << 1);
					}
					if (ImGui::MenuItem("PBR")) {
						globalSystem.setRenderStyle(0);
					}

					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("About")) {
				applicationState.isDemoWindowOpen = ImGui::MenuItem("Show demo window", "ctrl + d");
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
		if (applicationState.isDemoWindowOpen)
			ImGui::ShowDemoWindow(&applicationState.isDemoWindowOpen);
		auto windowExtent = window.GetExtent();
		ImGui::SetNextWindowSize(
			ImVec2(static_cast<float>(windowExtent.width), static_cast<float>(windowExtent.height) - ImGui::GetTextLineHeightWithSpacing()), ImGuiCond_FirstUseEver
		);
		ImGui::Begin("Hello World!", &open, ImGuiWindowFlags_MenuBar);
		auto contentRegionAvailable = ImGui::GetContentRegionAvail();
		applicationState.actualExtent.width = contentRegionAvailable.x;
		applicationState.actualExtent.height = contentRegionAvailable.y;
		if (applicationState.sceneTextureImageDescriptorSet.has_value()) {
			ImGui::Image(
				(ImTextureID)(void*)applicationState.sceneTextureImageDescriptorSet.value(),
				{static_cast<float>(applicationState.actualExtent.width), static_cast<float>(applicationState.actualExtent.height)},
				{0.0f, 0.0f},
				{(float)applicationState.actualExtent.width / applicationState.sceneExtent.width, (float)applicationState.actualExtent.height / applicationState.sceneExtent.height}
			);
		}

		ImGui::End();
		ImGui::Begin("Entities");
		renderEntities();
		ImGui::End();

		renderProperties(selectedEntity);
		renderStorageBuffers();
		renderAnimations();
		renderSystemInfos();
		// renderInspector();
		assetExplorer.render(registry);

		applicationState.fileBrowser.Display();
		if (applicationState.fileBrowser.HasSelected()) {
			auto loadResult = loader.Load(applicationState.fileBrowser.GetSelected(), registry);
			applicationState.loadResults.emplace_back(std::move(loadResult));
			applicationState.fileBrowser.ClearSelected();
		}
	}
	ImGui::EndFrame();
}
void Application::run() {
	ImGui::FileBrowser fileBrowser;

	auto defaultCamera = cameraSystem.createCamera(
		{0.0f, 0.0f, 0.0f, 1.0f}, glm::vec4{0.0f, 0.0f, -1.0f, 0.0f}, glm::vec4{0.0f, 1.0f, 0.0f, 0.0f}, glm::radians(65.0f), 16.0f / 9.0f, 0.1f, 1000.0f
	);

	flyCamController.Attach(defaultCamera);
	globalSystem.setCamera(defaultCamera);

	const auto swapchain = graphics.GetSwapchain();
	userInterfaceRenderer.SetTargetImageViews({.extent = {swapchain.extent.width, swapchain.extent.height, 1}, .format = swapchain.imageFormat}, swapchain.imageViews);

	std::optional<Devices::Texture> sceneTexture;
	glm::vec2 previousMousePosition{0, 0};

	/*Materials::Components::Material pointMaterial{
		.name = "Point",
		.source = "User",
		.baseColor = {0.0f, 1.0f, 0.0f, 1.0f},
		.ambientColor = {0.0f, 1.0f, 0.0f, 1.0f},
		.diffuseColor = {0.0f, 1.0f, 0.0f, 1.0f},
		.specularColor = {0.0f, 1.0f, 0.0f, 1.0f}
	};
	auto pointMaterialPointer = std::make_shared<Materials::Components::Material>(std::move(pointMaterial));

	auto pointMaterialEntity = registry.create();
	registry.emplace<std::shared_ptr<Materials::Components::Material>>(pointMaterialEntity, pointMaterialPointer);

	for (auto pointIndex = 0; pointIndex < 1024; pointIndex++) {
		auto r = glm::ballRand<float>(16.0);
		Points::Components::Point point{
			.materialEntity = pointMaterialEntity
		};

		Spatials::Components::Spatial pointSpatial{
			.relativeScale = { 1.0f, 1.0f, 1.0f, 1.0f },
			.relativeRotation = glm::quatLookAt(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
			.relativePosition = glm::vec4(r, 1.0),
			.relativeModel = glm::identity<glm::mat4>()
		};

		Objects::Relationship pointRelationship;
		Objects::Object pointObject{ .Name = "Point" };

		auto pointEntity = registry.create();

		registry.emplace<Points::Components::Point>(pointEntity, std::move(point));
		registry.emplace<Spatials::Components::Spatial>(pointEntity, std::move(pointSpatial));
		registry.emplace<Objects::Relationship>(pointEntity, std::move(pointRelationship));
		registry.emplace<Objects::Object>(pointEntity, std::move(pointObject));
	}*/
	//		for (auto pointIndex = 0; pointIndex < 2097152 / 1024; pointIndex++) {
	//			auto r = glm::ballRand<float>(16.0);
	//			Lines::Components::Line line{
	//				.materialEntity = pointMaterialEntity
	//			};
	//
	//			Spatials::Components::Spatial lineSpatial{
	//				.relativeScale = {1.0f, 1.0f, 1.0f, 1.0f},
	//				.relativeRotation = glm::quatLookAt(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
	//				.relativePosition = glm::vec4(r, 1.0),
	//				.relativeModel = glm::identity<glm::mat4>()
	//			};
	//
	//			Objects::Relationship lineRelationship;
	//			Objects::Object lineObject{.Name = "Line"};
	//
	//			auto lineEntity = registry.create();
	//
	//			registry.emplace<Lines::Components::Line>(lineEntity, std::move(line));
	//			registry.emplace<Spatials::Components::Spatial>(lineEntity, std::move(lineSpatial));
	//			registry.emplace<Objects::Relationship>(lineEntity, std::move(lineRelationship));
	//			registry.emplace<Objects::Object>(lineEntity, std::move(lineObject));
	//		}

	auto windowExtent = window.GetExtent();

	applicationState.currentTime = std::chrono::high_resolution_clock::now();

	while (!glfwWindowShouldClose(window.GetWindow())) {
		glfwPollEvents();
		auto newTime = std::chrono::high_resolution_clock::now();
		applicationState.frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - applicationState.currentTime).count();
		applicationState.currentTime = newTime;
		userInterface.HandleKeyboardEvents();
		if (userInterface.IsExplorationMode()) {
			flyCamController.HandleKeyboardEvents();
		}

		auto imGuiMousePosition = ImGui::GetMousePos();
		glm::vec2 mousePosition{imGuiMousePosition.x, imGuiMousePosition.y};
		if (!userInterface.IsExplorationMode()) {
			CursorPosCallback(window.GetWindow(), mousePosition.x, mousePosition.y);
		}

		const auto& frameState = engineState.getCurrentFrameState();
		const auto& fence = frameState.fence;
		const auto& waitForFenceResult = deviceContext.device.waitForFences(1, &fence, VK_TRUE, UINT64_MAX);
		const auto& resetFenceResult = deviceContext.device.resetFences(1, &fence);
		auto swapchainImageAcquisitionResult = graphics.AcquireSwapchainImageIndex();

		if (shouldRecreateSwapchain || swapchainImageAcquisitionResult.result == vk::Result::eSuboptimalKHR ||
			swapchainImageAcquisitionResult.result == vk::Result::eErrorOutOfDateKHR) {
			RecreateSwapchain(windowExtent);
			swapchainImageAcquisitionResult = graphics.AcquireSwapchainImageIndex();
		}

		if (applicationState.sceneExtent.width != applicationState.actualExtent.width || applicationState.sceneExtent.height != applicationState.actualExtent.height) {
			if (applicationState.sceneExtent.width < applicationState.actualExtent.width || applicationState.sceneExtent.height < applicationState.actualExtent.height) {
				if (applicationState.sceneTextureImageDescriptorSet.has_value())
					ImGui_ImplVulkan_RemoveTexture(applicationState.sceneTextureImageDescriptorSet.value());
				if (sceneTexture.has_value())
					deviceContext.destroyTexture(sceneTexture.value());
				sceneTexture =
					Scenes::Renderers::SceneRenderer::BuildSceneRenderTargetTexture(deviceContext, {applicationState.actualExtent.width, applicationState.actualExtent.height, 1});
				applicationState.sceneTextureImageDescriptorSet = (vk::DescriptorSet)ImGui_ImplVulkan_AddTexture(
					(VkSampler)engineState.GetDefaultTextureSampler(), (VkImageView)sceneTexture->imageView, static_cast<VkImageLayout>(vk::ImageLayout::eShaderReadOnlyOptimal)
				);

				sceneRenderer.setTargetImageViews(
					{
						applicationState.actualExtent,
						vk::Format::eR8G8B8A8Srgb,
					},
					{sceneTexture->imageView}
				);
				applicationState.sceneExtent = vk::Extent3D{applicationState.actualExtent.width, applicationState.actualExtent.height, 1};
			} else {
				//sceneRenderer.setTargetExtent(applicationState.actualExtent);
			}
		}

		//				ImGuizmo::SetDrawlist();
		//				float windowWidth = (float) viewportPanelSize.x;
		//				float windowHeight = (float) viewportPanelSize.y;
		//				auto viewport = ImGui::GetWindowViewport();
		//				ImGuizmo::SetRect(viewport->WorkPos.x, viewport->WorkPos.y, viewport->Size.x, viewport->Size.y);
		//
		//				const auto& camera = registry.get<Cameras::Components::Camera>(engineState.cameraEntity);
		//				auto perspective = camera.perspective;
		//				perspective[1][1] *= -1.0f;
		//				ImGuizmo::DrawGrid(
		//					glm::value_ptr(camera.view),
		//					glm::value_ptr(perspective),
		//					glm::value_ptr(glm::identity<glm::mat4>()),
		//					100.f
		//				);

		globalSystem.GlobalSynchronizationState.Reset(); //*
		
		// Resources to GPU
		textureSystem.UploadTextures();

		meshSystem.uploadMeshes();
			animationSystem.storeMeshes();

		flyCamController.Step(applicationState.frameTime);

		// Alterations
		animationSystem.updateAnimations();
			// Change propagations
			spatialSystem.PropagateChanges();
				//ProcessDirtyItems
				cameraSystem.processDirtyItems();
				lightPerspectiveSystem.processDirtyItems();
				directionalLightSystem.processDirtyItems();
				pointLightSystem.processDirtyItems();
				spotlightSystem.processDirtyItems();
				axisAlignedBoundingBoxSystem.processDirty();
				//frustumSystem.processDirty();

				// ??
				meshSystem.processDirtyDraws();
		

		// Resources to GPU
		materialSystem.store();
		meshSystem.store();
		pointSystem.store();
		lineSystem.store();
		spatialSystem.store();
		relativeSpatialSystem.store();
		objectSystem.store();
		cameraSystem.store();
		lightSystem.store();
		lightPerspectiveSystem.store();
		pointLightSystem.store();
		directionalLightSystem.store();
		spotlightSystem.store();
		axisAlignedBoundingBoxSystem.store();
		frustumSystem.store();
		boneSystem.store();

		// Store updates to GPU
		materialSystem.updateStore();
		meshSystem.updateStore();
		pointSystem.updateStore();
		lineSystem.updateStore();
		spatialSystem.updateStore();
		relativeSpatialSystem.updateStore();
		objectSystem.updateStore();
		cameraSystem.updateStore();
		lightSystem.updateStore();
		lightPerspectiveSystem.updateStore();
		pointLightSystem.updateStore();
		directionalLightSystem.updateStore();
		spotlightSystem.updateStore();
		axisAlignedBoundingBoxSystem.updateStore();
		boneSystem.updateStore();
		frustumSystem.updateStore();
		globalSystem.update();

		boneSpatialSystem.propagateChanges();
		boneSpatialSystem.store();
		boneSpatialSystem.updateStore();
		animationSystem.createSkinnedMeshInstanceResources(engineState.getFrameIndex());

		frameState.commandBuffer.reset();
		vk::CommandBufferBeginInfo commandBufferBeginInfo = {};
		const auto& result = frameState.commandBuffer.begin(&commandBufferBeginInfo);

		animationSystem.updateSkins(frameState.commandBuffer);
		// auto draws = registry.view<Scenes::Draws::SceneDraw>();
		// registry.destroy(draws.begin(), draws.end());

		// Emit draws
		meshSystem.emitDraws();
		meshShadowSystem.emitDraws();
		pointSystem.emitDraws();
		lineSystem.emitDraws();
		axisAlignedBoundingBoxSystem.emitDraws();
		frustumSystem.emitDraws();

		// Stores draws to GPU
		sceneSystem.updateShadowDraws();
		sceneSystem.updateDraws();

		// Clear frame
		registry.clear<Common::Components::Dirty<Spatials::Components::Spatial<Spatials::Components::Relative>>>();
		registry.clear<Common::Components::Dirty<Spatials::Components::Spatial<Spatials::Components::Absolute>>>();

		// Renders
		sceneRenderer.render(0, frameState.commandBuffer);

		renderGui(applicationState);

		if (sceneTexture.has_value()) {
			Devices::Device::transitionLayout(
				frameState.commandBuffer, sceneTexture->image.image, sceneTexture->imageCreateInfo.format, vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal, 1
			);
		}
		userInterfaceRenderer.render(swapchainImageAcquisitionResult.value, frameState.commandBuffer);

		frameState.commandBuffer.end();

		vk::PipelineStageFlags submissionWaitDstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		vk::SubmitInfo submitInfo = {
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &frameState.imageReadySemaphore,
			.pWaitDstStageMask = &submissionWaitDstStageMask,
			.commandBufferCount = 1,
			.pCommandBuffers = &frameState.commandBuffer,
			.signalSemaphoreCount = 1,
			.pSignalSemaphores = &frameState.imageRenderedSemaphore,
		};

		deviceContext.GraphicQueue.submit({submitInfo}, fence);

		vk::Result presentResult;
		bool outOfDate = false;
		try {
			presentResult = graphics.Present(swapchainImageAcquisitionResult.value);
		} catch (const vk::OutOfDateKHRError& e) {
			outOfDate = true;
		}

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}
	engineState.incrementFrameIndex();

	deviceContext.device.waitIdle();
	if (sceneTexture.has_value())
		deviceContext.destroyTexture(sceneTexture.value());
}

void Application::OnWindowSizeChanged(uint32_t width, uint32_t height) {
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window.GetWindow(), reinterpret_cast<int*>(&width), reinterpret_cast<int*>(&height));
		glfwWaitEvents();
	}
	if (windowExtent.width != width || windowExtent.height != height) {
		windowExtent.width = width;
		windowExtent.height = height;
		shouldRecreateSwapchain = true;
	}
}

void Application::CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
	auto application = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
	if (application->userInterface.IsExplorationMode()) {
		application->flyCamController.OnCursorPositionEvent(xpos, ypos);
	}
}
void Application::RecreateSwapchain(vk::Extent2D windowExtent) {
	graphics.RecreateSwapchain(windowExtent);
	const auto swapchain = graphics.GetSwapchain();
	userInterfaceRenderer.SetTargetImageViews({.extent = swapchain.extent, .format = swapchain.imageFormat}, swapchain.imageViews);
	shouldRecreateSwapchain = false;
}
}  // namespace drk::Applications