#include <iostream>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include <SOIL2.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"


// Function prototypes
void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mode );
void MouseCallback( GLFWwindow *window, double xPos, double yPos );
void DoMovement( );

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera( glm::vec3( 0.0f, 0.0f, 3.0f ) );
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;

// Deltatime
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// The MAIN function
int main( )
{
    // Init GLFW
    if ( !glfwInit( ) )
    {
        return EXIT_FAILURE;
    }
    // Set all the required options for GLFW
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );
    
    // Create a GLFWwindow object
    GLFWwindow* window = glfwCreateWindow( WIDTH, HEIGHT, "Lab8: Carga de Modelos 3D", nullptr, nullptr );
    
    if ( nullptr == window )
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate( );
        return EXIT_FAILURE;
    }
    
    glfwMakeContextCurrent( window );
    glfwGetFramebufferSize( window, &SCREEN_WIDTH, &SCREEN_HEIGHT );
    
    // Set the required callback functions
    glfwSetKeyCallback( window, KeyCallback );
    glfwSetCursorPosCallback( window, MouseCallback );
    
    // GLFW Options
    glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
    
    // Set this to true so GLEW knows to use a modern approach
    glewExperimental = GL_TRUE;
    // Initialize GLEW
    if ( GLEW_OK != glewInit( ) )
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }
    
    // Define the viewport dimensions
    glViewport( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT );
    
    // OpenGL options
    glEnable( GL_DEPTH_TEST );
    
    // Build and compile our shader program
    Shader ourShader( "res/shaders/model_loading.vs", "res/shaders/model_loading.frag" );
    
    // Validate shader linked correctly
    GLint shaderLinked = 0;
    glGetProgramiv( ourShader.ID, GL_LINK_STATUS, &shaderLinked );
    if ( !shaderLinked )
    {
        std::cout << "[FATAL] Shader program did not link. Check that res/shaders/ exists relative to the working directory." << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }
    
    // Load models
    // Note: You should place a model in assets/models/
    // For now, this is a placeholder. If no model exists, it will show an error in console but continue.
    Model ourModel( "../../assets/models/laptop.glb" );
    
    if ( ourModel.meshes.empty() )
    {
        std::cout << "[FATAL] Model failed to load or has no meshes. Check that ../../assets/models/911.glb exists." << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }
    
    // Game loop
    while ( !glfwWindowShouldClose( window ) )
    {
        // Calculate deltatime
        GLfloat currentFrame = glfwGetTime( );
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Check events
        glfwPollEvents( );
        DoMovement( );
        
        // Clear the colorbuffer
        glClearColor( 0.05f, 0.05f, 0.05f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        ourShader.use( );
        
        // View/Projection transformations
        glm::mat4 projection = glm::perspective( camera.Zoom, ( float )SCREEN_WIDTH / ( float )SCREEN_HEIGHT, 0.1f, 100.0f );
        glm::mat4 view = camera.GetViewMatrix( );
        ourShader.setMat4( "projection", projection );
        ourShader.setMat4( "view", view );
        glUniform3f(glGetUniformLocation(ourShader.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
        
        // Draw the loaded model
        glm::mat4 model = glm::mat4( 1.0f );
        model = glm::translate( model, glm::vec3( 0.0f, -0.5f, 0.0f ) ); // translated down a bit
        model = glm::scale( model, glm::vec3( 1.0f, 1.0f, 1.0f ) );	
        ourShader.setMat4( "model", model );
        ourModel.Draw( ourShader );
        
        // Swap the screen buffers
        glfwSwapBuffers( window );
    }
    
    // Terminate GLFW
    glfwTerminate( );
    
    return 0;
}

// Moves/alters the camera positions based on user input
void DoMovement( )
{
    if ( keys[GLFW_KEY_W] )
    {
        camera.ProcessKeyboard( FORWARD, deltaTime );
    }
    if ( keys[GLFW_KEY_S] )
    {
        camera.ProcessKeyboard( BACKWARD, deltaTime );
    }
    if ( keys[GLFW_KEY_A] )
    {
        camera.ProcessKeyboard( LEFT, deltaTime );
    }
    if ( keys[GLFW_KEY_D] )
    {
        camera.ProcessKeyboard( RIGHT, deltaTime );
    }
}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mode )
{
    if ( GLFW_KEY_ESCAPE == key && GLFW_PRESS == action )
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    
    if ( key >= 0 && key < 1024 )
    {
        if ( action == GLFW_PRESS )
        {
            keys[key] = true;
        }
        else if ( action == GLFW_RELEASE )
        {
            keys[key] = false;
        }
    }
}

void MouseCallback( GLFWwindow *window, double xPos, double yPos )
{
    if ( firstMouse )
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }
    
    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;
    
    lastX = xPos;
    lastY = yPos;
    
    camera.ProcessMouseMovement( xOffset, yOffset );
}
