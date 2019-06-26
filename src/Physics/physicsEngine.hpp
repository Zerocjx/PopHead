#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "CollisionHandlers/staticCollisionHandler.hpp"
#include "CollisionHandlers/kinematicCollisionHandler.hpp"

namespace ph{

class CollisionBody;

class PhysicsEngine
{
public:
    void addStaticBody(CollisionBody* staticBodyPtr);
    void addKinematicBody(CollisionBody* kinematicBodyPtr);

    void removeStaticBody(CollisionBody* staticBodyPtr);
    void removeKinematicBody(CollisionBody* kinematicBodyPtr);
private:
	void removeBody(std::vector<CollisionBody*>& bodies, CollisionBody* body);

public:
    void clear() noexcept;

    void update(sf::Time delta);
private:
    void handleStaticCollisionsFor(CollisionBody* kinematicBody);
    void handleKinematicCollisionsFor(CollisionBody* kinematicBody);
    bool isThereCollision(sf::FloatRect bodyA, sf::FloatRect bodyB);

private:
    std::vector<CollisionBody*> mStaticBodies;
    std::vector<CollisionBody*> mKinematicBodies;

    StaticCollisionHandler mStaticCollisionHandler;
    KinematicCollisionHandler mKinematicCollisionHandler;
};

}
