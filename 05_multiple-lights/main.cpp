#include "glad/glad.h"
#include "GLFW/glfw3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "utils/shader.hpp"
#include "cube_vertices.hpp"
#include "utils/camera.hpp"

#include <array>
#include <iostream>
#include <sstream>
#include <cmath>

// =============================================
#ifndef LESSON_NAME
#define LESSON_NAME "default"
#endif

#define LESSON_DIR std::string(LESSON_NAME)

// =============================================

// Utils
unsigned int loadTexture(const char * path);

// Callbacks
void framebuffer_size_callback(GLFWwindow * window, int w, int h);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow * window, double xoffset, double yoffset);
void processInput(GLFWwindow * window);

// Global vars
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0, 0.0, 3.0));
static float lastX = SCR_WIDTH / 2.0;
static float lastY = SCR_HEIGHT / 2.0;
static bool firstMouse = true;

// timing
static float deltaTime = 0.0f; // Time between current frame and last frame
static float lastFrame = 0.0f; // Time of last frame

// lighting
static glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
static bool flashlightOn = false;
static bool btnFPressed = false;

static constexpr float glowDuration = 3.0f;
static float glowStart = -2.0f * glowDuration;

static std::array<bool, 4> sLightState = { false, false, false, true };
static std::array<bool, 4> sLightBtnState = { false, false, false, false };
static_assert(sLightState.size() == sLightBtnState.size(), "Should be the same size");

// ===========================================================
// Start main
int main()
{
    // 0. Initialization. Create window, init GLAD.
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // window create
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL, Textures", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // configure global opengl state
    glEnable(GL_DEPTH_TEST);
    // ==================================
    // 1. Prepare data: Create objects 
    std::string shaderPath = "shaders/" + LESSON_DIR + "/object";
    Shader objectShader((shaderPath + ".vs").c_str(), (shaderPath + ".fs").c_str());

    shaderPath = "shaders/" + LESSON_DIR + "/lighting";
    Shader lightingShader((shaderPath + ".vs").c_str(), (shaderPath + ".fs").c_str());
    // setup light uniforms

    // ==================================
    // 2. Set up objects
    // prepare data and buffers

    // *** MAIN CUBE DATA ***
    unsigned int VBO; // vertex buffer object
    glGenBuffers(1, &VBO);

    unsigned int VAO; // vertex array object
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    // load and generate texture
    stbi_set_flip_vertically_on_load(true);
    std::string texturePath = "textures/"+ LESSON_DIR + "/container2.png";
    unsigned int diffuseMap = loadTexture(texturePath.c_str());

    texturePath = "textures/"+ LESSON_DIR + "/container2_specular.png";
    unsigned int specularMap = loadTexture(texturePath.c_str());

    texturePath = "textures/"+ LESSON_DIR + "/matrix.jpg";
    unsigned int emissionMap = loadTexture(texturePath.c_str());

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fullCubeVertices), fullCubeVertices, GL_STATIC_DRAW);
    
    unsigned int byte_stride = 8 * sizeof(float);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, byte_stride, (void*) (0 * sizeof(float)));
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, byte_stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, byte_stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // *** separate VAO for light cube ***
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    // we only need to bind to the VBO, the container's VBO's data already contains the data.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // set the vertex attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, byte_stride, (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(0);

    std::array cubePos = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.5f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f),
    };

    std::array pointLightsPos = {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f,  -3.0f),
    };

    std::cout << "End of preparation. Start main loop" << std::endl; 

    // RENDER LOOP
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = float(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame; 

        // input
        processInput(window);

        // render
        // clear the color buffer
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // set up camera related props
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.Zoom), float(SCR_WIDTH) / float(SCR_HEIGHT), 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model;

        // lighting object
        // recalculate light object pos
        lightingShader.use();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        lightingShader.setVec3("color", lightColor);
        
        for (size_t indx = 0; indx < pointLightsPos.size(); ++indx)
        {
            if (sLightState.at(indx) == false)
                continue;
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightsPos.at(indx));
            model = glm::scale(model, glm::vec3(0.1f));
    
            lightingShader.setMat4("model", model);        

            glBindVertexArray(lightVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // ==================================
        // real object

        objectShader.use();

        // set uniforms
        // material
        objectShader.setInt("material.diffuse", 0);
        objectShader.setInt("material.specular", 1);
        objectShader.setInt("material.emission", 2);
        objectShader.setFloat("material.shininess", 64.0f);
        
        // lights
        glm::vec3 ambientColor = lightColor * glm::vec3(0.2f);
        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);

        float amplitude = std::max(std::abs(cos(glm::radians(currentFrame * 10))), 0.1f);

        objectShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        objectShader.setVec3("dirLight.ambient", ambientColor * amplitude);
        objectShader.setVec3("dirLight.diffuse", diffuseColor * amplitude);
        objectShader.setVec3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f) * amplitude);

        // emission feature
        float shift = currentFrame / glowDuration;
        objectShader.setFloat("textShift", shift);
        
        float glow = 0.0f;
        if (currentFrame < glowStart + glowDuration * 1.2f)
        {
            glow = std::max(1.0f - (currentFrame - glowStart) / glowDuration, 0.0f);
        }
        objectShader.setFloat("textGlow", glow);

        for (uint8_t i = 0; i < pointLightsPos.size(); ++i)
        {
            std::ostringstream oss;
            oss << "pointLights[" << char('0' + i) <<  "].";
            std::string prefix = oss.str();

            objectShader.setVec3(prefix + "position", pointLightsPos[i]);

            objectShader.setFloat(prefix + "constant", 1.0f);
            objectShader.setFloat(prefix + "linear", 0.09f);
            objectShader.setFloat(prefix + "quadratic", 0.032f);

            if (sLightState.at(i))
            {
                objectShader.setVec3(prefix + "ambient", ambientColor);
                objectShader.setVec3(prefix + "diffuse", diffuseColor);
                objectShader.setVec3(prefix + "specular", glm::vec3(1.0f, 1.0f, 1.0f));                
            }
            else
            {
                objectShader.setVec3(prefix + "ambient", glm::vec3(0.0f));
                objectShader.setVec3(prefix + "diffuse", glm::vec3(0.0f));
                objectShader.setVec3(prefix + "specular",glm::vec3(0.0f));     
            }
        }

        objectShader.setVec3("spotLight.position", camera.Position);
        objectShader.setVec3("spotLight.direction", camera.Front);
        objectShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        objectShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(18.0f)));  

        objectShader.setFloat("spotLight.constant", 1.0f);
        objectShader.setFloat("spotLight.linear", 0.09f);
        objectShader.setFloat("spotLight.quadratic", 0.032f);

        if (flashlightOn)
        {
            objectShader.setVec3("spotLight.ambient", glm::vec3(0.1f));
            objectShader.setVec3("spotLight.diffuse", glm::vec3(1.0f));
            objectShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        }
        else 
        {
            objectShader.setVec3("spotLight.ambient", glm::vec3(0.0f));
            objectShader.setVec3("spotLight.diffuse", glm::vec3(0.0f));
            objectShader.setVec3("spotLight.specular", glm::vec3(0.0f));
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, emissionMap);

        objectShader.setVec3("viewPos", camera.Position);

        objectShader.setMat4("projection", projection);
        objectShader.setMat4("view", view);
        model = glm::mat4(1.0f);

        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < cubePos.size(); i++)
        {
            // calculate the model matrix for each object and it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f); // !!! make sure to initialize matrix to identity matrix first
            model = glm::translate(model, cubePos.at(i));
            float angle = currentFrame * 20.0f * (i % 3 + 1);
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

            objectShader.setMat4("model", model);

            // now render the triangles
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        // finish
        glBindVertexArray(0);

        // check and call events and swap the buffer
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional : de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);

    // terminate, learing all previously allocated GLFW resources
    glfwTerminate();
    return 0;
}

