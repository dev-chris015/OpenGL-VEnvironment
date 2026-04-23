cmake_minimum_required(VERSION 3.10)
project(PracticasOpenGL VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)
include_directories(Header-Files)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)

# Configurar FreeGLUT (Local)
set(FREEGLUT_ROOT "${CMAKE_SOURCE_DIR}/freeglut-MSVC/freeglut")
include_directories("${FREEGLUT_ROOT}/include")
link_directories("${FREEGLUT_ROOT}/lib/x64")

add_executable(app 
    src/main.cpp 
    src/glad.c
    src/EBO.cpp
    src/VAO.cpp
    src/VBO.cpp
    src/shaderClass.cpp
    src/stb.cpp
    src/Texture.cpp
)

target_link_libraries(app 
    PRIVATE 
    glfw 
    glm::glm
    GLEW::GLEW
    freeglut
)

if(WIN32)
    target_link_libraries(app PRIVATE opengl32 user32 gdi32 shell32)
endif()
