#include "engine/Shader.h"

#include <iostream>
#include <sstream>

Shader::Shader(const std::string& filepath) :
	filepath(filepath)
{
	isUsingFile = true;
}

Shader::Shader(const std::string & vertexShader, const std::string & fragmentShader) :
	vs(vertexShader),
	fs(fragmentShader)
{
	isUsingFile = false;
}

Shader::Shader(GLuint existingShader) {
	id = existingShader;
	isInitialized = true;
}

Shader::~Shader()
{
	if (isInitialized) {
		freeMemory();
	}
}

void Shader::setUniform1f(const std::string & uniformName, float value) {
	bind();
	glUniform1f(getUniformLocation(uniformName), value);
	unbind();
}

void Shader::setUniform2f(const std::string & uniformName, float * values) {
	bind();
	glUniform2f(getUniformLocation(uniformName), values[0], values[1]);
	unbind();
}

void Shader::setUniform3f(const std::string & uniformName, float * values) {
	bind();
	glUniform3f(getUniformLocation(uniformName), values[0], values[1], values[2]);
	unbind();
}

void Shader::setUniform4f(const std::string & uniformName, float * values) {
	bind();
	glUniform4f(getUniformLocation(uniformName), values[0], values[1], values[2], values[3]);
	unbind();
}

void Shader::setUniform2f(const std::string & uniformName, float a, float b) {
	bind();
	glUniform2f(getUniformLocation(uniformName), a, b);
	unbind();
}

void Shader::setUniform3f(const std::string & uniformName, float a, float b, float c) {
	bind();
	glUniform3f(getUniformLocation(uniformName), a, b, c);
	unbind();
}

void Shader::setUniform4f(const std::string & uniformName, float a, float b, float c, float d) {
	bind();
	glUniform4f(getUniformLocation(uniformName), a, b, c, d);
	unbind();
}

void Shader::setUniform1i(const std::string & uniformName, int value) {
	bind();
	glUniform1i(getUniformLocation(uniformName), value);
	unbind();
}

void Shader::setUniformIntArray(const std::string & uniformName, int * values, int count)
{
	bind();
	glUniform1iv(getUniformLocation(uniformName), count, values);
	unbind();
}

void Shader::bind() {
	if (!isInitialized) initialize();
	glUseProgram(id);
}

void Shader::unbind() {
	glUseProgram(0);
}

void Shader::freeMemory() {
	if (isInitialized) {
		glDeleteProgram(id);
	}
	unbind();
}

GLuint Shader::createShader(const std::string & vertexShader, const std::string & fragmentShader)
{
	GLuint program = glCreateProgram();
	GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShader);
	GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

GLuint Shader::compileShader(GLuint type, const std::string& source) {

	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = new char[length];
		glGetShaderInfoLog(id, length, &length, message);
		if (type == GL_VERTEX_SHADER) {
			std::cout << "Failed to compile vertex shader" << std::endl;
		}
		else if (type == GL_FRAGMENT_SHADER) {
			std::cout << "Failed to compile fragment shader" << std::endl;
		}
	
		std::cout << message << std::endl;
		glDeleteShader(id);
		delete[] message;
		return 0;
	}

	return id;
}


GLuint Shader::getUniformLocation(const std::string& name) {
	if (uniformLocationCache.find(name) != uniformLocationCache.end()) {
		return uniformLocationCache[name];
	}

	int location = glGetUniformLocation(id, name.c_str());
	if (location == -1) {
		std::cout << "Warning: uniform " << name << " doesn't exist" << std::endl;
	}

	uniformLocationCache[name] = location;
	return location;
}

void Shader::initialize()
{
	if (isUsingFile) {
		std::ifstream fileStream(filepath);

		if (!fileStream.good()) {
			std::cout << "shader file not found" << std::endl;
		}

		enum class ShaderType {
			NONE = -1, VERTEX = 0, FRAGMENT = 1
		};

		std::string line;
		std::stringstream ss[3];
		ShaderType type = ShaderType::NONE;
		while (getline(fileStream, line)) {
			if (line.find("#shader") != std::string::npos) {

				if (line.find("vertex") != std::string::npos) {
					type = ShaderType::VERTEX;
				}
				else if (line.find("fragment") != std::string::npos) {
					type = ShaderType::FRAGMENT;
				}
			}
			else {
				ss[(int)type] << line << '\n';
				//std::cout << line << std::endl;
			}
		}

		fileStream.close();

		id = createShader(ss[0].str(), ss[1].str());
	}
	else {
		id = createShader(vs, fs);
	}

	isInitialized = true;
}
