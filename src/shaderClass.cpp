#include"shaderClass.h"

std::string get_file_contents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);

	}
	throw(errno);
}

//Procedemos a contruir el constructor de sombras

Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
	std::string vertexCode = get_file_contents(vertexFile);
	std::string fragmentCode = get_file_contents(fragmentFile);

	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	//Create Vertex Shader Object and get its reference
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	//Attach Vertex Shader source to the Vertex Shader Object
	glShaderSource(vertexShader, 1, &vertexSource, NULL);

	//Compile the Vertex Shader into machine code
	glCompileShader(vertexShader);

	// Check for vertex shader compile errors
	GLint hasCompiled;
	char infoLog[1024];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &hasCompiled);
	if (hasCompiled == GL_FALSE) {
		glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
		std::cout << "SHADER_COMPILATION_ERROR for: VERTEX\n" << infoLog << std::endl;
	}

	//Create Fragment Shader Object and get its reference
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	//Attach Fragment Shader source to the Fragment Shader Object
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);

	//Compile the Vertex Shader into machine code
	glCompileShader(fragmentShader);

	// Check for fragment shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &hasCompiled);
	if (hasCompiled == GL_FALSE) {
		glGetShaderInfoLog(fragmentShader, 1024, NULL, infoLog);
		std::cout << "SHADER_COMPILATION_ERROR for: FRAGMENT\n" << infoLog << std::endl;
	}

	//Create Shader Program Object and get its reference
	ID = glCreateProgram();

	//Attach the Vertex and Fragment Shaders to the Shader Program
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);

	//Link all the shaders together into the Shader Program
	glLinkProgram(ID);

	// Check for linking errors
	glGetProgramiv(ID, GL_LINK_STATUS, &hasCompiled);
	if (hasCompiled == GL_FALSE) {
		glGetProgramInfoLog(ID, 1024, NULL, infoLog);
		std::cout << "SHADER_LINKING_ERROR\n" << infoLog << std::endl;
	}

	//Delete the now useless Vertex and Fragment Shader Objects
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::Activate()
{
	glUseProgram(ID);
}

void Shader::Delete()
{
	glDeleteProgram(ID);
}
