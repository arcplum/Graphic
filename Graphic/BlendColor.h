#pragma once

#include "ExampleBase.h"
#include "Camera.h"
#include <set>
#include <map>

using namespace std;
using namespace glm;

class BlendColor : public ExampleBase
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
		glDepthFunc(GL_LESS); // 默认

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD); // 默认

	}

	void renderLoop() {

		glClearColor(0.3f, 0.3f, 0.7f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		tryMoveCamera();

		resetMatrix();

		// Cubes
		glBindVertexArray(cubeVAO);
		glUniform1i(shader->getUniformId("boxTexture"), 0);
		model = mat4();
		model = translate(model, vec3(-1.0f, 0.0f, -1.0f));
		glUniformMatrix4fv(shader->getUniformId("model"), 1, GL_FALSE, value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		model = mat4();
		model = translate(model, vec3(2.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(shader->getUniformId("model"), 1, GL_FALSE, value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// Floor
		glBindVertexArray(planeVAO);
		model = mat4();
		glUniformMatrix4fv(shader->getUniformId("model"), 1, GL_FALSE, value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Sort windows
		map<GLfloat, vec3> sorted;
		for (GLuint i = 0; i < windows.size(); i++)
		{
			GLfloat distance = length(camera.Position - windows[i]);
			sorted[distance] = windows[i];
		}

		glDepthMask(GL_FALSE);

		glUniform1i(shader->getUniformId("boxTexture"), 1);
		glBindVertexArray(transparentVAO);
		for each (auto it in sorted)
		{
			model = mat4();
			model = translate(model, it.second);
			glUniformMatrix4fv(shader->getUniformId("model"), 1, GL_FALSE, value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		glDepthMask(GL_TRUE);

		glBindVertexArray(0);

	}



	void initTextures() {
		glGenTextures(2, textures);

		char* pics[2] = { "images/container.jpg", "images/window.png" };
		int width, height, nrChannels;
		for (int i = 0; i < 2; i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textures[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_uc *data = stbi_load(pics[i], &width, &height, &nrChannels, 0);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, i == 1 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			stbi_image_free(data);
		}
	}

	void initVertexData() {
		windows.push_back(vec3(-1.5f, 0.0f, -0.48f));
		windows.push_back(vec3(1.5f, 0.0f, 0.51f));
		windows.push_back(vec3(0.0f, 0.0f, 0.7f));
		windows.push_back(vec3(-0.3f, 0.0f, -2.3f));
		windows.push_back(vec3(0.5f, 0.0f, -0.6f));

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
		GLfloat transparentVertices[] = {
			// Positions         // Texture Coords (swapped y coordinates because texture is flipped upside down)
			0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, -0.5f, 0.0f, 0.0f, 1.0f,
			1.0f, -0.5f, 0.0f, 1.0f, 1.0f,

			0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
			1.0f, -0.5f, 0.0f, 1.0f, 1.0f,
			1.0f, 0.5f, 0.0f, 1.0f, 0.0f
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
		// Setup transparent plane VAO
		GLuint transparentVBO;
		glGenVertexArrays(1, &transparentVAO);
		glGenBuffers(1, &transparentVBO);
		glBindVertexArray(transparentVAO);
		glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glBindVertexArray(0);
	}

	private:
		GLuint transparentVAO, planeVAO, cubeVAO;
		vector<vec3> windows;
};
