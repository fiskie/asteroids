//
// Created by fisk on 09/12/17.
//

#ifndef ASTEROIDS_VECTOR_H
#define ASTEROIDS_VECTOR_H

#include <SFML/System/Vector2.hpp>

namespace vector {
sf::Vector2f getFromAngle(float angle);
float len(sf::Vector2f vector);
}

#endif //ASTEROIDS_VECTOR_H
