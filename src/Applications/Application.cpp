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
		const Graphics::EngineState& engineState,
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
		UserInterfaces::UserInterface& userInterface
	)
		: Window(window),
		  DeviceContext(deviceContext),
		  EngineState(engineState),
		  TextureSystem(textureSystem),
		  MaterialSystem(materialSystem),
		  MeshSystem(meshSystem),
		  SpatialSystem(spatialSystem),
		  ObjectSystem(objectSystem),
		  CameraSystem(cameraSystem),
		  GlobalSystem(globalSystem),
		  Loader(loader),
		  Graphics(graphics),
		  FlyCamController(flyCamController),
		  UserInterface(userInterface),
		  MainRenderContext(DeviceContext, EngineState, Graphics.GetSceneRenderTargetTexture()) {

		const auto& glfwWindow = Window.GetWindow();
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
		MaterialSystem.AddMaterialSystem(EngineState.Registry);
		MeshSystem.AddMeshSystem(EngineState.Registry);
		SpatialSystem.AddSpatialSystem(EngineState.Registry);
		ObjectSystem.AddObjectSystem(EngineState.Registry);
		CameraSystem.AddCameraSystem(EngineState.Registry);

		std::vector<Loaders::LoadResult> loadResults;

		auto defaultCamera = CameraSystem.CreateCamera(
			glm::zero<glm::vec4>(),
			glm::vec4{1.0f, 0.0f, 0.0f, 0.0f},
			glm::vec4{0.0f, 1.0f, 0.0f, 0.0f},
			glm::radians(65.0f),
			16.0f / 9.0f,
			0.1f,
			1000.0f
		);

		FlyCamController.Attach(defaultCamera);
		GlobalSystem.SetCamera(defaultCamera);

		ImGui::FileBrowser fileBrowser;

		vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
			.setLayoutCount = static_cast<uint32_t>(EngineState.DescriptorSetLayouts.size()),
			.pSetLayouts = EngineState.DescriptorSetLayouts.data(),
		};
		auto mainPipelineLayout = DeviceContext.Device.createPipelineLayout(pipelineLayoutCreateInfo);

		auto scenetexture = Graphics.GetSceneRenderTargetTexture();
		auto sceneTextureImageDescriptorSet = ImGui_ImplVulkan_AddTexture(
			EngineState.GetDefaultTextureSampler(),
			scenetexture.imageView,
			static_cast<VkImageLayout>(vk::ImageLayout::eShaderReadOnlyOptimal)
		);

		while (!glfwWindowShouldClose(Window.GetWindow())) {
			glfwPollEvents();

			const auto& frameState = EngineState.FrameStates[EngineState.FrameIndex];
			const auto& fence = frameState.Fence;

			const auto& waitForFenceResult = DeviceContext.Device.waitForFences(1, &fence, VK_TRUE, UINT64_MAX);
			auto swapchainImageIndex = Graphics.AcuireSwapchainImageIndex();
			const auto& resetFenceResult = DeviceContext.Device.resetFences(1, &fence);

			frameState.CommandBuffer.reset();
			vk::CommandBufferBeginInfo commandBufferBeginInfo = {};
			const auto& result = frameState.CommandBuffer.begin(&commandBufferBeginInfo);

			frameState.CommandBuffer.bindDescriptorSets(
				vk::PipelineBindPoint::eGraphics,
				mainPipelineLayout,
				0,
				frameState.DescriptorSets.size(),
				frameState.DescriptorSets.data(),
				0,
				nullptr
			);

			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			if (true || UserInterface.IsVisible()) {
				auto open = true;
				ImGui::Begin("Hello World!", &open, ImGuiWindowFlags_MenuBar);
				if (ImGui::BeginMenuBar()) {
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
					ImGui::EndMenuBar();
					ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
					ImGui::Image(sceneTextureImageDescriptorSet, viewportPanelSize);
				}
				ImGui::End();


				ImGui::Begin("Entities");
				for (const auto& loadResult: loadResults) {
					RenderEntityTree(loadResult.rootEntity);
				}
				ImGui::End();

				fileBrowser.Display();
				if (fileBrowser.HasSelected()) {
					auto loadResult = Loader.Load(fileBrowser.GetSelected());
					loadResults.emplace_back(std::move(loadResult));
					fileBrowser.ClearSelected();
				}
				ImGui::ShowDemoWindow();
			}
			ImGui::EndFrame();

			TextureSystem.UploadTextures();
			MeshSystem.UploadMeshes();

			MaterialSystem.StoreMaterials();
			MeshSystem.StoreMeshes();
			SpatialSystem.StoreSpatials();
			ObjectSystem.StoreObjects();
			CameraSystem.StoreCameras();

			FlyCamController.Step();

			SpatialSystem.PropagateChanges();
			CameraSystem.ProcessDirtyItems();

			MaterialSystem.UpdateMaterials();
			MeshSystem.UpdateMeshes();
			SpatialSystem.UpdateSpatials();
			ObjectSystem.UpdateObjects();
			CameraSystem.UpdateCameras();
			GlobalSystem.Update();

			EngineState.Registry.clear<Objects::Dirty<Spatials::Spatial>>();


			MainRenderContext.Render(frameState.CommandBuffer);
			Graphics.Render(frameState.CommandBuffer, swapchainImageIndex);

			frameState.CommandBuffer.end();

			vk::PipelineStageFlags submissionWaitDstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
			vk::SubmitInfo submitInfo = {
				.waitSemaphoreCount = 1,
				.pWaitSemaphores = &frameState.ImageReadySemaphore,
				.pWaitDstStageMask = &submissionWaitDstStageMask,
				.commandBufferCount = 1,
				.pCommandBuffers = &frameState.CommandBuffer,
				.signalSemaphoreCount = 1,
				.pSignalSemaphores = &frameState.ImageRenderedSemaphore,
			};

			DeviceContext.GraphicQueue.submit({submitInfo}, fence);

			Graphics.Present(swapchainImageIndex);

			if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}
		}

		DeviceContext.Device.waitIdle();
		DeviceContext.Device.destroyPipelineLayout(mainPipelineLayout);
	}

	void Application::RenderEntityTree(entt::entity entity) {
		auto relationship = EngineState.Registry.get<Objects::Relationship>(entity);
		auto object = EngineState.Registry.get<Objects::Object>(entity);

		auto child = relationship.firstChild;
		if (child != entt::null) {
			if (ImGui::TreeNode(fmt::format("{0} {1}", object.Name, relationship.childCount).c_str())) {
				while (child != entt::null) {
					RenderEntityTree(child);
					auto childRelationship = EngineState.Registry.get<Objects::Relationship>(child);
					child = childRelationship.nextSibling;
				}
				ImGui::TreePop();
			}
		} else {
			ImGui::Text(object.Name.c_str());
		}
	}

	void Application::OnWindowSizeChanged(uint32_t width, uint32_t height) {

		while (width == 0 || height == 0) {
			glfwGetFramebufferSize(Window.GetWindow(), reinterpret_cast<int*>(&width), reinterpret_cast<int*>(&height));
			glfwWaitEvents();
		}

		Graphics.SetExtent({width, height});
	}

	void Application::WaitFences() {
		std::vector<vk::Fence> fences;
		for (const auto& frameState : EngineState.FrameStates) {
			fences.push_back(frameState.Fence);
		}
		auto waitFenceResult = DeviceContext.Device.waitForFences(
			fences,
			VK_TRUE,
			UINT64_MAX
		);
	}

	void Application::SetKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		auto application = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
		application->FlyCamController.OnKeyboardEvent(key, scancode, action, mods);
		application->UserInterface.OnKeyboardEvent(key, scancode, action, mods);
	}

	void Application::CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
		auto application = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
		ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
		application->FlyCamController.OnCursorPositionEvent(xpos, ypos);
	}
	Application::~Application() {

	}
}