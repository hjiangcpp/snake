#include "gamedisplay.h"

#include <SDL3/SDL.h>

#include <cstddef>
#include <cstdint>
#include <deque>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

#include "gamestatus.h"
#include "sdl.h"

namespace gamedisplay {

Game::Game(const char* title, int width, int height, int pixel_size)
    : pixel_size_(pixel_size), snake_(width, height) {
  sdl::checkError(SDL_Init(SDL_INIT_VIDEO) == 0);
  window_ = std::make_unique<sdl::Window>(title, width * pixel_size,
                                          height * pixel_size);
  renderer_ = std::make_unique<sdl::Renderer>(window_->createRenderer());
}

void Game::drawElement(const std::pair<int, int>& obj, const SDL_Color& color) {
  auto logicalX = obj.first;
  auto logicalY = obj.second;

  SDL_FRect rect = {static_cast<float>(logicalX * pixel_size_),
                    static_cast<float>(logicalY * pixel_size_),
                    static_cast<float>(pixel_size_),
                    static_cast<float>(pixel_size_)};

  renderer_->setDrawColor(color);
  renderer_->fillRect(&rect);
}

void Game::drawBody(const std::deque<std::pair<int, int>>& obj,
                    const SDL_Color& color) {
  for (auto& element : obj) {
    drawElement(element, color);
  }
}

void Game::render() {
  SDL_Color bkg_color = {255, 255, 255, 255};
  SDL_Color food_color = {185, 87, 86, 255};
  SDL_Color body_color = {42, 76, 101, 255};

  renderer_->setDrawColor(bkg_color);
  renderer_->clear();
  drawElement(snake_.getFood(), food_color);
  drawBody(snake_.getBody().deque(), body_color);
  renderer_->present();
}

void Game::handleEvents() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_EVENT_QUIT:
        is_running_ = false;
        break;
      case SDL_EVENT_KEY_DOWN:
        switch ((event.key.keysym.sym)) {
          case SDLK_UP:
            snake_.newDirection(gamestatus::Direction::UP);
            break;
          case SDLK_DOWN:
            snake_.newDirection(gamestatus::Direction::DOWN);
            break;
          case SDLK_LEFT:
            snake_.newDirection(gamestatus::Direction::LEFT);
            break;
          case SDLK_RIGHT:
            snake_.newDirection(gamestatus::Direction::RIGHT);
            break;
          default:
            break;
        }
      default:
        break;
    }
  }
}

void Game::run() {
  std::cout << "Game Start!\n";
  Uint32 time_ms = 300;
  while (is_running_) {
    handleEvents();
    switch (snake_.next()) {
      case gamestatus::NextState::EAT:
        std::cout << "Current Score: " << snake_.getScore() << "\n";
        break;
      case gamestatus::NextState::DIE:
        is_running_ = false;
        std::cout << "Game Over! Final Score: " << snake_.getScore() << "\n";
        break;
      default:
        break;
    }

    render();
    sdl::delay(time_ms);
  }
}

}  // namespace gamedisplay
