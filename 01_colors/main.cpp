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
#include <cmath>

// =============================================

static const std::string LESSON_NAME = "01_colors";

// =============================================

// Utils
void loadAndGenerateTextureMipmap(const char * file_path, int src_type)
{
    int width, height, numChannels;
    unsigned char * data = stbi_load(file_path, &width, &height, &numChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, src_type, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "ERROR: Failed to load texture: " << file_path << std::endl;
    }
    stbi_image_free(data);
}

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
static glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

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
    std::string shaderPath = "shaders/" + LESSON_NAME + "-object";
    Shader objectShader((shaderPath + ".vs").c_str(), (shaderPath + ".fs").c_str());

    shaderPath = "shaders/" + LESSON_NAME + "-lighting";
    Shader lightingShader((shaderPath + ".vs").c_str(), (shaderPath + ".fs").c_str());
    // world position of our cubes
    std::array cubePositions = {
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
    
    // setup light uniforms
    objectShader.use();

    // ==================================
    // 2. Set up objects
    // prepare data and buffers

    // *** MAIN CUBE DATA ***
    unsigned int VBO; // vertex buffer object
    glGenBuffers(1, &VBO);

    unsigned int VAO; // vertex array object
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fullCubeVertices), fullCubeVertices, GL_STATIC_DRAW);
    
    unsigned int byte_stride = 3 * sizeof(float);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, byte_stride, (void*)0);
    glEnableVertexAttribArray(0);

    // *** separate VAO for light cube ***
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    // we only need to bind to the VBO, the container's VBO's data already contains the data.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // set the vertex attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, byte_stride, (void*)0);
    glEnableVertexAttribArray(0);

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
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));

        lightingShader.use();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        lightingShader.setMat4("model", model);
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // real object
        model = glm::mat4(1.0f);

        objectShader.use();
        objectShader.setMat4("projection", projection);
        objectShader.setMat4("view", view);
        objectShader.setMat4("model", model);

        objectShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        objectShader.setVec3("lightColor", lightColor);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

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
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_RELEASE)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    // ==========================
    // change mix value
    if (glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS)
    {

    }
    if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS)
    {

    }
    // ===========================
    // move camera
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(Camera::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(Camera::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(Camera::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(Camera::RIGHT, deltaTime);
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
