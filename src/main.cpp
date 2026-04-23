#include<iostream>
#include<vector>
#include<cmath>
#include<fstream>
#include<string>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>

#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"Texture.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Estructura para organizar los datos de los vértices
struct Vertex {
	float x, y, z;
	float r, g, b;
	float tx, ty;
};

// Función para generar vértices de un polígono regular
void generatePolygon(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, int numSides, float centerX, float centerY, float radius) {
	for (int i = 0; i < numSides; i++) {
		// Rotación para que un vértice apunte hacia arriba (90 grados o PI/2)
		float angle = 2.0f * M_PI * i / numSides + (M_PI / 2.0f);
		float vx = centerX + radius * cos(angle);
		float vy = centerY + radius * sin(angle);
		
		// Mapeo de coordenadas de textura (0 a 1 basado en la caja delimitadora del círculo)
		float tx = (cos(angle) + 1.0f) * 0.5f;
		float ty = (sin(angle) + 1.0f) * 0.5f;

		vertices.push_back({vx, vy, 0.0f, 1.0f, 1.0f, 1.0f, tx, ty});
	}

	// Generar índices usando un abanico (fan) desde el primer vértice (centro implícito para polígonos convexos)
	for (int i = 1; i < numSides - 1; i++) {
		indices.push_back(0);
		indices.push_back(i);
		indices.push_back(i + 1);
	}
}

int main()
{
	// Lógica para ciclar entre las texturas con cada ejecución
	int textureIndex = 0;
	{
		std::ifstream inFile(".texture_index");
		if (inFile.is_open()) {
			inFile >> textureIndex;
			textureIndex = (textureIndex + 1) % 4;
		}
		std::ofstream outFile(".texture_index");
		if (outFile.is_open()) {
			outFile << textureIndex;
		}
	}

	const char* texturePaths[] = {
		"Resource Files/Textures/textura#1.jpg",
		"Resource Files/Textures/textura#2.jpg",
		"Resource Files/Textures/textura#3.jpg",
		"Resource Files/Textures/textura#4.jpg"
	};

	// Definicion de polígonos
	int numSides = textureIndex + 5; 
	const char* polygonNames[] = { "Pentagono", "Hexagono", "Heptagono", "Octagono" };

	std::cout << "Ejecucion No. " << textureIndex + 1 << std::endl;
	std::cout << " - Poligono: " << polygonNames[textureIndex] << " (" << numSides << " lados)" << std::endl;
	std::cout << " - Textura: " << texturePaths[textureIndex] << std::endl;

	// Inicializar GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	std::string title = std::string(polygonNames[textureIndex]) + " con Textura #" + std::to_string(textureIndex + 1);
	GLFWwindow* window = glfwCreateWindow(800, 800, title.c_str(), NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	
	// Cargar GLAD
	gladLoadGL();
	glViewport(0, 0, 800, 800);

	// Cargar Shaders
	Shader shaderProgram("Resource Files/shaders/default.vert", "Resource Files/shaders/default.frag");

	// Generar el polígono correspondiente centrado en el origen (0.0, 0.0)
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	generatePolygon(vertices, indices, numSides, 0.0f, 0.0f, 0.6f);

	// Configurar VAO, VBO y EBO
	VAO VAO1;
	VAO1.Bind();
	VBO VBO1((GLfloat*)vertices.data(), vertices.size() * sizeof(Vertex));
	EBO EBO1(indices.data(), indices.size() * sizeof(GLuint));

	// Enlazar atributos: 0=Position, 1=Color, 2=TexCoord
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));

	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	// Obtener ubicación del uniform "scale" definido en el shader
	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

	// Cargar la textura correspondiente a esta ejecución
	Texture texture(texturePaths[textureIndex], GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	texture.texUnit(shaderProgram, "tex0", 0);

	// Loop principal
	while (!glfwWindowShouldClose(window))
	{
		// Color de fondo
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		shaderProgram.Activate();
		// Forzamos el uniform scale a 0.0f para asegurar que el polígono esté perfectamente centrado
		glUniform1f(uniID, 0.0f);
		
		texture.Bind();
		VAO1.Bind();
		
		// Dibujar el polígono usando los índices (GL_TRIANGLES con EBO)
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Limpieza de recursos
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	texture.Delete();
	shaderProgram.Delete();
	glfwDestroyWindow(window);
	glfwTerminate();
	
	return 0;
}
