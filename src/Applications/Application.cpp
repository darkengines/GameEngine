#include "Application.hpp"
#include "../Objects/Dirty.hpp"
#include <iostream>
#include "../Spatials/Components/SpatialEditor.hpp"
#include <entt/entt.hpp>
#include <stack>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <imgui.h>
#include <imfilebrowser.h>
#include <chrono>

namespace drk::Applications {
	Application::Application(
		const Windows::Window& window,
		const Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		Textures::TextureSystem& textureSystem,
		Materials::MaterialSystem& materialSystem,
		Meshes::MeshSystem& meshSystem,
		Spatials::SpatialSystem& spatialSystem,
		Objects::ObjectSystem& objectSystem,
		Cameras::CameraSystem& cameraSystem,
		Graphics::GlobalSystem& globalSystem,
		const Loaders::AssimpLoader& loader,
		Graphics::Graphics& graphics,
		Controllers::FlyCamController& flyCamController,
		UserInterfaces::UserInterface& userInterface,
		entt::registry& registry,
		UserInterfaces::Renderers::UserInterfaceRenderer& userInterfaceRenderer,
		Scenes::Renderers::SceneRenderer& sceneRenderer,
		Scenes::SceneSystem& sceneSystem,
		Points::PointSystem& pointSystem,
		Lines::LineSystem& lineSystem,
		Lights::Systems::PointLightSystem& pointLightSystem,
		Lights::Systems::DirectionalLightSystem& directionalLightSystem,
		Lights::Systems::SpotlightSystem& spotlightSystem,
		UserInterfaces::AssetExplorer& assetExplorer
	)
		: window(window),
		deviceContext(deviceContext),
		engineState(engineState),
		textureSystem(textureSystem),
		materialSystem(materialSystem),
		meshSystem(meshSystem),
		spatialSystem(spatialSystem),
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
		lineSystem(lineSystem),
		pointLightSystem(pointLightSystem),
		directionalLightSystem(directionalLightSystem),
		spotlightSystem(spotlightSystem),
		windowExtent(window.GetExtent()),
		assetExplorer(assetExplorer) {
		//ImGui::GetIO().IniFilename = NULL;
		const auto& glfwWindow = window.GetWindow();
		glfwSetCursorPosCallback(glfwWindow, CursorPosCallback);
		glfwSetWindowUserPointer(glfwWindow, this);
		glfwSetWindowSizeCallback(
			glfwWindow, [](GLFWwindow* window, int width, int height) {
				auto application = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
				application->OnWindowSizeChanged(width, height);
			}
		);
		ImGui_ImplGlfw_InitForVulkan(glfwWindow, true);
		auto io = ImGui::GetIO();
		glfwMakeContextCurrent(glfwWindow);
	}

