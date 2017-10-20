#pragma once

#include "ExampleBase.h"

class MultiInstance : public ExampleBase
{
	GLuint vao;
	Shader *shader;

public:
	void initGLData() {
		const char *className = typeid(*this).name() + 6;
		shader = new Shader(className);

		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

		GLfloat positions[] = {
			-0.05f, 0.05f, 0.0f,
			0.05f, -0.05f, 0.0f,
			-0.05f, -0.05f, 0.0f,

			-0.05f, 0.05f, 0.0f,
			0.05f, -0.05f, 0.0f,
			0.05f, 0.05f, 0.0f
		};

		glm::vec2 offsets[100];
		int index = 0;
		for (GLfloat y = -0.9f; y < 1; y += 0.2f) {
			for (GLfloat x = -0.9f; x < 1; x += 0.2f) {
				offsets[index++] = { x, y };
			}
		}


		// 创建 buffer 并上传数据
		GLuint bo;
		glGenBuffers(1, &bo);
		glBindBuffer(GL_COPY_WRITE_BUFFER, bo);
		glBufferData(GL_COPY_WRITE_BUFFER, sizeof(positions) + sizeof(offsets), nullptr, GL_STATIC_DRAW);
		glBufferSubData(GL_COPY_WRITE_BUFFER, 0, sizeof(positions), positions);
		glBufferSubData(GL_COPY_WRITE_BUFFER, sizeof(positions), sizeof(offsets), offsets);
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);


		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// 把刚才上传了数据的 buffer 绑定到 GL_ARRAY_BUFFER，用以指定属性对应的数据源
		glBindBuffer(GL_ARRAY_BUFFER, bo);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), 0);
		glEnableVertexAttribArray(3);
		glVertexAttribDivisor(3, 1);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (GLvoid *)sizeof(positions));
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);


	}

	void renderLoop() {

		glClearColor(0.3f, 0.3f, 0.7f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader->use();
		glBindVertexArray(vao);

		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);

	}
	
};
