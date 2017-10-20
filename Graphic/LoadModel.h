#pragma once

#include "ExampleBase.h"
#include "Camera.h"
#include <set>
#include "Model.h"


using namespace std;
using namespace glm;

class LoadModel : public ExampleBase
{

private:

	GLuint vao;
	GLuint textures[2];
	mat4 model, view, projection;


	GLfloat lastX, lastY;
	GLfloat lastTime;
	set<int> dirPress;
	Camera camera;
	Model* myModel;

public:

	void tryMoveCamera() {
		float now = glfwGetTime();
		lastTime = lastTime == 0 ? now : lastTime;
		float deltaTime = now - lastTime;
		lastTime = now;

		for each (int dir in dirPress) {
			if (dir == GLFW_KEY_W) {
				camera.move(FORWARD, deltaTime);
			} else if (dir == GLFW_KEY_S) {
				camera.move(BACKWARD, deltaTime);
			} else if (dir == GLFW_KEY_A) {
				camera.move(LEFT, deltaTime);
			} else {
				camera.move(RIGHT, deltaTime);
			}
		}
		
	}

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		__super::key_callback(window, key, scancode, action, mods);

		if (key == GLFW_KEY_W || key == GLFW_KEY_A || key == GLFW_KEY_S || key == GLFW_KEY_D)
		{
			if (action == GLFW_PRESS) {
				dirPress.insert(key);
			}
			else if (action == GLFW_RELEASE) {
				dirPress.erase(key);
			}
		}
	}

	void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
		lastX = lastX == 0 ? xpos : lastX;
		lastY = lastY == 0 ? ypos : lastY;

		GLfloat xoffset = xpos - lastX;
		GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

		lastX = xpos;
		lastY = ypos;

		camera.shake(xoffset, yoffset);
	}

	void initGLData() {
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

		const char *className = typeid(*this).name() + 6;
		shader = new Shader(className);
		shader->use();

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		myModel = new Model("nanosuit/nanosuit.obj");
	}

	void renderLoop() {

		glClearColor(0.3f, 0.3f, 0.7f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		tryMoveCamera();

		resetMatrix();


		
		mat4 model;
		model = translate(model, vec3(0.0f, -1.75f, 0.0f));
		model = scale(model, vec3(0.2f, 0.2f, 0.2f));
		mat4 normalModel = transpose(inverse(model));
		glUniformMatrix4fv(shader->getUniformId("model"), 1, GL_FALSE, value_ptr(model));
		glUniformMatrix4fv(shader->getUniformId("normalModel"), 1, GL_FALSE, value_ptr(normalModel));
		myModel->Draw(shader);

	}

	// 每一帧更新
	void resetMatrix() {
		projection = perspective(3.1415926f / 4, 1.0f*WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
		view = camera.matrix();

		glUniformMatrix4fv(shader->getUniformId("model"), 1, GL_FALSE, value_ptr(model));
		glUniformMatrix4fv(shader->getUniformId("view"), 1, GL_FALSE, value_ptr(view));
		glUniformMatrix4fv(shader->getUniformId("projection"), 1, GL_FALSE, value_ptr(projection));
	}


};
