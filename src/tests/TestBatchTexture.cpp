#include "TestBatchTexture.h"

#include "Debug.h"
#include "imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace test
{
    TestBatchTexture::TestBatchTexture() :
        m_ClearColor { 0.8f, 0.3f, 0.2f, 1.0f },
        m_Positions {
            150.0f, 150.0f, 0.0f, 0.0f, 0.0f, // 0
            250.0f, 150.0f, 1.0f, 0.0f, 0.0f, // 1
            250.0f, 250.0f, 1.0f, 1.0f, 0.0f, // 2
            150.0f, 250.0f, 0.0f, 1.0f, 0.0f, // 3

             50.0f,   50.0f, 0.0f, 0.0f, 1.0f,// 0
            150.0f,   50.0f, 1.0f, 0.0f, 1.0f,// 1
            150.0f,  150.0f, 1.0f, 1.0f, 1.0f,// 2
             50.0f,  150.0f, 0.0f, 1.0f, 1.0f,// 3
        },
        m_Indices {
            0, 1, 2,
            2, 3, 0,
            4, 5, 6,
            6, 7, 4
        },
        m_va(),
        m_ib(m_Indices, 12),
        m_vb(m_Positions, 5 * 8 * sizeof(float)),
        m_layout(),
        m_shader("res/shaders/BatchTexture.shader"),
        m_texture0("res/textures/phone.png"),
        m_texture1("res/textures/gold-dollar.png"),
        m_renderer(),
        m_proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)),
        m_view(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)))
    {
        m_layout.AddFloat(2);
        m_layout.AddFloat(2);
        m_layout.AddFloat(1);
        m_va.AddBuffer(m_vb, m_layout);

        int samplers[2] = {0, 1};
        m_shader.SetUniform1iv("u_Textures", 2, samplers);
    }

    TestBatchTexture::~TestBatchTexture()
    {
        m_va.Unbind();
        m_shader.Unbind();
    }

    void TestBatchTexture::OnUpdate(float deltaTime)
    {
    }

    void TestBatchTexture::OnRender()
    {
        m_renderer.Clear();
        GLCall(glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]));
        {
            m_shader.Bind();
            glm::mat4 mvp = m_proj * m_view;
            m_shader.SetUniformMat4f("u_MVP", mvp);
            m_texture0.Bind(0);
            m_texture1.Bind(1);
            m_renderer.Draw(m_va, m_ib, m_shader);
        }
    }

    void TestBatchTexture::OnImGuiRender()
    {
        ImGui::ColorEdit4("Batch Texture", m_ClearColor);
    }
};