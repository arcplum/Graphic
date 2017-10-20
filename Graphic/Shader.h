#ifndef _SHADER_H_
#define _SHADER_H_

#include <GL/glew.h>
#include <iterator>     // std::istreambuf_iterator
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

struct ShaderSource
{
	GLenum shaderType;
	string source;
	ShaderSource(GLenum type, string s)
		:shaderType(type), source(s) {}
};

class Shader
{
public:
	Shader(const char* shaderPath) :program(0) {
		vector<ShaderSource> sourceStrs;
		if (loadShaderSource(shaderPath, sourceStrs)) {
			compileShaders(sourceStrs);
		}
	}

	~Shader() {
		if (this->program) {
			glDeleteProgram(this->program);
		}
	}

	void use() {
		glUseProgram(this->program);
	}

	GLint getUniformId(const char *name) {
		return glGetUniformLocation(program, name);
	}

	GLint getUniformId(string name) {
		return getUniformId(name.c_str());
	}

private:
	GLuint program;

	void compileShaders(vector<ShaderSource>& shaderStrs)
	{
		std::vector<GLuint> shaderObjectIdVec;
		bool bSuccess = true;
		// 编译shader object
		for (size_t i = 0; i < shaderStrs.size(); ++i)
		{
			GLuint shaderId = glCreateShader(shaderStrs[i].shaderType);
			const char *c_str = shaderStrs[i].source.c_str();
			glShaderSource(shaderId, 1, &c_str, NULL);
			glCompileShader(shaderId);
			GLint compileStatus = 0;
			glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus); // 检查编译状态
			if (compileStatus == GL_FALSE) // 获取错误报告
			{
				GLint maxLength = 0;
				glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);
				std::vector<GLchar> errLog(maxLength);
				glGetShaderInfoLog(shaderId, maxLength, &maxLength, &errLog[0]);
				std::cout << "Error::Shader file compiled failed," << &errLog[0] << std::endl;
				bSuccess = false;
			}
			shaderObjectIdVec.push_back(shaderId);
		}
		// 链接shader program
		if (bSuccess)
		{
			this->program = glCreateProgram();
			for (size_t i = 0; i < shaderObjectIdVec.size(); ++i)
			{
				glAttachShader(this->program, shaderObjectIdVec[i]);
			}
			glLinkProgram(this->program);
			GLint linkStatus;
			glGetProgramiv(this->program, GL_LINK_STATUS, &linkStatus);
			if (linkStatus == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetProgramiv(this->program, GL_INFO_LOG_LENGTH, &maxLength);
				std::vector<GLchar> errLog(maxLength);
				glGetProgramInfoLog(this->program, maxLength, &maxLength, &errLog[0]);
				std::cout << "Error::shader link failed," << &errLog[0] << std::endl;
			}
		}
		// 链接完成后detach 并释放shader object
		for (size_t i = 0; i < shaderObjectIdVec.size(); ++i)
		{
			if (this->program != 0)
			{
				glDetachShader(this->program, shaderObjectIdVec[i]);
			}
			glDeleteShader(shaderObjectIdVec[i]);
		}
	}
	/*
	* 读取着色器程序源码
	*/
	bool loadShaderSource(const char* filePath, vector<ShaderSource>& sourceStrs)
	{
		std::ifstream in_stream(filePath);
		if (!in_stream)
		{
			cout << "Error::Shader could not load file:" << filePath << endl;
			return false;
		}
		string source;
		source.assign(std::istreambuf_iterator<char>(in_stream),
			std::istreambuf_iterator<char>()); // 文件流迭代器构造字符串

		stringstream ss(source);
		while (getline(ss, source, '@')) {
			sourceStrs.push_back(ShaderSource(GL_VERTEX_SHADER, source));
		}
		sourceStrs.back().shaderType = GL_FRAGMENT_SHADER;
		if (sourceStrs.size() == 3) {
			sourceStrs[1].shaderType = GL_GEOMETRY_SHADER;
		}
		return true;
	}
};
#endif