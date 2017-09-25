//
// Created by fisk on 21/09/17.
//

#include <iostream>

#include "Game.h"
void Game::run() {
  tick = 0;
  running = true;

  while (running) {
    tick++;
    main();

    struct timespec tim, tim2;
    tim.tv_sec = 0;
    tim.tv_nsec = 30000000;

    nanosleep(&tim, &tim2);
  }
}

void Game::stop() {
  running = false;
}
