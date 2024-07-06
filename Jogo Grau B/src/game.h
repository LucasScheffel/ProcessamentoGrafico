#ifndef GAME_H
#define GAME_H

#include "tilemap.h"
#include "character.h"
#include "shader.h"

class Game {
public:
    Game();
    void init();
    void update(float deltaTime);
    void render();
    void handleInput();

private:
    Tilemap tilemap;
    Character character;
    Shader shader;

    void loadResources();
};

#endif // GAME_H
