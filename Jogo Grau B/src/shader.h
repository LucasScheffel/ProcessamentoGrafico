#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>

class Shader {
public:
    GLuint programID;

    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    void use();
    void setUniform(const std::string& name, const glm::mat4& value);

private:
    std::string readFile(const std::string& filePath);
    GLuint compileShader(const std::string& source, GLenum shaderType);
};

#endif // SHADER_H
