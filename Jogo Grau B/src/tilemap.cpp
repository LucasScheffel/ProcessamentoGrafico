#include "tilemap.h"
#include <iostream>
#include <fstream>
#include <sstream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Tilemap::Tilemap(const std::string& mapConfigPath, Shader& shader) : shader(shader) {
    loadMapConfig(mapConfigPath);
    loadTileset();
}

void Tilemap::loadMapConfig(const std::string& mapConfigPath) {
    std::ifstream file(mapConfigPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open map config file: " << mapConfigPath << std::endl;
        return;
    }

    file >> tilesetPath >> tileCount >> tileWidth >> tileHeight;
    file >> mapWidth >> mapHeight;

    mapData.resize(mapHeight, std::vector<int>(mapWidth));
    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            file >> mapData[y][x];
        }
    }
}

void Tilemap::loadTileset() {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(tilesetPath.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = GL_RGBA;
        if (nrChannels == 1) format = GL_RED;
        else if (nrChannels == 3) format = GL_RGB;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Failed to load texture: " << tilesetPath << std::endl;
    }
    stbi_image_free(data);
}

void Tilemap::draw() {
    shader.use();

    glm::mat4 projection = glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f);
    shader.setUniform("projection", projection);

    glBindTexture(GL_TEXTURE_2D, textureID);

    float tileWidthVisible = 420.0f; // Largura visível do tile em pixels
    float tileHeightVisible = 337.0f; // Altura visível do tile em pixels

    float texTileWidth = tileWidthVisible / 1380.0f;
    float texTileHeight = tileHeightVisible / 1073.0f;

    float scaleX = 1.2f; // Ajuste da proporção
    float scaleY = 1.2f;

    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            int tileIndex = mapData[y][x];
            float tx = (tileIndex % 3) * texTileWidth;
            float ty = 1.0f - (tileIndex / 3) * texTileHeight - texTileHeight;

            glm::mat4 model = glm::mat4(1.0f);
            float xpos = (x - y) * (tileWidth / 2.0f) * scaleX;
            float ypos = (x + y) * (tileHeight / 2.0f) * scaleY;
            model = glm::translate(model, glm::vec3(xpos + 960, ypos - 100, 0.0f)); // Ajuste para centralizar na tela
            model = glm::scale(model, glm::vec3(scaleX, scaleY, 1.0f)); // Ajustar escala
            shader.setUniform("model", model);

            float vertices[] = {
                0.0f,          0.0f,           0.0f, tx,                  ty + texTileHeight,
                tileWidth,     0.0f,           0.0f, tx + texTileWidth,   ty + texTileHeight,
                tileWidth,     tileHeight,     0.0f, tx + texTileWidth,   ty,
                0.0f,          tileHeight,     0.0f, tx,                  ty
            };

            unsigned int indices[] = {
                0, 1, 2,
                2, 3, 0
            };

            GLuint VBO, VAO, EBO;
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

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
        }
    }
}
