#pragma once

#include "ExampleBase.h"

class VertexArray : public ExampleBase
{

private:

	GLuint vao;
	Shader *shader;

public:

	void initGLData() {
		const char *className = typeid(*this).name() + 6;
		shader = new Shader(className);

		glViewport(0, 0, WINDOW_WIDTH - 100, WINDOW_HEIGHT - 100);

		GLfloat vertices[] = {
			-0.99f, -0.99f, 0.0f,
			-0.99f, 0.99f, 0.0f,
			0.99f, 0.99f, 0.0f,

			-0.98f, -0.99f, 0.0f,
			1.f, 0.99f, 0.0f,
			1.f, -0.99f, 0.0f
		};

		GLfloat colors[] = {
			1.0f, 0.5f, 0.3f, 1.0f,
			0.5f, 0.7f, 1.0f, 1.0f,
			0.6f, 0.2f, 0.9f, 1.0f,

			0.3f, 0.7f, 0.8f, 1.0f,
			0.5f, 0.7f, 1.0f, 1.0f,
			1.0f, 0.5f, 0.3f, 1.0f
		};


		// ���� buffer ���ϴ�����
		GLuint bo;
		glGenBuffers(1, &bo);
		glBindBuffer(GL_COPY_WRITE_BUFFER, bo);
		glBufferData(GL_COPY_WRITE_BUFFER, sizeof(vertices) + sizeof(colors), nullptr, GL_STATIC_DRAW);
		glBufferSubData(GL_COPY_WRITE_BUFFER, 0, sizeof(vertices), vertices);
		glBufferSubData(GL_COPY_WRITE_BUFFER, sizeof(vertices), sizeof(colors), colors);
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);


		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// �Ѹղ��ϴ������ݵ� buffer �󶨵� GL_ARRAY_BUFFER������ָ�����Զ�Ӧ������Դ
		glBindBuffer(GL_ARRAY_BUFFER, bo);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), 0);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (GLvoid *)sizeof(vertices));
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);


	}

	void renderLoop() {

		glClearColor(0.3f, 0.3f, 0.7f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader->use();
		glBindVertexArray(vao);

		glDrawArrays(GL_TRIANGLES, 0, 6);


	}

};
