#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

static unsigned int CompileShader( unsigned int type,const std::string& source)
{
    unsigned int id= glCreateShader(type);

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
        std::cout << "Fail to compile" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") <<std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
        }

    return id;
}

// Input the two 
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs =CompileShader(GL_VERTEX_SHADER,vertexShader);
    unsigned int fs =CompileShader(GL_FRAGMENT_SHADER,fragmentShader);

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

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Use glew to get the function pointers in the local driver,so that we can use them*/
    if (glewInit() != GLEW_OK)
        std::cerr << "Error" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;


    float positions[6] =
    {
       -0.5f,  -0.5f,
        0.0f,   0.0f,
        0.5f,  -0.5f,
    };

    // The idea of generating a buffer
    unsigned int buffer;
    glGenBuffers(1,&buffer);// 1 represents that you want exactly one element to generate

    //Bind-context
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    //This code means that the current time you bind with sth,so you don't need to cite which buffer you will draw.Just bind it.
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

    //These 2 func should called before bind
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2,0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    std::string vertexShader =
        "#version 330 core\n"//Specifit the version 
        "\n"
        "layout(location = 0) in vec4 position;\n"
        "\n"
        "void main()\n"
        "{\n"
        "  gl_Position = position;\n"
        "}\n";

    std::string fragmentShader =
        "#version 330 core\n"//Specifit the version 
        "\n"
        "layout(location = 0) out vec4 color;\n"
        "\n"
        "void main()\n"
        "{\n"
        "  color = vec4(1.0,0.0,0.0,1.0);\n"
        "}\n";

    unsigned int shader = CreateShader(vertexShader,fragmentShader);
    glUseProgram(shader);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw a traingle
        glDrawArrays(GL_TRIANGLES,0,3);
        //glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}