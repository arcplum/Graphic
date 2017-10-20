#pragma once

#include "ExampleBase.h"



class CreateGLContext : public ExampleBase
{

public:

	void initGLData() {

		// ���޳���Ҫ�ֶ�����
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glFrontFace(GL_CCW);

		glViewport(0, 0, WINDOW_WIDTH - 100, WINDOW_HEIGHT - 100);

	}

	void renderLoop() {

		glClearColor(0.3f, 0.3f, 0.7f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

	}

};
