#include <utility/vector.h>
#include <utility/drawing.h>
#include "Asteroid.h"
#include "player/PlayerSession.h"
#include "TemporaryParticle.h"

void Asteroid::onDestroyed() {
  if (size > 13) {
    auto ast1 = new Asteroid(world, size / 2);
    ast1->pos.x = pos.x - 6;
    ast1->pos.y = pos.y;
    ast1->vel = vector::rot(vel, vector::rot(vel) + 5);

    world->pushObject(ast1);

    auto ast2 = new Asteroid(world, size / 2);
    ast2->pos.x = pos.x + 6;
    ast2->pos.y = pos.y;
    ast2->vel = vector::rot(vel, vector::rot(vel) - 5);

    world->pushObject(ast2);
  }

  auto explosion = new ParticleSystem(128, 128);;
  explosion->setColor(sf::Color(255, 255, 255));
  explosion->setParticleSpeed(20);
  explosion->fuel(25);
  explosion->setDissolve(true);
  explosion->setDissolutionRate(10);
  explosion->setShape(Shape::CIRCLE);
  auto emitter = new TemporaryParticle(world, explosion);
  emitter->pos.x = pos.x;
  emitter->pos.y = pos.y;
  world->pushObject(emitter);
}

bool Asteroid::isRecyclable() {
  return isDestroyed();
}

void Asteroid::update() {
  this->pos += vel;
  this->rot += rotSpeed;
}

void Asteroid::renderTo(sf::RenderWindow *renderWindow) {
  shape.setPosition(pos);
  shape.setRotation(rot);

  renderWindow->draw(shape);

  if (health < getMaxHealth()) {
    drawing::drawHealthBar(
        renderWindow,
        getHealth(),
        getMaxHealth(),
        sf::Vector2f(pos.x, pos.y + 40),
        sf::Vector2f(origin.x, 20)
    );
  }
}

void Asteroid::onBulletHit(Bullet *bullet) {
  // take damage
  takeDamage(10);
  vel.x += bullet->vel.x / 30;
  vel.y += bullet->vel.y / 30;
  bullet->setLifetime(0);

  if (isDestroyed()) {
    auto session = bullet->getOwner();
    session->setScore(session->getScore() + getScoreValue());
  }

  auto ricochet = new ParticleSystem(128, 128);
  ricochet->setColor(bullet->getOwner()->getPlayer()->getColor());
  ricochet->setParticleSpeed(20);
  ricochet->fuel(10);
  ricochet->setDissolve(true);
  ricochet->setDissolutionRate(10);
  ricochet->setShape(Shape::CIRCLE);
  auto emitter = new TemporaryParticle(world, ricochet);
  emitter->pos.x = bullet->pos.x;
  emitter->pos.y = bullet->pos.y;
  world->pushObject(emitter);
}

void Asteroid::onAsteroidHit(Asteroid *other) {
  // bounce
  auto diff = vel - other->vel;
  diff.x /= 1.05;
  diff.y /= 1.05;

  vel -= diff;
  other->vel += diff;

  auto rotDiff = rotSpeed - other->rotSpeed;
  rotDiff /= 10.0;

  rotSpeed -= rotDiff;
  other->rotSpeed += rotDiff;
}

void Asteroid::onShipHit(Ship *other) {
  if (!other->isOnInvincibilityCooldown()) {
    other->onDestroyed();
  }
}

void Asteroid::onCollision(AbstractWorldObject *other) {
  if (other->getClass() == WorldObjectClass::BULLET) {
    onBulletHit(dynamic_cast<Bullet *>(other));
  }

  if (other->getClass() == WorldObjectClass::ASTEROID) {
    onAsteroidHit(dynamic_cast<Asteroid *>(other));
  }

  if (other->getClass() == WorldObjectClass::SHIP) {
    onShipHit(dynamic_cast<Ship *>(other));
  }
}
Asteroid::Asteroid(WorldInterface *world, float size) : AbstractWorldObject(world) {
  this->size = size;
  rotSpeed = (-5 + float(random() % 10)) / 10;

  auto edges = 5 + size / 20;
  points = shape::generateShape(static_cast<unsigned long>(edges), size);
  origin = sf::Vector2f(size / 2, size / 2);
  health = this->getMaxHealth();

  shape.setPointCount(points.size());

  for (size_t i = 0; i < points.size(); i++) {
    shape.setPoint(i, points[i]);
  }

  shape.setFillColor(fillColor);
  shape.setOutlineColor(outlineColor);
  shape.setOutlineThickness(1.0F);
  shape.setOrigin(origin.x, origin.y);
}
void Asteroid::takeDamage(int damage) {
  // guards against triggering onDestroyed() multiple times if hit again before entity recycle
  if (health > 1) {
    health -= damage;

    if (health <= 0)
      this->onDestroyed();
  }
}