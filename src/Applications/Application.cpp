#include "Application.hpp"
#include "../Objects/Dirty.hpp"
#include "../Objects/Relationship.hpp"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <iostream>
#include <imfilebrowser.h>
#include <uv.h>
#include <GLFW/glfw3.h>

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
		Points::PointSystem& pointSystem
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
		  windowExtent(window.GetExtent()) {

		const auto& glfwWindow = window.GetWindow();
		glfwSetWindowUserPointer(glfwWindow, this);
		glfwSetWindowSizeCallback(
			glfwWindow, [](GLFWwindow* window, int width, int height) {
				auto application = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
				application->OnWindowSizeChanged(width, height);
			}
		);
		glfwSetCursorPosCallback(glfwWindow, CursorPosCallback);
		glfwSetKeyCallback(glfwWindow, SetKeyCallback);
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
			glm::vec4{1.0f, 0.0f, 0.0f, 0.0f},
			glm::vec4{0.0f, 1.0f, 0.0f, 0.0f},
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
				.extent= swapchain.extent,
				.format= swapchain.imageFormat
			}, swapchain.imageViews
		);

		std::optional<Devices::Texture> sceneTexture;
		vk::Extent3D sceneExtent{0, 0, 0};
		std::optional<vk::DescriptorSet> sceneTextureImageDescriptorSet;

		while (!glfwWindowShouldClose(window.GetWindow())) {
			glfwPollEvents();

			const auto& frameState = engineState.getCurrentFrameState();
			const auto& fence = frameState.fence;

			const auto& waitForFenceResult = deviceContext.device.waitForFences(1, &fence, VK_TRUE, UINT64_MAX);
			auto swapchainImageAcquisitionResult = graphics.AcuireSwapchainImageIndex();
			if (swapchainImageAcquisitionResult.result == vk::Result::eSuboptimalKHR ||
				swapchainImageAcquisitionResult.result == vk::Result::eErrorOutOfDateKHR) {
				graphics.RecreateSwapchain(windowExtent);
			}
			const auto& resetFenceResult = deviceContext.device.resetFences(1, &fence);

			frameState.commandBuffer.reset();
			vk::CommandBufferBeginInfo commandBufferBeginInfo = {};
			const auto& result = frameState.commandBuffer.begin(&commandBufferBeginInfo);

			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			if (true || userInterface.IsVisible()) {
				auto open = true;

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
					ImGui::EndMainMenuBar();
				}

				ImGui::Begin("Hello World!", &open, ImGuiWindowFlags_MenuBar);
				ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
				vk::Extent2D newSceneExtent = {
					static_cast<uint32_t>(viewportPanelSize.x),
					static_cast<uint32_t>(viewportPanelSize.y)
				};
				if (sceneExtent.width != newSceneExtent.width || sceneExtent.height != newSceneExtent.height) {
					if (sceneExtent.width < newSceneExtent.width || sceneExtent.height < newSceneExtent.height) {
						sceneTexture = Scenes::Renderers::SceneRenderer::BuildSceneRenderTargetTexture(
							deviceContext,
							{
								newSceneExtent.width,
								newSceneExtent.height,
								1
							}
						);

						if (sceneTextureImageDescriptorSet.has_value()) ImGui_ImplVulkan_RemoveTexture(*sceneTextureImageDescriptorSet);
						sceneTextureImageDescriptorSet = ImGui_ImplVulkan_AddTexture(
							engineState.GetDefaultTextureSampler(),
							sceneTexture->imageView,
							static_cast<VkImageLayout>(vk::ImageLayout::eShaderReadOnlyOptimal)
						);

						sceneRenderer.setTargetImageViews(
							{
								newSceneExtent,
								graphics.GetSwapchain().imageFormat,
							},
							{sceneTexture->imageView}
						);
					} else {
						sceneRenderer.setTargetExtent(newSceneExtent);
					}
					sceneExtent = vk::Extent3D{
						newSceneExtent.width,
						newSceneExtent.height,
						1
					};
				}
				ImGui::Image(*sceneTextureImageDescriptorSet, viewportPanelSize);
				ImGui::End();


				ImGui::Begin("Entities");
				for (const auto& loadResult: loadResults) {
					RenderEntityTree(loadResult.rootEntity);
				}
				ImGui::End();

				renderProperties(entt::null);

				fileBrowser.Display();
				if (fileBrowser.HasSelected()) {
					auto loadResult = loader.Load(fileBrowser.GetSelected());
					loadResults.emplace_back(std::move(loadResult));
					fileBrowser.ClearSelected();
				}
			}
			ImGui::EndFrame();

			//Resources to GPU
			textureSystem.UploadTextures();
			meshSystem.UploadMeshes();

			//Resources to GPU
			materialSystem.StoreMaterials();
			meshSystem.StoreMeshes();
			spatialSystem.StoreSpatials();
			objectSystem.StoreObjects();
			cameraSystem.StoreCameras();

			//Alterations
			flyCamController.Step();

			//Change propagations
			spatialSystem.PropagateChanges();
			cameraSystem.ProcessDirtyItems();

			//Store updates to GPU
			materialSystem.UpdateMaterials();
			meshSystem.UpdateMeshes();
			spatialSystem.UpdateSpatials();
			objectSystem.UpdateObjects();
			cameraSystem.UpdateCameras();
			globalSystem.Update();

			//Emit draws
			meshSystem.EmitDraws();
			pointSystem.EmitDraws();

			//Stores draws to GPU
			sceneSystem.UpdateDraws();

			//Clear frame
			registry.clear<Objects::Dirty<Spatials::Spatial>>();

			if (shouldRecreateSwapchain) {
				graphics.RecreateSwapchain(windowExtent);
				shouldRecreateSwapchain = true;
			}

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

			deviceContext.GraphicQueue.submit({submitInfo}, fence);


			vk::Result presentResult;
			bool outOfDate = false;
			try {
				presentResult = graphics.Present(swapchainImageAcquisitionResult);
			} catch (const vk::OutOfDateKHRError& e) {
				outOfDate = true;
			}
			shouldRecreateSwapchain = outOfDate || presentResult == vk::Result::eSuboptimalKHR;

			engineState.incrementFrameIndex();

			if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}
		}

		deviceContext.device.waitIdle();
	}

	void Application::RenderEntityTree(entt::entity entity) {
		auto relationship = registry.get<Objects::Relationship>(entity);
		auto object = registry.get<Objects::Object>(entity);

		auto child = relationship.firstChild;
		if (child != entt::null) {
			if (ImGui::TreeNode(fmt::format("{0} {1}", object.Name, relationship.childCount).c_str())) {
				while (child != entt::null) {
					RenderEntityTree(child);
					auto childRelationship = registry.get<Objects::Relationship>(child);
					child = childRelationship.nextSibling;
				}
				ImGui::TreePop();
			}
		} else {
			ImGui::Text(object.Name.c_str());
		}
	}

	void Application::renderProperties(entt::entity entity) {
		ImGui::Begin("Properties");

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
		application->flyCamController.OnKeyboardEvent(key, scancode, action, mods);
		application->userInterface.OnKeyboardEvent(key, scancode, action, mods);
	}

	void Application::CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
		auto application = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
		ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
		application->flyCamController.OnCursorPositionEvent(xpos, ypos);
	}
	Application::~Application() {

	}
}