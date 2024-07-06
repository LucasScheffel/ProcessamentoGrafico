#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "tilemap.h"
#include "shader.h"

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

int main() {
    // Inicialização da GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Criação da janela GLFW
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Tilemap Isometrico", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Inicialização da GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Inicialização do shader
    Shader shader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");

    // Inicialização do tilemap
    Tilemap tilemap("../config/map.txt", shader);

    // Loop principal do jogo
    while (!glfwWindowShouldClose(window)) {
        // Processa entradas do usuário
        processInput(window);

        // Renderiza o jogo
        glClear(GL_COLOR_BUFFER_BIT);
        tilemap.draw();

        // Troca os buffers e processa eventos
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Libera recursos e encerra a aplicação
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}
