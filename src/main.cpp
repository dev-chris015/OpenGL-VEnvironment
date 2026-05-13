#include<iostream>
#include<vector>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include"Texture.h"
#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"Camara.h"

const unsigned int width = 800;
const unsigned int height = 800;

glm::vec3 calculateNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
    return glm::normalize(glm::cross(p2 - p1, p3 - p1));
}

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 texCoord;
    glm::vec3 normal;
};

std::vector<Vertex> octahedronVertices;
std::vector<GLuint> octahedronIndices;

void setupOctahedron() {
    glm::vec3 top(0.0f, 0.5f, 0.0f);
    glm::vec3 bottom(0.0f, -0.5f, 0.0f);
    glm::vec3 p1(0.5f, 0.0f, 0.0f);
    glm::vec3 p2(0.0f, 0.0f, 0.5f);
    glm::vec3 p3(-0.5f, 0.0f, 0.0f);
    glm::vec3 p4(0.0f, 0.0f, -0.5f);
    glm::vec3 color(1.0f, 1.0f, 1.0f);
    auto addFace = [&](glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) {
        glm::vec3 n = calculateNormal(v1, v2, v3);
        GLuint startIdx = (GLuint)octahedronVertices.size();
        octahedronVertices.push_back({v1, color, glm::vec2(0.5f, 1.0f), n});
        octahedronVertices.push_back({v2, color, glm::vec2(0.0f, 0.0f), n});
        octahedronVertices.push_back({v3, color, glm::vec2(1.0f, 0.0f), n});
        octahedronIndices.push_back(startIdx);
        octahedronIndices.push_back(startIdx + 1);
        octahedronIndices.push_back(startIdx + 2);h
    };
    addFace(top, p1, p2); addFace(top, p2, p3); addFace(top, p3, p4); addFace(top, p4, p1);
    addFace(bottom, p2, p1); addFace(bottom, p3, p2); addFace(bottom, p4, p3); addFace(bottom, p1, p4);
}

std::vector<Vertex> prismVertices;
std::vector<GLuint> prismIndices;

void setupPrism() {
    float h = 0.5f, r = 0.4f;
    glm::vec3 t1(0.0f, h, r), t2(-0.346f, h, -0.2f), t3(0.346f, h, -0.2f);
    glm::vec3 b1(0.0f, -h, r), b2(-0.346f, -h, -0.2f), b3(0.346f, -h, -0.2f);
    glm::vec3 color(1.0f, 1.0f, 1.0f);
    auto addTri = [&](glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) {
        glm::vec3 n = calculateNormal(v1, v2, v3);
        GLuint startIdx = (GLuint)prismVertices.size();
        prismVertices.push_back({v1, color, glm::vec2(0.5f, 1.0f), n});
        prismVertices.push_back({v2, color, glm::vec2(0.0f, 0.0f), n});
        prismVertices.push_back({v3, color, glm::vec2(1.0f, 0.0f), n});
        prismIndices.push_back(startIdx); prismIndices.push_back(startIdx + 1); prismIndices.push_back(startIdx + 2);
    };
    auto addRect = [&](glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4) {
        glm::vec3 n = calculateNormal(v1, v2, v3);
        GLuint startIdx = (GLuint)prismVertices.size();
        prismVertices.push_back({v1, color, glm::vec2(0.0f, 1.0f), n});
        prismVertices.push_back({v2, color, glm::vec2(0.0f, 0.0f), n});
        prismVertices.push_back({v3, color, glm::vec2(1.0f, 0.0f), n});
        prismVertices.push_back({v4, color, glm::vec2(1.0f, 1.0f), n});
        prismIndices.push_back(startIdx); prismIndices.push_back(startIdx + 1); prismIndices.push_back(startIdx + 2);
        prismIndices.push_back(startIdx); prismIndices.push_back(startIdx + 2); prismIndices.push_back(startIdx + 3);
    };
    addTri(t1, t2, t3); addTri(b1, b3, b2);
    addRect(t1, b1, b2, t2); addRect(t2, b2, b3, t3); addRect(t3, b3, b1, t1);
}

std::vector<Vertex> pentaVertices;
std::vector<GLuint> pentaIndices;

