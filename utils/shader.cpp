#include "shader.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>
#include <sstream>

std::string readCodeFromFile(const char * path)
{
    std::string code;
    std::ifstream file;
    try 
    {
        file.open(path);
        std::stringstream codeStream;
        codeStream << file.rdbuf();
        file.close();

        code = codeStream.str();
    }
    catch(std::ifstream::failure & e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        return {};
    }
    return code;
}

void checkShaderCompilation(unsigned int shaderID)
{
    // Check if shader compiles right
    int success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[1024]; 
        glGetShaderInfoLog(shaderID, sizeof(infoLog), NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}

void checkProgramLink(unsigned int programID)
{
    int success = 1;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success)
    {
        char logInfo[1024];
        glGetProgramInfoLog(programID, sizeof(logInfo), NULL, logInfo);
        std::cerr << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << logInfo << std::endl;

    }
}

Shader::Shader(const char * vertexPath, const char * fragmentPath)
{
    // 1. Read files
    auto vertexCodeStr = readCodeFromFile(vertexPath);
    auto fragmentCodeStr = readCodeFromFile(fragmentPath);
    const char * vertexCode = vertexCodeStr.c_str();
    const char * fragmentCode = fragmentCodeStr.c_str();

    // 2. Compile shaders
    unsigned int vertexID, fragmentID;

    vertexID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexID, 1, &vertexCode, NULL);
    glCompileShader(vertexID);
    checkShaderCompilation(vertexID);

    fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentID, 1, &fragmentCode, NULL);
    glCompileShader(fragmentID);
    checkShaderCompilation(fragmentID);

    ID = glCreateProgram();
    glAttachShader(ID, vertexID);
    glAttachShader(ID, fragmentID);
    glLinkProgram(ID);
    checkProgramLink(ID);

    // delete the shaders as they're linked into out program now and no longer necessary
    glDeleteShader(vertexID);
    glDeleteShader(fragmentID);
}

void Shader::use() const
{
    glUseProgram(ID);
}
// Set uniforms
// Trivial
void Shader::setBool(std::string const & name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setInt(std::string const & name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(std::string const & name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

// Vectors
void Shader::setVec2(std::string const & name, glm::vec2 const & value) const
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setVec3(std::string const & name, glm::vec3 const & value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setVec4(std::string const & name, glm::vec4 const & value) const
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setVec2(std::string const & name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);

}
void Shader::setVec3(std::string const & name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
void Shader::setVec4(std::string const & name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

// Matrix
void Shader::setMat2(std::string const & name, glm::mat2 const & value) const
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setMat3(std::string const & name, glm::mat3 const & value) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setMat4(std::string const & name, glm::mat4 const & value) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

// More flexible variant
void Shader::setMatrix4Float(std::string const & name, GLsizei count, GLboolean transpose, GLfloat const * value) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), count, transpose, value);
}

unsigned int Shader::getID() const 
{
    return ID;
}