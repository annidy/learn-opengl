#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <iostream>

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Renderer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"

constexpr auto vertexShaderSource = R"(
    #version 330 core
    
    layout (location = 0) in vec4 aPos;

    void main()
    {
        gl_Position = aPos;
    }
)";

constexpr auto fragmentShaderSource = R"(
    #version 330 core

    out vec4 FragColor;

    uniform vec4 u_Color;

    void main()
    {
        FragColor = u_Color;
    }
)";

enum VAO_IDs { Triangles, NumVAOs};
enum Buffer_IDs { ArrayBuffer, ElementBuffer, NumBuffers };
enum Attrib_IDs { vPosition, NumAttributes };

unsigned int CompileShader(const std::string &source, unsigned int type)
{
    unsigned int id = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char *message = (char *)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cerr << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cerr << message << std::endl;
        glDeleteShader(id);
        return 0;
    }
    return id;
}

unsigned int CreateShaderProgram(const std::string &vertexShaderSource, const std::string &fragmentShaderSource)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(vertexShaderSource, GL_VERTEX_SHADER);
    unsigned int fs = CompileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);
    // Optional: Delete the shaders once linked
    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

void framebufferSizeChanged(GLFWwindow *window, int width, int height);

static void error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

int main()
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    GLFWwindow *window = glfwCreateWindow(600, 480, "OpenGL", NULL, NULL);
    if (window == NULL)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    fprintf(stdout, "GLEW version: %s\n", glewGetString(GLEW_VERSION));
    fprintf(stdout, "GL version: %s\n", glGetString(GL_VERSION));

    // glfwSetFramebufferSizeCallback(window, framebufferSizeChanged);

    float positions[] = {
        -0.5f, -0.5f,  // 0
         0.5f, -0.5f,  // 1
         0.5f, 0.5f,   // 2
        -0.5f, 0.5f,   // 3
    };
    VertexArray va;
    VertexBuffer vb(positions, sizeof(positions));
    VertexBufferLayout vbl;
    vbl.AddFloat(2); 
    va.AddBuffer(vb, vbl);

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    IndexBuffer ib(indices, 6);

    unsigned int shaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
    glUseProgram(shaderProgram);

    // 清空状态机
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0); 

    glUniform4f(glGetUniformLocation(shaderProgram, "u_Color"), 0.0f, 0.3f, 0.0f, 1.0f);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        va.Bind();
        ib.Bind();
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void framebufferSizeChanged(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
