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
#include "Shader.h"
#include "Texture.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "tests/TestClearColor.h"
#include "tests/TestTriangle.h"
#include "tests/TestUniform.h"
#include "tests/TestMultipleObjects.h"
#include "tests/TestBatchTexture.h"


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
        0.0f, 0.0f, 0.0f, 0.0f, // 0
        100.0f, 0.0f, 1.0f, 0.0f, // 1
        100.0f, 100.0f, 1.0f, 1.0f, // 2
        0.0f, 100.0f, 0.0f, 1.0f  // 3
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
    glm::mat4 proj = glm::ortho(0.0f, 600.0f, 0.0f, 480.0f, -1.0f, 1.0f);

    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));


    Shader shaderProgram("res/shaders/Basic.shader");
    shaderProgram.Bind();

    Texture texture("res/textures/gold-dollar.png");
    texture.Bind(0);

    // 清空状态机
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0); 

    shaderProgram.SetUniform1i("u_Texture", 0);
    shaderProgram.SetUniformMat4f("u_MVP", proj * view);

    Renderer renderer;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCanvasResizeCallback("#canvas");
#endif
    ImGui_ImplOpenGL3_Init("#version 150");

    int currentSelection = -1;
    int radioSelection = 0;
    test::Test *test;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        renderer.Clear();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::RadioButton("ClearColor",      &radioSelection, 0); ImGui::SameLine();
            ImGui::RadioButton("Triangle",        &radioSelection, 1); ImGui::SameLine();
            ImGui::RadioButton("Uniform",         &radioSelection, 2); ImGui::SameLine();
            ImGui::RadioButton("MultipleObjects", &radioSelection, 3); ImGui::SameLine();
            ImGui::RadioButton("BatchTexture", &radioSelection, 4);
        }

        if (currentSelection != radioSelection)
        {
            switch(radioSelection)
            {
                case 0 : delete test;
                         test = new test::TestClearColor();
                         break;
                case 1 : delete test;
                         test = new test::TestTriangle();
                         break;
                case 2 : delete test;
                         test = new test::TestUniform();
                         break;
                case 3 : delete test;
                         test = new test::TestMultipleObjects();
                         break;
                case 4 : delete test;
                         test = new test::TestBatchTexture();
                         break;
            }
            currentSelection = radioSelection;
        }

        test->OnUpdate(0.0f);
        test->OnRender();
        test->OnImGuiRender();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebufferSizeChanged(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
