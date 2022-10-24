#include "implemantations.hpp"
#include "Devices/Device.hpp"
#include "Windows/Window.hpp"
#include "Graphics/Graphics.hpp"
#include <GLFW/glfw3.h>
#include "Applications/Application.hpp"

int main(int argc, char **argv) {

	drk::Applications::Application application{};
	application.Run();
	return 0;
}