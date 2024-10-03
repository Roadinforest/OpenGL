#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cassert>

#define GLCall(x) GLClearError(); \
x;\
assert(GLLogCall(#x,__FILE__,__LINE__))
//The x turn the x function into a string

typedef void(APIENTRY* DEBUGPROC)
(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam);

DEBUGPROC callBack=[]
(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
		//printf("I am here\n");
	//std::cout << "Serverity:" <<severity << '\n'<< message << std::endl;
};

static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

static void GLCheckError()
{
	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR)
	{
		std::cout << "[OpenGL Error] (" << error << ")" << std::endl;
	}
}

static bool GLLogCall(const char* function , const char* file ,int line)
{
	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR)
	{
		std::cout << "[OpenGL Error] (" << error << ")" << function << " " << file <<":"<<line<<std::endl;
		return false;
	}
	return true;
}

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string & filePath)
{
	std::ifstream stream(filePath);

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;

	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)//npos means that it doesn't get the word
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
			//set the fragment shader
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
		//std::cout << line << std::endl;
	}

	return { ss[0].str(),ss[1].str() };
}
/*
这段代码中将我在大一中所用到的字符串知识和文件读写知识“实际”应用起来了
而不是说单纯只是应付一下任务，就草草了事
*/

static unsigned int CompileShader(unsigned int type, const std::string & source)
{
	unsigned int id = glCreateShader(type);

	const char* src = source.c_str();
	//Turning the string into c_string is because string is not a pure array,it can extend and any other things
	//Many API now still use C_string , whihc is ended with \0

	glShaderSource(id, 1, &src, nullptr);
	//The 4th param is nullptr rather than the pointer of the length. 
	//This means that the string is by default ended with a null terminator , which is \0
	//This is also the reason why we use the source.c_str() above.It can really save time.
	//Again, read the file and notice as much as details as you can, cause a tiny mistake can lead to a black screen and throw you into a mess

	glCompileShader(id);

	//   TODO: ERROR Handler
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Fail to compile" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

// Input the two 
static unsigned int CreateShader(const std::string & vertexShader, const std::string & fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	//Link them into one program
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);
	//There is another function called glDetachShader function , which will clear the source code together
	//This is not recommended

	return program;
}


int main(void)
{
	GLFWwindow* window;
	std::cout << "Test" << std::endl;
	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);
;
	/* Use glew to get the function pointers in the local driver,so that we can use them*/
	if (glewInit() != GLEW_OK)
		std::cerr << "Error" << std::endl;

	std::cout << glGetString(GL_VERSION) << std::endl;


	float positions[] =
	{
	   -0.5f,  -0.5f,
		0.5f,  -0.5f,
		0.5f,   0.5f,
	   -0.5f,   0.5f,
	};

	unsigned int indices[] = {
		0,1,2,
		2,3,0,
	};

	unsigned int vao;
	GLCall(glGenVertexArrays(1, &vao));
	GLCall(glBindVertexArray(vao));

	int triangleNum = sizeof(positions) / sizeof(float) / 6;
	std::cout << "Number:" << triangleNum << std::endl;

	// The idea of generating a buffer
	unsigned int buffer;
	glGenBuffers(1, &buffer);// 1 represents that you want exactly one element to generate
	//Bind-context
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	//This code means that the current time you bind with sth,so you don't need to cite which buffer you will draw.Just bind it.
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

	//These 2 func should called before bind
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	// Store the indice data from CPU into GPU
	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	std::cout << "Size of indices " << sizeof(indices) << std::endl;

	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
	//std::cout << source.VertexString << std::endl;
	//std::cout << source.FragmentString << std::endl;
	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
	glUseProgram(shader);

	int location = glGetUniformLocation(shader,"u_Color");
	assert(location != -1);
	float r=0.0f;


	float increment = 0.05f;
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));
		//GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		glDebugMessageCallback(callBack, nullptr);

		if (r > 1.0f)
			increment = -0.05f;
		else if(r<0.0f)
			increment = 0.05f;

		r += increment;

		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}