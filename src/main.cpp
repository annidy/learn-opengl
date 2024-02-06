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
#include "Texture.h"

constexpr auto vertexShaderSource = R"(
    #version 330 core
    
    layout (location = 0) in vec4 aPos;
    layout (location = 1) in vec2 texCoord;

    out vec2 v_TexCoord;

    uniform mat4 u_MVP;

    void main()
    {
        gl_Position = u_MVP * aPos;
        v_TexCoord = texCoord;
    }
)";

constexpr auto fragmentShaderSource = R"(
    #version 330 core

    in vec2 v_TexCoord;
    out vec4 FragColor;

    uniform sampler2D u_Texture;

    void main()
    {
        vec4 texColor = texture(u_Texture, v_TexCoord);
        FragColor = texColor;
    }
)";

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

    GLCall( glEnable(GL_BLEND) );
    GLCall( glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) );

    // glfwSetFramebufferSizeCallback(window, framebufferSizeChanged);

    float positions[] = {
        -0.5f, -0.5f, 0.0f, 0.0f, // 0
         0.5f, -0.5f, 1.0f, 0.0f, // 1
         0.5f,  0.5f, 1.0f, 1.0f, // 2
        -0.5f,  0.5f, 0.0f, 1.0f  // 3
    };
    VertexArray va;
    VertexBuffer vb(positions, sizeof(positions));
    VertexBufferLayout vbl;
    vbl.AddFloat(2); 
    vbl.AddFloat(2); 
    va.AddBuffer(vb, vbl);

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    IndexBuffer ib(indices, 6);
    glm::mat4 proj = glm::ortho(-2.0f, 2.f, -.5f, .5f, -1.0f, 1.0f);


    unsigned int shaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
    glUseProgram(shaderProgram);

    Texture texture("res/textures/phone.png");
    texture.Bind(0);

    // 清空状态机
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0); 

    glUniform1i(glGetUniformLocation(shaderProgram, "u_Texture"), 0);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "u_MVP"), 1, GL_FALSE, &proj[0][0]);

    float red = 0.0f;
    float step = 0.05f;

    Renderer renderer;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        renderer.Clear();

        renderer.Draw(va, ib);

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
