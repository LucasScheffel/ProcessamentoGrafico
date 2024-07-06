#include "game.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Game::Game() 
    : shader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl"), // Inicialize o shader primeiro
      tilemap("../config/map.txt", shader), // Passe o shader para o Tilemap
      character("../assets/Sprites/PNG/Sword_Run/Sword_Run_full.png", 64, 64, 8) {}

void Game::init() {
    // Inicializar OpenGL, shaders e outros recursos
}

void Game::update(float deltaTime) {
    character.update(deltaTime);
    // Atualizar lógica do jogo
}

void Game::render() {
    shader.use();
    tilemap.draw();
    character.draw(glm::vec2(0.0f, 0.0f));  // Posicionar o personagem corretamente
}

void Game::handleInput() {
    // Lidar com entrada do usuário (teclado)
}

void Game::loadResources() {
    // Carregar texturas, shaders e outros recursos
}
