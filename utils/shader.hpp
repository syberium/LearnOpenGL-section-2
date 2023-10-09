#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>

class Shader
{
public:
    // main constructor
    Shader(const char * vertexShaderPath, const char * fragmentShaderPath);
    // use/activate shader
    void use() const;
    // utility uniform functions
    void setBool(std::string const & name, bool value) const;
    void setInt(std::string const & name, int value) const;
    void setFloat(std::string const & name, float value) const;

    void setVec2(std::string const & name, glm::vec2 const & value) const;
    void setVec3(std::string const & name, glm::vec3 const & value) const;
    void setVec4(std::string const & name, glm::vec4 const & value) const;

    void setVec2(std::string const & name, float x, float y) const;
    void setVec3(std::string const & name, float x, float y, float z) const;
    void setVec4(std::string const & name, float x, float y, float z, float w) const;

    void setMat2(std::string const & name, glm::mat2 const & value) const;
    void setMat3(std::string const & name, glm::mat3 const & value) const;
    void setMat4(std::string const & name, glm::mat4 const & value) const;
    
    void setMatrix4Float(std::string const & name, GLsizei count, GLboolean transpose, GLfloat const * value) const;
    // get program id
    unsigned int getID() const;

private:
    //! @brief Program id
    unsigned int ID{0};
};