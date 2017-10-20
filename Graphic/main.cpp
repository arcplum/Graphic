#include <iostream>
#include <functional>
#include "CreateGLContext.h"
#include "VertexArray.h"
#include "MultiInstance.h"
#include "UseTexture.h"
#include "UseMatrix.h"
#include "StencilTest.h"
#include "BlendColor.h"
#include "LoadModel.h"
#include "LightModel.h"

using namespace std;

int WINDOW_WIDTH = 1600, WINDOW_HEIGHT = 900;

ExampleBase *rb = new LightModel();

GLFWwindow* initGLFW() {
	// 初始化 glfw 窗口辅助库
	if (!glfwInit())
	{
		cout << "Error::GLFW could not initialize GLFW!" << endl;
		return nullptr;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);


	// 创建窗口
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "title-title-title", NULL, NULL);
	if (!window)
	{
		cout << "Error::GLFW could not create window!" << endl;
		glfwTerminate();
		return nullptr;
	}

	glfwSetWindowPos(window, 0, 0);
	// 创建的窗口的context指定为当前context
	glfwMakeContextCurrent(window);

	auto key_callback = [](GLFWwindow* window, int key, int scancode, int action, int mods){
		rb->key_callback(window, key, scancode, action, mods); 
	};

	auto mouse_callback = [](GLFWwindow* window, double xpos, double ypos) {
		rb->mouse_callback(window, xpos, ypos);
	};

	auto scroll_callback = [](GLFWwindow* window, double xoffset, double yoffset) {
		rb->scroll_callback(window, xoffset, yoffset);
	};

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return window;
}

bool initGLEW() {
	// 初始化 glew api辅助库
	GLenum status = glewInit();
	if (status != GLEW_OK)
	{
		cout << "Error::GLEW glew version:" << glewGetString(GLEW_VERSION)
			<< " error string:" << glewGetErrorString(status) << endl;
		glfwTerminate();
		
		return false;
	}
	return true;
}

int main() {
	GLFWwindow *window = initGLFW();
	if (!window) {
		system("pause");
		return -1;
	}

	if (!initGLEW()) {
		system("pause");
		return -1;
	}

	rb->initGLData();

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		rb->renderLoop();

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}
