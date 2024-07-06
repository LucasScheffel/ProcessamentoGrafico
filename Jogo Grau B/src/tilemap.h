#ifndef TILEMAP_H
#define TILEMAP_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "shader.h"

class Tilemap {
public:
    Tilemap(const std::string& mapConfigPath, Shader& shader);
    void loadTileset();
    void draw();

private:
    std::string tilesetPath;
    int tileCount;
    int tileWidth, tileHeight;
    int mapWidth, mapHeight;
    std::vector<std::vector<int>> mapData;
    GLuint textureID;
    Shader& shader;

    void loadMapConfig(const std::string& mapConfigPath);
    void loadTexture(const std::string& tilesetPath);
};

#endif // TILEMAP_H
