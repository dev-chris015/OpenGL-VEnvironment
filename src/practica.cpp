#include<iostream>
#include<vector>
#include<cmath>
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
void generatePolygon(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, int numSides, float centerX, float centerY, float radius, GLuint baseIndexOffset) {
	for (int i = 0; i < numSides; i++) {
		float angle = 2.0f * M_PI * i / numSides + (M_PI / numSides); // Rotación ligera para que se vean mejor
		float vx = centerX + radius * cos(angle);
		float vy = centerY + radius * sin(angle);
		
		// Mapeo de coordenadas de textura (0 a 1 basado en la caja delimitadora del círculo)
		float tx = (cos(angle) + 1.0f) * 0.5f;
		float ty = (sin(angle) + 1.0f) * 0.5f;

		vertices.push_back({vx, vy, 0.0f, 1.0f, 1.0f, 1.0f, tx, ty});
	}

	// Generar índices usando un abanico (fan) desde el primer vértice
	for (int i = 1; i < numSides - 1; i++) {
		indices.push_back(baseIndexOffset);
		indices.push_back(baseIndexOffset + i);
		indices.push_back(baseIndexOffset + i + 1);
	}
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 800, "Texturas Practica", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	gladLoadGL();
	glViewport(0, 0, 800, 800);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader shaderProgram("Resource Files/shaders/default.vert", "Resource Files/shaders/default.frag");

	std::vector<Vertex> allVertices;
	std::vector<GLuint> allIndices;

	// Generar los 4 polígonos
	// 1. Cuadrado (4 lados) - Sup. Izq.
	generatePolygon(allVertices, allIndices, 4, -0.5f, 0.5f, 0.35f, 0);
	// 2. Pentágono (5 lados) - Sup. Der.
	generatePolygon(allVertices, allIndices, 5, 0.5f, 0.5f, 0.35f, 4);
	// 3. Hexágono (6 lados) - Inf. Izq.
	generatePolygon(allVertices, allIndices, 6, -0.5f, -0.5f, 0.35f, 4 + 5);
	// 4. Heptágono (7 lados) - Inf. Der.
	generatePolygon(allVertices, allIndices, 7, 0.5f, -0.5f, 0.35f, 4 + 5 + 6);

	VAO VAO1;
	VAO1.Bind();
	VBO VBO1((GLfloat*)allVertices.data(), allVertices.size() * sizeof(Vertex));
	EBO EBO1(allIndices.data(), allIndices.size() * sizeof(GLuint));

	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));

	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

	// Cargar Texturas
	Texture tex1("Resource Files/Textures/textura#1.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	Texture tex2("Resource Files/Textures/textura#2.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	Texture tex3("Resource Files/Textures/textura#3.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	Texture tex4("Resource Files/Textures/textura#4.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);

	tex1.texUnit(shaderProgram, "tex0", 0);

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		shaderProgram.Activate();
		glUniform1f(uniID, 0.0f);
		VAO1.Bind();

		// 1. Dibujar Cuadrado (4 lados -> 2 triángulos -> 6 índices)
		tex1.Bind();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

		// 2. Dibujar Pentágono (5 lados -> 3 triángulos -> 9 índices)
		tex2.Bind();
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, (void*)(6 * sizeof(GLuint)));

		// 3. Pentágono usó 9 índices, así que el Hexágono empieza en offset 6+9=15
		// 3. Dibujar Hexágono (6 lados -> 4 triángulos -> 12 índices)
		tex3.Bind();
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, (void*)(15 * sizeof(GLuint)));

		// 4. Hexágono usó 12 índices, Heptágono empieza en 15+12=27
		// 4. Dibujar Heptágono (7 lados -> 5 triángulos -> 15 índices)
		tex4.Bind();
		glDrawElements(GL_TRIANGLES, 15, GL_UNSIGNED_INT, (void*)(27 * sizeof(GLuint)));

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	tex1.Delete(); tex2.Delete(); tex3.Delete(); tex4.Delete();
	shaderProgram.Delete();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
