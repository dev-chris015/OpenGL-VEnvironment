#include <cmath>
#include <iostream>
#include <vector>
#include <string>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include <SOIL2.h>
#include <stb/stb_image.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include "Camera.h"
#include "Model.h"
#include "Shader.h"

// Function prototypes
void KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();
unsigned int loadCubemap(std::vector<std::string> faces);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600; int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 20.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;

// Deltatime
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// The MAIN function
int main() {
  // Init GLFW
  if (!glfwInit()) {
    return EXIT_FAILURE;
  }
  // Set all the required options for GLFW
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  // Create a GLFWwindow object
  GLFWwindow *window = glfwCreateWindow(
      WIDTH, HEIGHT, "Importando un modelo 3D gltf y Skybox", nullptr, nullptr);

  if (nullptr == window) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return EXIT_FAILURE;
  }

  glfwMakeContextCurrent(window);
  glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

  // Set the required callback functions
  glfwSetKeyCallback(window, KeyCallback);
  glfwSetCursorPosCallback(window, MouseCallback);

  // GLFW Options
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // Set this to true so GLEW knows to use a modern approach
  glewExperimental = GL_TRUE;
  // Initialize GLEW
  if (GLEW_OK != glewInit()) {
    std::cout << "Failed to initialize GLEW" << std::endl;
    return EXIT_FAILURE;
  }

  // Define the viewport dimensions
  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

  // OpenGL options
  glEnable(GL_DEPTH_TEST);

  // Build and compile our shader programs
  Shader ourShader("res/shaders/model_loading.vs",
                   "res/shaders/model_loading.frag");
  Shader skyboxShader("res/shaders/skybox.vs", "res/shaders/skybox.frag");

  // Load models
  Model ourModel("../../assets/models/tv.glb");

  if (ourModel.meshes.empty()) {
    std::cout << "[FATAL] Model failed to load or has no meshes. Check that "
                 "../../assets/models/tv.glb exists."
              << std::endl;
    glfwTerminate();
    return EXIT_FAILURE;
  }

  // Skybox vertices (without front face +Z to see inside)
  float skyboxVertices[] = {
      // Back face (-Z)
      -1.0f,  1.0f, -1.0f,
      -1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f,  1.0f, -1.0f,
      -1.0f,  1.0f, -1.0f,

      // Left face (-X)
      -1.0f, -1.0f,  1.0f,
      -1.0f, -1.0f, -1.0f,
      -1.0f,  1.0f, -1.0f,
      -1.0f,  1.0f, -1.0f,
      -1.0f,  1.0f,  1.0f,
      -1.0f, -1.0f,  1.0f,

      // Right face (+X)
       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,
       1.0f,  1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,

      // Top face (+Y)
      -1.0f,  1.0f, -1.0f,
       1.0f,  1.0f, -1.0f,
       1.0f,  1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,
      -1.0f,  1.0f,  1.0f,
      -1.0f,  1.0f, -1.0f,

      // Bottom face (-Y)
      -1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f,  1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f,  1.0f,
       1.0f, -1.0f,  1.0f
  };

  // Skybox VAO/VBO
  unsigned int skyboxVAO, skyboxVBO;
  glGenVertexArrays(1, &skyboxVAO);
  glGenBuffers(1, &skyboxVBO);
  glBindVertexArray(skyboxVAO);
  glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

  // Load skybox textures
  // We use the available textures in assets/textures for the demonstration.
  std::vector<std::string> faces {
      "../../assets/textures/textura#4.jpeg", // Right
      "../../assets/textures/textura#5.jpeg", // Left
      "../../assets/textures/textura#6.jpeg", // Top
      "../../assets/textures/textura#7.jpeg", // Bottom
      "../../assets/textures/textura#8.jpeg", // Front (missing geometry, but texture is loaded)
      "../../assets/textures/textura#9.jpeg"  // Back
  };
  unsigned int cubemapTexture = loadCubemap(faces);

  skyboxShader.use();
  skyboxShader.setInt("skybox", 0);

  // Game loop
  while (!glfwWindowShouldClose(window)) {
    // Calculate deltatime
    GLfloat currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Check events
    glfwPollEvents();
    DoMovement();

    // Clear the colorbuffer
    glClearColor(0.22f, 0.22f, 0.22f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // --- Render Environment (Skybox mapped as a physical room) ---
    // Disable face culling so we can see the inside of the cube from the outside
    glDisable(GL_CULL_FACE);

    skyboxShader.use();
    glm::mat4 projection = glm::perspective(
        camera.Zoom, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    skyboxShader.setMat4("view", view);
    skyboxShader.setMat4("projection", projection);
    
    // Scale the cube to be large enough to contain the model
    glm::mat4 envModel = glm::mat4(1.0f);
    envModel = glm::scale(envModel, glm::vec3(15.0f, 15.0f, 15.0f));
    skyboxShader.setMat4("model", envModel);
    
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    // Draw 30 vertices (5 faces * 6 vertices)
    glDrawArrays(GL_TRIANGLES, 0, 30);
    glBindVertexArray(0);

    // --- Render Model ---
    ourShader.use();
    ourShader.setMat4("projection", projection);
    ourShader.setMat4("view", view);
    glUniform3f(glGetUniformLocation(ourShader.ID, "viewPos"),
                camera.Position.x, camera.Position.y, camera.Position.z);
    
    // Set a default material color in case the model has no diffuse textures
    glUniform4f(glGetUniformLocation(ourShader.ID, "materialColor"), 0.8f, 0.8f, 0.8f, 1.0f);

    // Draw the loaded model
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Centrado en el skybox
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotado para estar derecho
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotado para mirar hacia la cámara
    model = glm::scale(model, glm::vec3(12.0f, 12.0f, 12.0f));
    ourShader.setMat4("model", model);
    ourModel.Draw(ourShader);

    // Swap the screen buffers
    glfwSwapBuffers(window);
  }

  // Terminate GLFW
  glDeleteVertexArrays(1, &skyboxVAO);
  glDeleteBuffers(1, &skyboxVBO);
  glfwTerminate();

  return 0;
}

// Loads a cubemap texture from 6 individual texture faces
unsigned int loadCubemap(std::vector<std::string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = GL_RGB;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;

            // OpenGL exige que todas las caras de un cubemap sean cuadradas y del mismo tamaño.
            // Redimensionamos a 1024x1024 usando interpolación de vecino más cercano.
            int targetSize = 1024;
            unsigned char* resizedData = new unsigned char[targetSize * targetSize * nrChannels];
            for (int y = 0; y < targetSize; ++y) {
                for (int x = 0; x < targetSize; ++x) {
                    int srcX = x * width / targetSize;
                    int srcY = y * height / targetSize;
                    for (int c = 0; c < nrChannels; ++c) {
                        resizedData[(y * targetSize + x) * nrChannels + c] = data[(srcY * width + srcX) * nrChannels + c];
                    }
                }
            }

            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, format, targetSize, targetSize, 0, format, GL_UNSIGNED_BYTE, resizedData);
            
            delete[] resizedData;
            stbi_image_free(data);
        } else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

// Moves/alters the camera positions based on user input
void DoMovement() {
  if (keys[GLFW_KEY_W]) {
    camera.ProcessKeyboard(FORWARD, deltaTime);
  }
  if (keys[GLFW_KEY_S]) {
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  }
  if (keys[GLFW_KEY_A]) {
    camera.ProcessKeyboard(LEFT, deltaTime);
  }
  if (keys[GLFW_KEY_D]) {
    camera.ProcessKeyboard(RIGHT, deltaTime);
  }
}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mode) {
  if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }

  if (key >= 0 && key < 1024) {
    if (action == GLFW_PRESS) {
      keys[key] = true;
    } else if (action == GLFW_RELEASE) {
      keys[key] = false;
    }
  }
}

void MouseCallback(GLFWwindow *window, double xPos, double yPos) {
  if (firstMouse) {
    lastX = xPos;
    lastY = yPos;
    firstMouse = false;
  }

  GLfloat xOffset = xPos - lastX;
  GLfloat yOffset = lastY - yPos;

  lastX = xPos;
  lastY = yPos;

  camera.ProcessMouseMovement(xOffset, yOffset);
}