void processLight(size_t indx, bool isOn)
{
    auto & btn = sLightBtnState.at(indx);
    auto & state = sLightState.at(indx);

    if (isOn)
    {
        if (btn == false)
        {
            btn = true;
            state = !state;
            std::cout << "Point light " << indx << " is " << (state ? "on" : "off") << "!" << std::endl;
        }
    }
    else
    {
        btn = false;
    }
}

// Process all input
// Keyboard
void processInput(GLFWwindow * window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        std::cout << "ESC button pressed" << std::endl;
        glfwSetWindowShouldClose(window, true);
    }
    // MY: Change polygone mode
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_RELEASE) { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }
    // ==========================
    // Lights turn on/off
    // flashlight
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) 
    { 
        if (btnFPressed == false) 
        { 
            btnFPressed = true; 
            flashlightOn = !flashlightOn; 
            std::cout << "Flash light turns " << (flashlightOn ? "on" : "off") << "!" << std::endl;
        } 
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) { btnFPressed = false; }
    // point lights
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) { processLight(0, true); }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) { processLight(0, false); }

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) { processLight(1, true); }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) { processLight(1, false); }
    
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) { processLight(2, true); }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE) { processLight(2, false); }
    
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) { processLight(3, true); }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_RELEASE) { processLight(3, false); }
    // emission feature
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) { glowStart = lastFrame; }

    // ===========================
    // move camera
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { camera.ProcessKeyboard(Camera::FORWARD, deltaTime); }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { camera.ProcessKeyboard(Camera::BACKWARD, deltaTime); }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { camera.ProcessKeyboard(Camera::LEFT, deltaTime); }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { camera.ProcessKeyboard(Camera::RIGHT, deltaTime); }
    // ===========================
    // Change camera mode
    if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET))
    {
        if (camera.mode != Camera::FLY)
        {
            camera.mode = Camera::FLY;
            std::cout << "FLY camera mode activated" << std::endl;            
        }
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET))
    {
        if (camera.mode != Camera::FPS)
        {
            camera.mode = Camera::FPS;
            std::cout << "FPS camera mode activated" << std::endl;
        }
    }
    // ===========================
}
// Mouse
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    auto xpos = float(xposIn);
    auto ypos = float(yposIn);

    if (firstMouse)
    {
        firstMouse = false;
        lastX = xpos;
        lastY = ypos;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}
// Scroll
void scroll_callback(GLFWwindow * window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(float(yoffset));
}

// react on window size changed
void framebuffer_size_callback(GLFWwindow * window, int w, int h)
{
    glViewport(0, 0, w, h);
}

// Util for loading 2d texture form file
unsigned int loadTexture(const char * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, numComponents;
    unsigned char * data = stbi_load(path, &width, &height, &numComponents, 0);
    if (data)
    {
        GLenum format;
        switch (numComponents)
        {
            case 1:
            {
                format = GL_RED;
                break;                
            }
            case 3:
            {
                format = GL_RGB;
                break;
            }
            case 4:
            {
                format = GL_RGBA;
                break;
            }
            default:
            {
                std::cout << "ERROR: Failed to load texture: " << path 
                          << ". Unhandled number of components: " << numComponents <<std::endl;
                stbi_image_free(data);
                return textureID;
            }
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {  
        std::cout << "ERROR: Failed to load texture: " << path << std::endl;
    }
    stbi_image_free(data);
    return textureID;
}