void setupPenta() {
    float h = 0.5f, r = 0.4f;
    std::vector<glm::vec3> topPts, botPts;
    for (int i = 0; i < 5; ++i) {
        float angle = glm::radians(72.0f * i);
        topPts.push_back(glm::vec3(r * cos(angle), h, r * sin(angle)));
        botPts.push_back(glm::vec3(r * cos(angle), -h, r * sin(angle)));
    }
    glm::vec3 color(1.0f, 1.0f, 1.0f);
    glm::vec3 topCenter(0.0f, h, 0.0f), topNormal(0.0f, 1.0f, 0.0f);
    for (int i = 0; i < 5; ++i) {
        GLuint startIdx = (GLuint)pentaVertices.size();
        pentaVertices.push_back({topCenter, color, glm::vec2(0.5f, 0.5f), topNormal});
        pentaVertices.push_back({topPts[i], color, glm::vec2(0.5f + 0.5f * cos(glm::radians(72.0f * i)), 0.5f + 0.5f * sin(glm::radians(72.0f * i))), topNormal});
        pentaVertices.push_back({topPts[(i + 1) % 5], color, glm::vec2(0.5f + 0.5f * cos(glm::radians(72.0f * (i + 1))), 0.5f + 0.5f * sin(glm::radians(72.0f * (i + 1)))), topNormal});
        pentaIndices.push_back(startIdx); pentaIndices.push_back(startIdx + 1); pentaIndices.push_back(startIdx + 2);
    }
    glm::vec3 botCenter(0.0f, -h, 0.0f), botNormal(0.0f, -1.0f, 0.0f);
    for (int i = 0; i < 5; ++i) {
        GLuint startIdx = (GLuint)pentaVertices.size();
        pentaVertices.push_back({botCenter, color, glm::vec2(0.5f, 0.5f), botNormal});
        pentaVertices.push_back({botPts[(i + 1) % 5], color, glm::vec2(0.5f + 0.5f * cos(glm::radians(72.0f * (i + 1))), 0.5f + 0.5f * sin(glm::radians(72.0f * (i + 1)))), botNormal});
        pentaVertices.push_back({botPts[i], color, glm::vec2(0.5f + 0.5f * cos(glm::radians(72.0f * i)), 0.5f + 0.5f * sin(glm::radians(72.0f * i))), botNormal});
        pentaIndices.push_back(startIdx); pentaIndices.push_back(startIdx + 1); pentaIndices.push_back(startIdx + 2);
    }
    for (int i = 0; i < 5; ++i) {
        glm::vec3 n = calculateNormal(topPts[i], botPts[i], botPts[(i + 1) % 5]);
        GLuint startIdx = (GLuint)pentaVertices.size();
        pentaVertices.push_back({topPts[i], color, glm::vec2(0.0f, 1.0f), n});
        pentaVertices.push_back({botPts[i], color, glm::vec2(0.0f, 0.0f), n});
        pentaVertices.push_back({botPts[(i + 1) % 5], color, glm::vec2(1.0f, 0.0f), n});
        pentaVertices.push_back({topPts[(i + 1) % 5], color, glm::vec2(1.0f, 1.0f), n});
        pentaIndices.push_back(startIdx); pentaIndices.push_back(startIdx + 1); pentaIndices.push_back(startIdx + 2);
        pentaIndices.push_back(startIdx); pentaIndices.push_back(startIdx + 2); pentaIndices.push_back(startIdx + 3);
    }
}

