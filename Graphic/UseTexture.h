#pragma once

#include "ExampleBase.h"
#include <cmath>

class UseTexture : public ExampleBase
{

private:

	GLuint vao;
	GLuint textures[2];
	Shader *shader;
	GLfloat factor = 0.5f;

public:

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		__super::key_callback(window, key, scancode, action, mods);
		
		// �и� repeat �¼�����ʾ���²��ɣ������Լ���¼���º�̧���״̬
		if (key == GLFW_KEY_LEFT && action == GLFW_REPEAT)
		{
			factor -= 0.01f;
			factor = fmax(factor, 0.f);
		}

		if (key == GLFW_KEY_RIGHT && action == GLFW_REPEAT)
		{
			factor += 0.01f;
			factor = fmin(factor, 1.f);
		}

	}

	void initGLData() {
		glViewport(0, 0, WINDOW_WIDTH - 100, WINDOW_HEIGHT - 100);
		
		const char *className = typeid(*this).name() + 6;
		shader = new Shader(className);

		initTextures();

		GLfloat vertices[] = {
			// Positions // Colors // Texture Coords
			0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // Top Right
			0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom Right
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // Bottom Left
			-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f // Top Left
		};

		GLuint indices[] = {
			0,1,2,
			0,2,3
		};

		GLuint ibo, vbo;

		{
			// ������ͨ buffer ���ϴ���������
			glGenBuffers(1, &ibo);
			glBindBuffer(GL_COPY_WRITE_BUFFER, ibo);
			glBufferData(GL_COPY_WRITE_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		}
		{
			// ������ͨ buffer ���ϴ���������
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_COPY_WRITE_BUFFER, vbo);
			glBufferData(GL_COPY_WRITE_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
			glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
		}
		{
			// ���� VAO
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			// �Ѹղ��ϴ������ݵ� ibo �󶨵� GL_ELEMENT_ARRAY_BUFFER
			// ע�⣺��ʹ�����Ѿ����� GL_ELEMENT_ARRAY_BUFFER����������
			// �������µ� VAO ���Ի�Ҫ���°�һ��
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

			// �Ѹղ��ϴ������ݵ� buffer �󶨵� GL_ARRAY_BUFFER������ָ�����Զ�Ӧ������Դ
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), 0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid *)(6 * sizeof(GL_FLOAT)));
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindVertexArray(0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

		shader->use();
		// ��������   glUniform ������ shader Use ֮�����
		glUniform1i(shader->getUniformId("boxTexture"), 0);
		glUniform1i(shader->getUniformId("wallTexture"), 1);

	}

	void renderLoop() {

		glClearColor(0.3f, 0.3f, 0.7f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUniform1f(shader->getUniformId("factor"), factor);

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

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

};
