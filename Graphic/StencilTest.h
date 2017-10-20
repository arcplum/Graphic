#pragma once

#include "ExampleBase.h"
#include "Camera.h"
#include <set>
#include <map>

using namespace std;
using namespace glm;

class StencilTest : public ExampleBase
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
			}
			else if (dir == GLFW_KEY_S) {
				camera.move(BACKWARD, deltaTime);
			}
			else if (dir == GLFW_KEY_A) {
				camera.move(LEFT, deltaTime);
			}
			else {
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

	// 每一帧更新
	void resetMatrix() {
		projection = perspective(3.1415926f / 4, 1.0f*WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
		//model = rotate(model, 0.01f, vec3(0.5f, 1.0f, 0.0f));
		view = camera.matrix();

		glUniformMatrix4fv(shader->getUniformId("model"), 1, GL_FALSE, value_ptr(model));
		glUniformMatrix4fv(shader->getUniformId("view"), 1, GL_FALSE, value_ptr(view));
		glUniformMatrix4fv(shader->getUniformId("projection"), 1, GL_FALSE, value_ptr(projection));
	}

	void initGLData() {
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

		const char *className = typeid(*this).name() + 6;
		shader = new Shader(className);
		shader->use();

		initTextures();
		initVertexData();

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);

		glEnable(GL_STENCIL_TEST);
		glStencilMask(0xff);

	}

	void renderLoop() {
		glClearColor(0.3f, 0.3f, 0.7f, 1.f);
		glClearStencil(0);
		glClearDepth(1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		tryMoveCamera();

		resetMatrix();
		
		glDepthMask(GL_FALSE);
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 1, 0xff);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

 		// mirror mask and mirror
 		glBindVertexArray(planeVAO);
 		model = mat4();
		model = scale(model, vec3(0.5, 1, 0.5));
		glUniform1i(shader->getUniformId("beBorder"), 1);
		glUniformMatrix4fv(shader->getUniformId("model"), 1, GL_FALSE, value_ptr(model));
 		glDrawArrays(GL_TRIANGLES, 0, 6);

		glStencilFunc(GL_EQUAL, 1, 0xff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glDepthMask(GL_TRUE);

		// mirror cubes
		glBindVertexArray(cubeVAO);
		model = mat4();
		model = translate(model, vec3(0, -1.5, 0));
		model = scale(model, vec3(1, -1, 1));
		glUniform1i(shader->getUniformId("boxTexture"), 1);
		glUniform1i(shader->getUniformId("beBorder"), 0);
		glUniformMatrix4fv(shader->getUniformId("model"), 1, GL_FALSE, value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);


		glDisable(GL_STENCIL_TEST);

		// cubes
		glBindVertexArray(cubeVAO);
		model = mat4();
		model = translate(model, vec3(0, 0.5, 0));
		glUniform1i(shader->getUniformId("boxTexture"), 1);
		glUniform1i(shader->getUniformId("beBorder"), 0);
		glUniformMatrix4fv(shader->getUniformId("model"), 1, GL_FALSE, value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);

	}



	void initTextures() {
		glGenTextures(3, textures);

		char* pics[3] = { "images/container.jpg", "images/cat.png", "images/awesomeface.png" };
		int width, height, nrChannels;
		for (int i = 0; i < 3; i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textures[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_uc *data = stbi_load(pics[i], &width, &height, &nrChannels, 0);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, i == 0 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			stbi_image_free(data);
		}
	}

	void initVertexData() {
		GLfloat cubeVertices[] = {
			// Positions          // Texture Coords
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
			0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
			0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
			0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
			-0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

			-0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
			-0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
			-0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

			0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
			0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
			0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
			0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
			0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
			0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
			0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
			-0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f
		};
		GLfloat planeVertices[] = {
			// Positions          // Texture Coords (note we set these higher than 1 that together with GL_REPEAT as texture wrapping mode will cause the floor texture to repeat)
			5.0f, -0.5f, 5.0f, 1.0f, 0.0f,
			-5.0f, -0.5f, 5.0f, 0.0f, 0.0f,
			-5.0f, -0.5f, -5.0f, 0.0f, 1.0f,

			5.0f, -0.5f, 5.0f, 1.0f, 0.0f,
			-5.0f, -0.5f, -5.0f, 0.0f, 1.0f,
			5.0f, -0.5f, -5.0f, 1.0f, 1.0f
		};
		
		// Setup cube VAO
		GLuint cubeVBO;
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		glBindVertexArray(cubeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glBindVertexArray(0);
		// Setup plane VAO
		GLuint planeVBO;
		glGenVertexArrays(1, &planeVAO);
		glGenBuffers(1, &planeVBO);
		glBindVertexArray(planeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glBindVertexArray(0);

	}

	private:
		GLuint planeVAO, cubeVAO;
};
