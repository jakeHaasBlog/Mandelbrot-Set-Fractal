#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <fstream>

#include <unordered_map>

class Shader {
public:
	/* file must contain vertex and fragment shader
	- first line of vertex shader must be: #shader vertex
	- first line of fragment shader must be: #shader fragment 
	* note that the '#vertex shader' tag must be the first line in the file*/
	Shader(const std::string& filepath);

	// manually input vertex and fragent shaders as std::strings
	Shader(const std::string& vertexShader, const std::string& fragmentShader);

	// stores the id of an already initialized shader program
	Shader(GLuint existingShader);

	~Shader();

	// prevents copying
	Shader(const Shader&) = delete;

	void setUniform1f(const std::string& uniformName, float  value);
	void setUniform2f(const std::string& uniformName, float* values);
	void setUniform3f(const std::string& uniformName, float* values);
	void setUniform4f(const std::string& uniformName, float* values);

	void setUniform2f(const std::string& uniformName, float a, float b);
	void setUniform3f(const std::string& uniformName, float a, float b, float c);
	void setUniform4f(const std::string& uniformName, float a, float b, float c, float d);

	// usually used for specifying the desired texture slot
	void setUniform1i(const std::string& uniformName, int value);
	void setUniformIntArray(const std::string& uniformName, int* values, int count);

	// important: you must set any required uniforms BEFORE you bind the shader for rendering
	void bind();
	void unbind();

private:
	GLuint createShader(const std::string& vertexShader, const std::string& fragmentShader);
	GLuint compileShader(GLuint type, const std::string& source);

	GLuint getUniformLocation(const std::string& name);

	void freeMemory();

private:
	GLuint id = 0;
	std::unordered_map<std::string, int> uniformLocationCache;

	void initialize();

	bool isInitialized = false;
	bool isUsingFile;
	const std::string filepath;
	const std::string fs;
	const std::string vs;

};