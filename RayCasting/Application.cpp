#include <GL/glew.h>
// Include GLFW
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <assert.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"
#include "vendor/stb_image.h"
glm::mat4 rotate = glm::mat4(1.0f);
float angx = 0.0f, angy = 0.0f, angz = 0.0f;
#define ASSERT(x) if (!(x)) assert(false)
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLCheckError())
glm::vec3 key_brd(0.0f, 0.0f, 0.0f);
glm::vec3 vscale(1.0f, 1.0f, 1.0f);

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
GLFWwindow* window;

static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

static bool GLCheckError()
{
	while (GLenum error = glGetError())
	{

		std::cout << "[OpenGL Error] ";
		switch (error) {
		case GL_INVALID_ENUM:
			std::cout << "GL_INVALID_ENUM : An unacceptable value is specified for an enumerated argument.";
			break;
		case GL_INVALID_VALUE:
			std::cout << "GL_INVALID_OPERATION : A numeric argument is out of range.";
			break;
		case GL_INVALID_OPERATION:
			std::cout << "GL_INVALID_OPERATION : The specified operation is not allowed in the current state.";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION : The framebuffer object is not complete.";
			break;
		case GL_OUT_OF_MEMORY:
			std::cout << "GL_OUT_OF_MEMORY : There is not enough memory left to execute the command.";
			break;
		case GL_STACK_UNDERFLOW:
			std::cout << "GL_STACK_UNDERFLOW : An attempt has been made to perform an operation that would cause an internal stack to underflow.";
			break;
		case GL_STACK_OVERFLOW:
			std::cout << "GL_STACK_OVERFLOW : An attempt has been made to perform an operation that would cause an internal stack to overflow.";
			break;
		default:
			std::cout << "Unrecognized error" << error;
		}
		std::cout << std::endl;
		return false;
	}
	return true;
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	GLCall(unsigned int id = glCreateShader(type));
	const char* src = source.c_str();
	GLCall(glShaderSource(id, 1, &src, nullptr));
	GLCall(glCompileShader(id));

	// Error handling
	int result;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
	std::cout << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader compile status: " << result << std::endl;
	if (result == GL_FALSE)
	{
		int length;
		GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		char* message = (char*)alloca(length * sizeof(char));
		GLCall(glGetShaderInfoLog(id, length, &length, message));
		std::cout
			<< "Failed to compile "
			<< (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
			<< "shader"
			<< std::endl;
		std::cout << message << std::endl;
		GLCall(glDeleteShader(id));
		return 0;
	}

	return id;
}

int main(void)
{
	unsigned int m_RendererIDn(0);
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(1024, 768, "Raycast", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

	// Initialize GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	float positions[] = {
	//Front
	-0.25f,    -0.25f,  0.0,	 0.0f,  0.0f, 1.0f,   0.0f, 0.0f,// 0
	 0.0f,     -0.25f,  0.0,	 0.0f,  0.0f, 1.0f,   1.0f, 0.0f,// 1
	 0.0f,      0.0f,   0.0,	 0.0f,  0.0f, 1.0f,   1.0f, 1.0f,// 2
	-0.25f,     0.0f,   0.0,	 0.0f,  0.0f, 1.0f,   0.0f, 1.0f,// 3
	//Back
	-0.25f,    -0.25f,  -0.25,   0.0f,  0.0f, -1.0f,  0.0f, 0.0f, //4
	 0.0f,     -0.25f,  -0.25,   0.0f,  0.0f, -1.0f,  1.0f, 0.0f,//5
	 0.0f,      0.0f,   -0.25,   0.0f,  0.0f, -1.0f,  1.0f, 1.0f,//6
	-0.25f,     0.0f,   -0.25,   0.0f,  0.0f, -1.0f,  0.0f, 1.0f,//7
	//top
	-0.25,      0.0f,    0.0,    -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,//8
	-0.25,      0.0f,   -0.25,   -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,//9
	 0.0,       0.0,     0.0,    -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,//10
	 0.0f,      0.0f,   -0.25,   -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,//11
	//left
	-0.25f,     0.0f,    0.0,     0.0f, -1.0f,  0.0f, 0.0f, 0.0f,//12
	-0.25,      0.0,    -0.25,    0.0f, -1.0f,  0.0f, 0.0f, 0.0f,//13
	-0.25f,    -0.25f,   0.0,     0.0f, -1.0f,  0.0f, 0.0f, 1.0f,//14
	-0.25,     -0.25,   -0.25,    0.0f, -1.0f,  0.0f, 0.0f, 1.0f, //15
	//right
	0.0f,      -0.25f,   0.0,     0.0f,   1.0f,  0.0f, 1.0f, 0.0f,// 16
	0.0f,       0.0f,    0.0,     0.0f,   1.0f,  0.0f, 1.0f, 1.0f,// 17
	0.0,       -0.25,   -0.25,    0.0f,   1.0f,  0.0f, 1.0f, 0.0f,//18
	0.0,        0.0,    -0.25,    0.0f,   1.0f,  0.0f, 1.0f, 1.0f,//19
	//bottom
	-0.25f,	   -0.25f,	  0.0,	   1.0f,  0.0f,  0.0f, 0.0f, 0.0f,// 20
	 0.0,	   -0.25,	 -0.25,	   1.0f,  0.0f,  0.0f, 1.0f, 0.0f,// 21
	 0.0f,	   -0.25f,	  0.0,	   1.0f,  0.0f,  0.0f, 1.0f, 0.0f,//22	
	-0.25,	   -0.25,	 -0.25,	   1.0f,  0.0f,  0.0f, 0.0f, 0.0f //23
	};

	unsigned  int indices[6][6] = {
		{0, 1, 2,
		2, 3, 0},	

		{8,9,10,
		9,10,11},

		{20, 21,22,
		20, 21,23}, 

		{4, 5, 6,
		6, 7, 4},

		{16,17,18,
		17,18,19},

		{12,13,14,
		14,13,15}
	};

	int m_Width = 0, m_Height = 0, m_BPP = 0;
	stbi_set_flip_vertically_on_load(1);
	unsigned char* m_LocalBuffer_color;
	m_LocalBuffer_color = new unsigned char[180 * 1 * 4];
	m_LocalBuffer_color = stbi_load("res/textures/matplotlib-virdis.png", &m_Width, &m_Height, &m_BPP, 4);

	unsigned int vao;
	GLCall(glGenVertexArrays(1, &vao));
	GLCall(glBindVertexArray(vao));

	unsigned int buffer;
	GLCall(glGenBuffers(1, &buffer));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
	GLCall(glBufferData(GL_ARRAY_BUFFER, 24 * 8 * sizeof(float), positions, GL_STATIC_DRAW));

	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0));
	GLCall(glEnableVertexAttribArray(0));

	GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(GL_FLOAT))));
	GLCall(glEnableVertexAttribArray(1));

	GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(GL_FLOAT))));
	GLCall(glEnableVertexAttribArray(2));

	unsigned int ibo;
	GLCall(glGenBuffers(1, &ibo));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));

	std::ifstream stream("res/shader/Basic.shader");
	std::string line;
	std::stringstream ss[2];
	int Shadertype = -1;

	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				Shadertype = 0;
			else if (line.find("fragment") != std::string::npos)
				Shadertype = 1;
		}
		else
		{
			ss[Shadertype] << line << '\n';
		}
	}
	unsigned int shader = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, ss[0].str());
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, ss[1].str());

	GLCall(glAttachShader(shader, vs));
	GLCall(glAttachShader(shader, fs));
	GLCall(glLinkProgram(shader));
	GLint program_linked;
	GLCall(glGetProgramiv(shader, GL_LINK_STATUS, &program_linked));
	std::cout << "Program link status: " << program_linked << std::endl;
	if (program_linked != GL_TRUE)
	{
		GLsizei log_length = 0;
		GLchar message[1024];
		GLCall(glGetProgramInfoLog(shader, 1024, &log_length, message));
		std::cout << "Failed to link program" << std::endl;
		std::cout << message << std::endl;
	}

	GLCall(glValidateProgram(shader));
	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));
	GLCall(unsigned int u_ColorMap = glGetUniformLocation(shader, "colormap"));

	GLCall(glUseProgram(0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	GLCall(glBindVertexArray(0));
	GLCall(glUseProgram(shader));
	GLCall(glBindVertexArray(vao));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glMatrixMode(GL_MODELVIEW);
	glm::vec3 view = glm::vec3(-1.0f, -1.0f, 1.0f);
	glm::mat4 model(1.0f);

	GLCall(glGenTextures(1, &m_RendererIDn));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererIDn));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 180, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer_color));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	GLint viewLoc = glGetUniformLocation(shader, "view");

	do {		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 trans = glm::translate(model, key_brd);
		model = glm::scale(glm::mat4(1.0f), vscale);
		model = glm::rotate(model, angx, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, angy, glm::vec3(0.0f, 0.8f, 0.0f));
		model = glm::rotate(model, angz, glm::vec3(0.0f, 0.0f, 0.3f));
		model = trans * model;

		GLCall(glActiveTexture(GL_TEXTURE0));
		GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererIDn));
		GLCall(glUniform1i(u_ColorMap, 0));
		GLint modelLoc = glGetUniformLocation(shader, "model");
		
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(unsigned int), indices, GL_STATIC_DRAW));
		GLCall(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr));

		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	glDisable(GL_BLEND);
	GLCall(glDeleteBuffers(1, &buffer));
	GLCall(glDeleteVertexArrays(1, &vao));
	GLCall(glDeleteProgram(shader));
	glfwTerminate();
	return 0;
}
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	key_brd = glm::vec3(0.0f, 0.0f, 0.0f);
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		key_brd = glm::vec3(0.0f, 0.01f, 0.0f);

	}
	else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		key_brd = glm::vec3(0.0f, -0.01f, 0.0f);
	}
	else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
	{
		key_brd = glm::vec3(0.01f, 0.0f, 0.0f);
	}
	else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
	{
		key_brd = glm::vec3(-0.01f, 0.0f, 0.0f);
	}
	else if (key == GLFW_KEY_KP_ADD && action == GLFW_PRESS)
	{
		vscale = glm::vec3(1.01f, 1.01f, 1.01f);
	}
	else if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS)
	{
		vscale = glm::vec3(0.99f, 0.99f, 0.99f);
	}
	else if (key == GLFW_KEY_KP_MULTIPLY && action == GLFW_PRESS)
	{
		angx = 0.01f;
		angy = 0.01f;
		angz = 0.01f;
	}
	else if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		angx = 0.01f;
	}
	else if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		angy = 0.01f;
	}
	else if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		angz = 0.01f;
	}
	else
	{
		angx = 0.0f;
		angy = 0.0f;
		angz = 0.0f;
		key_brd = glm::vec3(0.0f, 0.0f, 0.0f);
		vscale = glm::vec3(1.0f, 1.0f, 1.0f);
	}
}
