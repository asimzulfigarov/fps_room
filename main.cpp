
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <memory>


#include "Renderer.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

#include "tools/cpp/runfiles/runfiles.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


using bazel::tools::cpp::runfiles::Runfiles;

int main(int argc, char** argv)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Room Walker", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "[FATAL] GLEW init failed" << std::endl;
        return -1;
    }

    GLCall(glEnable(GL_DEPTH_TEST)); 

    std::cout << glGetString(GL_VERSION) << std::endl;


    /////______________________________________________________________________////

      /* Resolve shader path via Bazel runfiles */
    std::string error;
        std::string argv0 = argv[0];
        std::unique_ptr<Runfiles> runfiles(Runfiles::Create(argv0, &error));

        if (!runfiles)
        {
            std::cout << "[FATAL] Runfiles failed: " << error << std::endl;
            return -1;
        }

        std::string shaderPath  = runfiles->Rlocation(
            "opengl_app/shaders/application.shader"
        );
        
        std::cout << "Shader path: " << shaderPath  << std::endl;



        /////______________________________________________________________________////

          /* Scope so all GL objects destruct before glfwTerminate() */

    //define buffer and indicies
    {
        float vertices[] = {
    // positions
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f
};

        VertexArray va;
        VertexBuffer vb(vertices, 36 * 3 * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(3);
        va.AddBuffer(vb, layout);

        Shader shader(shaderPath);
        shader.Bind();
        shader.SetUniform4f("u_Color", 1.0f, 0.5f, 0.2f, 1.0f);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 640.0f / 480.0f, 0.1f, 100.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));

        while (!glfwWindowShouldClose(window))
        {
            GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            float time = (float)glfwGetTime();
            glm::mat4 model = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.5f, 1.0f, 0.0f));

            shader.Bind();
            shader.SetUniformMat4f("u_Model", model);
            shader.SetUniformMat4f("u_View", view);
            shader.SetUniformMat4f("u_Projection", projection);

            va.Bind();
            GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
    glfwTerminate();
    return 0;
}