	void Application::Run() {
		materialSystem.AddMaterialSystem(registry);
		meshSystem.AddMeshSystem(registry);
		spatialSystem.AddSpatialSystem(registry);
		objectSystem.AddObjectSystem(registry);
		cameraSystem.AddCameraSystem(registry);

		std::vector<Loaders::LoadResult> loadResults;

		auto defaultCamera = cameraSystem.CreateCamera(
			glm::zero<glm::vec4>(),
			glm::vec4{ 1.0f, 0.0f, 0.0f, 0.0f },
			glm::vec4{ 0.0f, 1.0f, 0.0f, 0.0f },
			glm::radians(65.0f),
			16.0f / 9.0f,
			0.1f,
			1000.0f
		);

		flyCamController.Attach(defaultCamera);
		globalSystem.SetCamera(defaultCamera);

		ImGui::FileBrowser fileBrowser;

		const auto swapchain = graphics.GetSwapchain();
		userInterfaceRenderer.SetTargetImageViews(
			{
				.extent = {swapchain.extent.width, swapchain.extent.height, 1},
				.format = swapchain.imageFormat
			}, swapchain.imageViews
		);

		std::optional<Devices::Texture> sceneTexture;
		vk::Extent3D sceneExtent{ 0, 0, 0 };
		std::optional<vk::DescriptorSet> sceneTextureImageDescriptorSet;
		vk::Extent3D sceneTextureExtent{ 0, 0, 1 };

		glm::vec2 previousMousePosition{ 0, 0 };
		auto isDemoWindowOpen = false;

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

		auto t0 = std::chrono::high_resolution_clock::now();

		while (!glfwWindowShouldClose(window.GetWindow())) {
			glfwPollEvents();

			userInterface.HandleKeyboardEvents();
			if (userInterface.IsExplorationMode()) {
				flyCamController.HandleKeyboardEvents();
			}

			auto imGuiMousePosition = ImGui::GetMousePos();
			glm::vec2 mousePosition{ imGuiMousePosition.x, imGuiMousePosition.y };
			if (!userInterface.IsExplorationMode()) {
				CursorPosCallback(window.GetWindow(), mousePosition.x, mousePosition.y);
			}

			const auto& frameState = engineState.getCurrentFrameState();
			const auto& fence = frameState.fence;

			const auto& waitForFenceResult = deviceContext.device.waitForFences(1, &fence, VK_TRUE, UINT64_MAX);
			auto swapchainImageAcquisitionResult = graphics.AcuireSwapchainImageIndex();
			if (shouldRecreateSwapchain || swapchainImageAcquisitionResult.result == vk::Result::eSuboptimalKHR ||
				swapchainImageAcquisitionResult.result == vk::Result::eErrorOutOfDateKHR) {
				RecreateSwapchain(windowExtent);
			}
			else {
				const auto& resetFenceResult = deviceContext.device.resetFences(1, &fence);

				frameState.commandBuffer.reset();
				vk::CommandBufferBeginInfo commandBufferBeginInfo = {};
				const auto& result = frameState.commandBuffer.begin(&commandBufferBeginInfo);
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

					if (shouldRecreateSwapchain) {
						graphics.RecreateSwapchain(windowExtent);
						const auto swapchain = graphics.GetSwapchain();
						userInterfaceRenderer.SetTargetImageViews(
							{
								.extent = swapchain.extent,
								.format = swapchain.imageFormat
							}, swapchain.imageViews
						);
						shouldRecreateSwapchain = false;
					}
					ImGui::DockSpaceOverViewport();
					if (ImGui::BeginMainMenuBar()) {
						if (ImGui::BeginMenu("File")) {
							if (ImGui::MenuItem("Open", "ctrl + o")) {
								fileBrowser.Open();
							}
							if (ImGui::MenuItem("Save", "ctrl + s")) {

							}
							if (ImGui::MenuItem("Close", "alt + f4")) {

							}
							ImGui::EndMenu();
						}
						if (ImGui::BeginMenu("About")) {
							isDemoWindowOpen = ImGui::MenuItem("Show demo window", "ctrl + d");
							ImGui::EndMenu();
						}
						ImGui::EndMainMenuBar();
					}
					if (isDemoWindowOpen) ImGui::ShowDemoWindow(&isDemoWindowOpen);
					auto windowExtent = window.GetExtent();
					ImGui::SetNextWindowSize(
						ImVec2(
							windowExtent.width,
							windowExtent.height - ImGui::GetTextLineHeightWithSpacing()),
						ImGuiCond_FirstUseEver
					);
					ImGui::Begin("Hello World!", &open, ImGuiWindowFlags_MenuBar);

					ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
					vk::Extent3D newSceneExtent = {
						static_cast<uint32_t>(viewportPanelSize.x),
						static_cast<uint32_t>(viewportPanelSize.y),
						1
					};
					if (sceneExtent.width != newSceneExtent.width || sceneExtent.height != newSceneExtent.height) {
						if (sceneTextureExtent.width < newSceneExtent.width ||
							sceneTextureExtent.height < newSceneExtent.height) {
							deviceContext.device.waitIdle();
							if (sceneTextureImageDescriptorSet.has_value())
								ImGui_ImplVulkan_RemoveTexture(
									sceneTextureImageDescriptorSet.value());
							if (sceneTexture.has_value()) deviceContext.destroyTexture(sceneTexture.value());
							sceneTexture = Scenes::Renderers::SceneRenderer::BuildSceneRenderTargetTexture(
								deviceContext,
								{
									newSceneExtent.width,
									newSceneExtent.height,
									1
								}
							);
							sceneTextureImageDescriptorSet = ImGui_ImplVulkan_AddTexture(
								engineState.GetDefaultTextureSampler(),
								sceneTexture->imageView,
								static_cast<VkImageLayout>(vk::ImageLayout::eShaderReadOnlyOptimal)
							);

							sceneRenderer.setTargetImageViews(
								{
									newSceneExtent,
									vk::Format::eR8G8B8A8Srgb,
								},
								{ sceneTexture->imageView }
							);
							sceneTextureExtent = vk::Extent3D{
								newSceneExtent.width,
								newSceneExtent.height,
								1
							};
						}
						else {
							sceneRenderer.setTargetExtent(newSceneExtent);
						}
						sceneExtent = newSceneExtent;
					}
					ImGui::Image(
						sceneTextureImageDescriptorSet.value(),
						viewportPanelSize,
						{ 0, 0 },
						{
							viewportPanelSize.x / sceneTextureExtent.width,
							viewportPanelSize.y / sceneTextureExtent.height
						}
						);
					ImGui::End();


					ImGui::Begin("Entities");
					renderEntities();
					ImGui::End();

					renderProperties(selectedEntity);
					renderStorageBuffers();
					assetExplorer.render(registry);

					fileBrowser.Display();
					if (fileBrowser.HasSelected()) {
						auto loadResult = loader.Load(fileBrowser.GetSelected(), registry);
						globalSystem.setPointLightCount(loadResult.pointLights.size());
						globalSystem.setDirectionalLightCount(loadResult.directionalLights.size());
						globalSystem.setSpotlightCount(loadResult.spotlights.size());
						loadResults.emplace_back(std::move(loadResult));
						fileBrowser.ClearSelected();
					}
				}
				ImGui::EndFrame();

				//Resources to GPU
				textureSystem.UploadTextures();
				meshSystem.UploadMeshes();

				//Resources to GPU
				materialSystem.Store();
				meshSystem.Store();
				pointSystem.Store();
				lineSystem.Store();
				spatialSystem.Store();
				objectSystem.Store();
				cameraSystem.Store();
				pointLightSystem.Store();
				directionalLightSystem.Store();
				spotlightSystem.Store();

				//Alterations
				flyCamController.Step();

				//Change propagations
				spatialSystem.PropagateChanges();
				cameraSystem.ProcessDirtyItems();

				//Store updates to GPU
				materialSystem.UpdateStore();
				meshSystem.UpdateStore();
				pointSystem.UpdateStore();
				lineSystem.UpdateStore();
				spatialSystem.UpdateStore();
				objectSystem.UpdateStore();
				cameraSystem.UpdateStore();
				pointLightSystem.UpdateStore();
				directionalLightSystem.UpdateStore();
				spotlightSystem.UpdateStore();
				globalSystem.Update();

				//auto draws = registry.view<Scenes::Draws::SceneDraw>();
				//registry.destroy(draws.begin(), draws.end());

				//Emit draws
				meshSystem.EmitDraws();
				pointSystem.EmitDraws();
				lineSystem.EmitDraws();

				//Stores draws to GPU
				sceneSystem.UpdateDraws();

				//Clear frame
				registry.clear<Objects::Dirty<Spatials::Components::Spatial>>();

				//Renders
				sceneRenderer.render(0, frameState.commandBuffer);

				Devices::Device::transitionLayout(
					frameState.commandBuffer,
					sceneTexture->image.image,
					sceneTexture->imageCreateInfo.format,
					vk::ImageLayout::eUndefined,
					vk::ImageLayout::eShaderReadOnlyOptimal,
					1
				);
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

				deviceContext.GraphicQueue.submit({ submitInfo }, fence);


				vk::Result presentResult;
				bool outOfDate = false;
				try {
					presentResult = graphics.Present(swapchainImageAcquisitionResult.value);
				}
				catch (const vk::OutOfDateKHRError& e) {
					outOfDate = true;
				}
				shouldRecreateSwapchain = outOfDate || presentResult == vk::Result::eSuboptimalKHR;
				if (shouldRecreateSwapchain) {
					RecreateSwapchain(windowExtent);
				}

				if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
					ImGui::UpdatePlatformWindows();
					ImGui::RenderPlatformWindowsDefault();
				}
			}
			engineState.incrementFrameIndex();
		}
		deviceContext.device.waitIdle();
		if (sceneTexture.has_value()) deviceContext.destroyTexture(sceneTexture.value());
	}

	void Application::renderEntities() {
		const auto& relationships = registry.view<Objects::Relationship>();
		relationships.each(
			[this](entt::entity entity, Objects::Relationship& relationship) {
				if (relationship.parent == entt::null) renderEntity(entity);
			}
		);
	}

	void Application::renderEntity(entt::entity entity) {
		const auto& [relationship, object] = registry.get<Objects::Relationship, Objects::Object>(entity);
		if (relationship.children.size() > 0) {
			auto isOpen = ImGui::TreeNode(
				(void*)entity,
				fmt::format("{0}", object.Name).c_str()
			);
			if (ImGui::IsItemClicked()) {
				selectedEntity = entity;
			}
			if (isOpen) {
				for (const auto& childEntity : relationship.children) {
					renderEntity(childEntity);
				}
				ImGui::TreePop();
			}
		}
		else {
			ImGui::Text(fmt::format("{0}", object.Name).c_str());
			if (ImGui::IsItemClicked()) {
				selectedEntity = entity;
			}
		}
	}

	void Application::renderProperties(entt::entity entity) {
		ImGui::Begin("Properties");
		for (auto&& curr : registry.storage()) {
			entt::id_type id = curr.first;

			if (auto& storage = curr.second; storage.contains(entity)) {
				auto typeInfo = storage.type();
				auto component = storage.find(entity);
				ImGui::Text(typeInfo.name().data());
				auto spatialComponentTypeId = entt::type_id<Spatials::Components::Spatial>();
				if (typeInfo == spatialComponentTypeId) {
					auto& spatial = registry.get<Spatials::Components::Spatial>(entity);
					if (Spatials::Components::SpatialEditor::Spatial(spatial)) {
						spatialSystem.MakeDirty(entity);
					}
				}
			}
		}
		ImGui::End();
	};

	void Application::renderStorageBuffers() {
		ImGui::Begin("StorageBuffers");
		auto frameIndex = 0;
		for (auto& frameState : engineState.frameStates) {
			for (auto& store : frameState.Stores) {
				for (auto& storeBuffer : store.second.get()->stores) {
					ImGui::Text(
						fmt::format(
							"frame:{0}, type:{1}, buffer index:{2}, count:{3}",
							frameIndex,
							store.first.name(),
							storeBuffer.get()->descriptorArrayElement,
							storeBuffer.get()->count
						).c_str());
				}
			}
			frameIndex++;
		}
		ImGui::End();
	};

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

	void Application::SetKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		auto application = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
		//application->flyCamController.OnKeyboardEvent(key, scancode, action, mods);
		//application->userInterface.OnKeyboardEvent(key, scancode, action, mods);
	}

	void Application::CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
		auto application = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
		if (application->userInterface.IsExplorationMode()) {
			application->flyCamController.OnCursorPositionEvent(xpos, ypos);
		}
	}
	Application::~Application() {

	}
	void Application::RecreateSwapchain(vk::Extent2D windowExtent) {
		graphics.RecreateSwapchain(windowExtent);
		const auto swapchain = graphics.GetSwapchain();
		userInterfaceRenderer.SetTargetImageViews(
			{
				.extent = swapchain.extent,
				.format = swapchain.imageFormat
			}, swapchain.imageViews
		);
		shouldRecreateSwapchain = false;
	}
}