int main()
{
	try {
		std::cout << "Iniciando GLFW..." << std::endl;
		if (!glfwInit()) {
			std::cerr << "Error: No se pudo iniciar GLFW" << std::endl;
			system("pause"); return -1;
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		std::cout << "Creando ventana..." << std::endl;
		GLFWwindow* window = glfwCreateWindow(width, height, "Incorporando manipulacion de la Camara", NULL, NULL);
		if (window == NULL) {
			std::cerr << "Error: No se pudo crear la ventana GLFW" << std::endl;
			glfwTerminate(); system("pause"); return -1;
		}
		glfwMakeContextCurrent(window);

		std::cout << "Iniciando GLAD..." << std::endl;
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::cerr << "Error: No se pudo iniciar GLAD" << std::endl;
			system("pause"); return -1;
		}
		glViewport(0, 0, width, height);

		std::cout << "Cargando shaders..." << std::endl;
		Shader shaderProgram("Resource Files/shaders/default.vert", "Resource Files/shaders/default.frag");
		Shader lightShader("Resource Files/shaders/light.vert", "Resource Files/shaders/light.frag");

		std::cout << "Configurando figuras..." << std::endl;
		setupOctahedron(); setupPrism(); setupPenta();

		VAO vaoOct; vaoOct.Bind();
		VBO vboOct((GLfloat*)octahedronVertices.data(), octahedronVertices.size() * sizeof(Vertex));
		EBO eboOct(octahedronIndices.data(), octahedronIndices.size() * sizeof(GLuint));
		vaoOct.LinkAttrib(vboOct, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
		vaoOct.LinkAttrib(vboOct, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
		vaoOct.LinkAttrib(vboOct, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
		vaoOct.LinkAttrib(vboOct, 3, 3, GL_FLOAT, sizeof(Vertex), (void*)(8 * sizeof(float)));
		vaoOct.Unbind(); vboOct.Unbind(); eboOct.Unbind();

		VAO vaoPrism; vaoPrism.Bind();
		VBO vboPrism((GLfloat*)prismVertices.data(), prismVertices.size() * sizeof(Vertex));
		EBO eboPrism(prismIndices.data(), prismIndices.size() * sizeof(GLuint));
		vaoPrism.LinkAttrib(vboPrism, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
		vaoPrism.LinkAttrib(vboPrism, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
		vaoPrism.LinkAttrib(vboPrism, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
		vaoPrism.LinkAttrib(vboPrism, 3, 3, GL_FLOAT, sizeof(Vertex), (void*)(8 * sizeof(float)));
		vaoPrism.Unbind(); vboPrism.Unbind(); eboPrism.Unbind();

		VAO vaoPenta; vaoPenta.Bind();
		VBO vboPenta((GLfloat*)pentaVertices.data(), pentaVertices.size() * sizeof(Vertex));
		EBO eboPenta(pentaIndices.data(), pentaIndices.size() * sizeof(GLuint));
		vaoPenta.LinkAttrib(vboPenta, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
		vaoPenta.LinkAttrib(vboPenta, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
		vaoPenta.LinkAttrib(vboPenta, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
		vaoPenta.LinkAttrib(vboPenta, 3, 3, GL_FLOAT, sizeof(Vertex), (void*)(8 * sizeof(float)));
		vaoPenta.Unbind(); vboPenta.Unbind(); eboPenta.Unbind();

		GLfloat lightVertices[] = { -0.1f,-0.1f,0.1f,-0.1f,-0.1f,-0.1f,0.1f,-0.1f,-0.1f,0.1f,-0.1f,0.1f,-0.1f,0.1f,0.1f,-0.1f,0.1f,-0.1f,0.1f,0.1f,-0.1f,0.1f,0.1f,0.1f };
		GLuint lightIndices[] = { 0,1,2,0,2,3,0,4,7,0,7,3,3,7,6,3,6,2,2,6,5,2,5,1,1,5,4,1,4,0,4,5,6,4,6,7 };
		VAO lightVAO; lightVAO.Bind();
		VBO lightVBO(lightVertices, sizeof(lightVertices));
		EBO lightEBO(lightIndices, sizeof(lightIndices));
		lightVAO.LinkAttrib(lightVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
		lightVAO.Unbind(); lightVBO.Unbind(); lightEBO.Unbind();

		std::cout << "Cargando texturas..." << std::endl;
		Texture tex1("Resource Files/Textures/textura#1.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
		Texture tex2("Resource Files/Textures/textura#2.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
		Texture tex3("Resource Files/Textures/textura#3.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);

		glm::vec4 lightColorOct = glm::vec4(1.0f, 0.1f, 0.1f, 1.0f);
		glm::vec4 lightColorPrism = glm::vec4(0.1f, 1.0f, 0.1f, 1.0f);
		glm::vec4 lightColorPenta = glm::vec4(0.1f, 0.1f, 1.0f, 1.0f);

		glm::vec3 lightPosOct = glm::vec3(-1.2f, 1.2f, 0.5f);
		glm::vec3 lightPosPrism = glm::vec3(0.0f, 1.2f, 0.5f);
		glm::vec3 lightPosPenta = glm::vec3(1.2f, 1.2f, 0.5f);

		glEnable(GL_DEPTH_TEST);
		Camera camera(width, height, glm::vec3(0.0f, 0.5f, 3.0f));
		float rotation = 0.0f; double prevTime = glfwGetTime();

		std::cout << "Iniciando bucle principal..." << std::endl;
		while (!glfwWindowShouldClose(window)) {
			glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			camera.Inputs(window);
			camera.updateMatrix(45.0f, 0.1f, 100.0f);
			
			shaderProgram.Activate();
			glUniform3f(glGetUniformLocation(shaderProgram.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
			camera.Matrix(shaderProgram, "camMatrix");

			// Octaedro con su luz roja
			glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPosOct.x, lightPosOct.y, lightPosOct.z);
			glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColorOct.x, lightColorOct.y, lightColorOct.z, lightColorOct.w);
			glm::mat4 modelOct = glm::translate(glm::mat4(1.0f), glm::vec3(-1.2f, 0.0f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelOct));
			tex1.Bind(); vaoOct.Bind(); glDrawElements(GL_TRIANGLES, (GLsizei)octahedronIndices.size(), GL_UNSIGNED_INT, 0);

			// Prisma con su luz verde
			glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPosPrism.x, lightPosPrism.y, lightPosPrism.z);
			glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColorPrism.x, lightColorPrism.y, lightColorPrism.z, lightColorPrism.w);
			glm::mat4 modelPrism = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelPrism));
			tex2.Bind(); vaoPrism.Bind(); glDrawElements(GL_TRIANGLES, (GLsizei)prismIndices.size(), GL_UNSIGNED_INT, 0);

			// Pentágono con su luz azul
			glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPosPenta.x, lightPosPenta.y, lightPosPenta.z);
			glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColorPenta.x, lightColorPenta.y, lightColorPenta.z, lightColorPenta.w);
			glm::mat4 modelPenta = glm::translate(glm::mat4(1.0f), glm::vec3(1.2f, 0.0f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelPenta));
			tex3.Bind(); vaoPenta.Bind(); glDrawElements(GL_TRIANGLES, (GLsizei)pentaIndices.size(), GL_UNSIGNED_INT, 0);

			// Renderizar los 3 cubos de luz
			lightShader.Activate(); camera.Matrix(lightShader, "camMatrix");
			
			// Cubo Rojo
			glm::mat4 modelLOct = glm::translate(glm::mat4(1.0f), lightPosOct);
			glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelLOct));
			glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColorOct.x, lightColorOct.y, lightColorOct.z, lightColorOct.w);
			lightVAO.Bind(); glDrawElements(GL_TRIANGLES, sizeof(lightIndices)/sizeof(int), GL_UNSIGNED_INT, 0);

			// Cubo Verde
			glm::mat4 modelLPrism = glm::translate(glm::mat4(1.0f), lightPosPrism);
			glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelLPrism));
			glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColorPrism.x, lightColorPrism.y, lightColorPrism.z, lightColorPrism.w);
			lightVAO.Bind(); glDrawElements(GL_TRIANGLES, sizeof(lightIndices)/sizeof(int), GL_UNSIGNED_INT, 0);

			// Cubo Azul
			glm::mat4 modelLPenta = glm::translate(glm::mat4(1.0f), lightPosPenta);
			glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelLPenta));
			glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColorPenta.x, lightColorPenta.y, lightColorPenta.z, lightColorPenta.w);
			lightVAO.Bind(); glDrawElements(GL_TRIANGLES, sizeof(lightIndices)/sizeof(int), GL_UNSIGNED_INT, 0);

			glfwSwapBuffers(window); glfwPollEvents();
		}
		vaoOct.Delete(); vboOct.Delete(); eboOct.Delete();
		vaoPrism.Delete(); vboPrism.Delete(); eboPrism.Delete();
		vaoPenta.Delete(); vboPenta.Delete(); eboPenta.Delete();
		tex1.Delete(); tex2.Delete(); tex3.Delete();
		shaderProgram.Delete(); lightShader.Delete();
		glfwDestroyWindow(window); glfwTerminate();
		std::cout << "Aplicacion finalizada normalmente." << std::endl;
		system("pause");
	}
	catch (int e) {
		std::cerr << "Excepcion (Error archivo): " << e << std::endl;
		system("pause"); return -1;
	}
	catch (const std::exception& e) {
		std::cerr << "Excepcion: " << e.what() << std::endl;
		system("pause"); return -1;
	}
	catch (...) {
		std::cerr << "Error desconocido." << std::endl;
		system("pause"); return -1;
	}
	return 0;
}
