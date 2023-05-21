#include "noks_window.hpp"

// std
#include <stdexcept>

namespace noks {

	NoksWindow::NoksWindow(int w, int h, std::string name) : width{ w }, height{ h }, windowName{ name } {
		initWindow();
	}

	NoksWindow::~NoksWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void NoksWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}

	void NoksWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to craete window surface");
		}
	}

	void NoksWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto noksWindow = reinterpret_cast<NoksWindow*>(glfwGetWindowUserPointer(window));
		noksWindow->framebufferResized = true;
		noksWindow->width = width;
		noksWindow->height = height;
	}

}  // namespace noks
