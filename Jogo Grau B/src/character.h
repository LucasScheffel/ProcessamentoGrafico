#ifndef CHARACTER_H
#define CHARACTER_H

#include <GL/glew.h>
#include <string>
#include <glm/glm.hpp>

class Character {
public:
    Character(const std::string& spriteSheetPath, int frameWidth, int frameHeight, int numFrames);
    void update(float deltaTime);
    void draw(const glm::vec2& position);

private:
    GLuint textureID;
    int frameWidth, frameHeight;
    int numFrames;
    int currentFrame;
    float frameDuration;
    float elapsedTime;

    void loadTexture(const std::string& spriteSheetPath);
    void nextFrame();
};

#endif // CHARACTER_H
