#include <iostream>
#include "character.h"
#include "stb_image.h"


Character::Character(const std::string& spriteSheetPath, int frameWidth, int frameHeight, int numFrames)
    : frameWidth(frameWidth), frameHeight(frameHeight), numFrames(numFrames), currentFrame(0), frameDuration(0.1f), elapsedTime(0.0f) {
    loadTexture(spriteSheetPath);
}

void Character::loadTexture(const std::string& spriteSheetPath) {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(spriteSheetPath.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = GL_RGBA;
        if (nrChannels == 1) format = GL_RED;
        else if (nrChannels == 3) format = GL_RGB;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Failed to load texture: " << spriteSheetPath << std::endl;
    }
    stbi_image_free(data);
}

void Character::update(float deltaTime) {
    elapsedTime += deltaTime;
    if (elapsedTime >= frameDuration) {
        nextFrame();
        elapsedTime = 0.0f;
    }
}

void Character::nextFrame() {
    currentFrame = (currentFrame + 1) % numFrames;
}

void Character::draw(const glm::vec2& position) {
    // Implementar a renderização do personagem
    // Usar o shader para renderizar a animação do personagem
}
