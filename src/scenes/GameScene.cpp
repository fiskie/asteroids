#include <sstream>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <input/KeyboardController.h>
#include <utility/drawing.h>
#include "GameScene.h"
#include "GameOverScene.h"

void GameScene::drawHud(IWindowRenderer *renderer) {
  auto font = renderer->getFont();
  auto window = renderer->getWindow();
  auto view = renderer->getView();
  int offset = 4;

  for (const auto &session: game->getSessions()) {
    sf::Text nameText(session->player->getName(), font, 14);
    sf::Text scoreText("Score: " + std::to_string(session->score), font, 14);

    // Bing bing wahoo
    std::stringstream marios;

    for (int i = 0; i < session->lives; i++) {
      marios << "^";
    }

    sf::Text livesText(marios.str(), font, 14);

    nameText.setFillColor(session->player->getColor());

    nameText.setPosition(offset, 0);
    scoreText.setPosition(offset, 14);
    livesText.setPosition(offset, 28);

    window->draw(nameText);
    window->draw(scoreText);
    window->draw(livesText);

    offset += 96;
  }

  if (paused) {
    sf::Text pausedText("[Paused]", font, 16);

    auto bounds = pausedText.getLocalBounds();
    pausedText.setOrigin(sf::Vector2f(int(bounds.left + bounds.width / 2), 0));

    auto center = view.getSize().x / 2;
    auto middle = view.getSize().y / 2;

    pausedText.setPosition(center, middle - 14);
    window->draw(pausedText);
  }

  if (waveId == 0) {
    sf::Text readyText("Get Ready!!!", font, 16);

    auto bounds = readyText.getLocalBounds();
    readyText.setOrigin(sf::Vector2f(int(bounds.left + bounds.width / 2), 0));

    auto center = view.getSize().x / 2;
    readyText.setPosition(center, 64);
    window->draw(readyText);
  }
}

void GameScene::main() {
  if (!paused) {
    if (waveTimer <= 0) {
      startWave();
    }

    waveTimer -= 1;

    world.update();

    int remainingAsteroids = 0;

    for (auto object : world.getObjects()) {
      if (object->getClass() == "asteroid") {
        remainingAsteroids++;
      }
    }

    if (remainingAsteroids == 0 && waveId != 0) {
      startWave();
    }

    updateRespawnTimers();

    if (getRemainingPlayerCount() == 0) {
      world.clear();
      game->setScene(new GameOverScene(game));
    }
  }
}

void GameScene::startRespawnTimer(PlayerSession *playerSession) {
  respawnTimers.push_back(std::make_unique<respawn_timer_t>(respawn_timer_t{
      session: playerSession,
      time: RESPAWN_TIME
  }));
}

void GameScene::drawWaveBar(IWindowRenderer *renderer) {
  auto font = renderer->getFont();
  auto view = renderer->getView();
  auto window = renderer->getWindow();

  auto maxWidth = view.getSize().x / 4;
  auto barWidth = (maxWidth / BASE_WAVE_INTERVAL_TIME) * waveTimer;
  auto position = sf::Vector2f(int((view.getSize().x / 8) * 3), int(view.getSize().y - 16));

  progress_bar_t progress_bar;
  progress_bar.pos = sf::Vector2f(position);
  progress_bar.origin = sf::Vector2f(0, 0);
  progress_bar.progress = static_cast<int>(barWidth);
  progress_bar.maxProgress = static_cast<int>(maxWidth);
  progress_bar.fillColor = sf::Color(49, 88, 150);
  progress_bar.height = 4;
  progress_bar.gutter = 1;

  drawing::drawProgressBar(
      window,
      progress_bar
  );

  sf::Text waveText("Wave " + std::to_string(waveId), font, 16);
  waveText.setPosition(sf::Vector2f(view.getSize().x / 2, view.getSize().y - 40));

  auto bounds = waveText.getLocalBounds();
  waveText.setOrigin(sf::Vector2f(int(bounds.left + bounds.width / 2), 0));
  waveText.setFillColor(sf::Color(255, 255, 255));

  if (waveTimer > BASE_WAVE_INTERVAL_TIME - 50 && waveTimer / 2 % 2 == 0) {
    waveText.setFillColor(sf::Color(127, 127, 127));
  }

  window->draw(waveText);
}

GameScene::GameScene(IGame *game) : world(game, 640, 480) {
  this->game = game;

  // load only the fragment shader
  if (!this->shader.loadFromFile("assets/nebulae.frag", sf::Shader::Fragment))
  {
    exit(253);
  }
}