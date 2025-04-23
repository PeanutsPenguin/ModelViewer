#include "Resources/Shader.h"
#include <fstream>
#include <iostream>

std::string Resources::Shader::readShader(const char* fileName)
{
	std::ifstream f(fileName);
	std::string buff;
	if (f) 
	{
		std::ostringstream ss;
		ss << f.rdbuf(); 
		buff = ss.str();
	}
	return buff;
}

unsigned int Resources::Shader::setShader(const char* fileName, GLenum mode)
{
	std::string shader = readShader(fileName);
	const char* source = shader.c_str();
	
	unsigned int shader_id = glCreateShader(mode);
	glShaderSource(shader_id, 1, &source, NULL);
	glCompileShader(shader_id);
	
	int success; 
	char infoLog[512];
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
	
	if (!success)
	{
	    glGetShaderInfoLog(shader_id, 512, NULL, infoLog);
	    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	    return -1;
	}
	
	return shader_id;
}