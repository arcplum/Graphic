#pragma once

#include <iostream>

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <GLM\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Shader.h"

extern int WINDOW_WIDTH, WINDOW_HEIGHT;

class ExampleBase
{

protected:
	Shader *shader;

public:
	ExampleBase() {};
	~ExampleBase() {};

	virtual void initGLData() = 0;
	virtual void renderLoop() = 0;
	virtual void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}

	virtual void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {

	}

	virtual void mouse_callback(GLFWwindow* window, double xpos, double ypos) {

	}


};

