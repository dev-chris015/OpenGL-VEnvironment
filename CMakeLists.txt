cmake_minimum_required(VERSION 3.10)
project(PracticasOpenGL VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Directorios de inclusion
include_directories(include)
include_directories(Header-Files)
include_directories(dependencies/soil2)

add_definitions(-DGLM_ENABLE_EXPERIMENTAL)
add_definitions(-DGLM_FORCE_CTOR_INIT)

# Buscar paquetes
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(assimp CONFIG REQUIRED)

# Configurar FreeGLUT (Local)
set(FREEGLUT_ROOT "${CMAKE_SOURCE_DIR}/freeglut-MSVC/freeglut")
include_directories("${FREEGLUT_ROOT}/include")
link_directories("${FREEGLUT_ROOT}/lib/x64")

# Archivos de SOIL2
set(SOIL2_SOURCES
    dependencies/soil2/image_array.c
    dependencies/soil2/image_DXT.c
    dependencies/soil2/image_helper.c
    dependencies/soil2/SOIL2.c
    dependencies/soil2/wfETC.c
)

# Funcion para agregar un laboratorio
function(add_lab_target NAME SRC_DIR)
    add_executable(${NAME}
        ${SRC_DIR}/main.cpp
        ${SOIL2_SOURCES}
    )
    target_include_directories(${NAME} PRIVATE ${SRC_DIR})
    
    # Intentar usar GLEW dinamico para x64-windows
    target_link_libraries(${NAME}
        PRIVATE
        GLEW::GLEW
        glfw 
        glm::glm
        freeglut
        assimp::assimp
    )
    if(WIN32)
        target_link_libraries(${NAME} PRIVATE opengl32 user32 gdi32 shell32)
    endif()
endfunction()

# Definicion de ejecutables para cada fase
add_lab_target(Carga_Modelos src/Carga_Modelos)
