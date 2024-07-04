#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Vertex Shader Source
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform vec2 offset;

void main()
{
    TexCoord = aTexCoord + offset;
    gl_Position = vec4(aPos, 1.0);
}
)";

// Fragment Shader Source
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, TexCoord);
}
)";

// Função para compilação dos shaders
GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shader;
}

// Classe Sprite com controle de renderização e animação
class Sprite {
public:
    GLuint VAO, VBO, EBO, texture;
    int nAnimations, nFrames;
    float ds, dt;
    int iFrame, iAnimation;
    float lastTime;
    float frameDuration;
    bool isMoving;

    Sprite(const char* texturePath, int nAnimations, int nFrames, float frameDuration)
        : nAnimations(nAnimations), nFrames(nFrames), frameDuration(frameDuration), isMoving(false) {
        ds = 1.0f / nFrames;
        dt = 1.0f; // Atualize esta linha para 1.0f já que a sprite sheet tem uma única linha
        iFrame = 0;
        iAnimation = 0;
        lastTime = 0.0f;

        // Carregar textura
        loadTexture(texturePath);

        // Vértices
        float vertices[] = {
            -0.5f,  0.5f, 0.0f,  0.0f, 0.0f,
            -0.5f, -0.5f, 0.0f,  0.0f, dt,
             0.5f,  0.5f, 0.0f,  ds, 0.0f,
             0.5f, -0.5f, 0.0f,  ds, dt
        };
        unsigned int indices[] = {
            0, 1, 2,
            1, 2, 3
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    void loadTexture(const char* path) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        // Wrapping/Filtros
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Carregar imagem
        int width, height, nrChannels;
        unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            std::cerr << "Failed to load texture: " << path << std::endl;
        }
        stbi_image_free(data);
    }

    void update(float currentTime) {
        if (isMoving && (currentTime - lastTime >= frameDuration)) {
            iFrame = (iFrame + 1) % nFrames;
            lastTime = currentTime;
        }
    }

    void draw(GLuint shaderProgram, float posX, float posY) {
        glUseProgram(shaderProgram);

        float offsetS = iFrame * ds;
        float offsetT = iAnimation * dt;

        GLuint offsetLoc = glGetUniformLocation(shaderProgram, "offset");
        glUniform2f(offsetLoc, offsetS, offsetT);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(posX, posY, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 1.0f)); // Ajuste a escala conforme necessário
        GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
};

class CharacterController : public Sprite {
public:
    float posX, posY;
    float speed;

    CharacterController(const char* texturePath, int nAnimations, int nFrames, float frameDuration)
        : Sprite(texturePath, nAnimations, nFrames, frameDuration), posX(0.0f), posY(0.0f), speed(2.0f) {}

    void processInput(GLFWwindow* window, float deltaTime) {
        bool moved = false;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            posY += speed * deltaTime;
            moved = true;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            posY -= speed * deltaTime;
            moved = true;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            posX -= speed * deltaTime;
            moved = true;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            posX += speed * deltaTime;
            moved = true;
        }

        isMoving = moved; // Atualiza o estado de movimento
    }

    void draw(GLuint shaderProgram) {
        Sprite::draw(shaderProgram, posX, posY);
    }
};

int main() {
    // Inicialização da GLFW and GLEW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "Sprite Animation", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Compilação dos Shader e criação dos Shader Programs
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Sprite e Controller
    CharacterController character("../Sprites/Samurai Pack/Samurai/Run.png", 1, 8, 0.1f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float lastFrame = glfwGetTime();

    // Loop de Renderização
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Processar Input
        character.processInput(window, deltaTime);

        // Atualizar animação
        character.update(currentFrame);

        // Render
        glClear(GL_COLOR_BUFFER_BIT);
        character.draw(shaderProgram);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
