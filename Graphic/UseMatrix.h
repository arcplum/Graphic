#pragma once

#include "ExampleBase.h"
#include "Camera.h"
#include <set>

using namespace std;
using namespace glm;

class UseMatrix : public ExampleBase
{

private:

	GLuint vao;
	GLuint textures[2];
	mat4 model, view, projection;


	GLfloat lastX, lastY;
	GLfloat lastTime;
	set<int> dirPress;
	Camera camera;

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

		initTextures();
		initVertexData();

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
	}

	void renderLoop() {

		glClearColor(0.3f, 0.3f, 0.7f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		tryMoveCamera();

		resetMatrix();

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

	}

	// 每一帧更新
	void resetMatrix() {
		projection = perspective(3.1415926f / 4, 1.0f*WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
		view = camera.matrix();

		glUniformMatrix4fv(shader->getUniformId("model"), 1, GL_FALSE, value_ptr(model));
		glUniformMatrix4fv(shader->getUniformId("view"), 1, GL_FALSE, value_ptr(view));
		glUniformMatrix4fv(shader->getUniformId("projection"), 1, GL_FALSE, value_ptr(projection));
	}



	void initTextures() {

		glGenTextures(2, textures);

		char* pics[2] = { "images/container.jpg", "images/wall.jpg" };
		int width, height, nrChannels;
		for (int i = 0; i < 2; i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textures[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_uc *data = stbi_load(pics[i], &width, &height, &nrChannels, 0);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			stbi_image_free(data);
		}
	}

	void initVertexData() {
		GLfloat vertices[] = {
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0,0,-1,
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0, 0, -1,
			0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0, 0, -1,
			0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0, 0, -1,
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0, 0, -1,
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0, 0, -1,

			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0, 0, 1,
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0, 0, 1,
			0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0, 0, 1,
			0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0, 0, 1,
			-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0, 0, 1,
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0, 0, 1,

			-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -1, 0, 0,
			-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, -1, 0, 0,
			-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -1, 0, 0,
			-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -1, 0, 0,
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -1, 0, 0,
			-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -1, 0, 0,

			0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1, 0, 0,
			0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1, 0, 0,
			0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1, 0, 0,
			0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1, 0, 0,
			0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1, 0, 0,
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1, 0, 0,

			-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0, -1, 0,
			0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0, -1, 0,
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0, -1, 0,
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0, -1, 0,
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0, -1, 0,
			-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0, -1, 0,

			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0, 1, 0,
			0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0, 1, 0,
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0, 1, 0,
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0, 1, 0,
			-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0, 1, 0,
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0, 1, 0
		};

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid *)(3 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid *)(5 * sizeof(GL_FLOAT)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
};
