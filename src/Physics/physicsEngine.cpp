#include "physicsEngine.hpp"

#include "collisionBody.hpp"

#include <iostream>

using PopHead::Physics::PhysicsEngine;
using PopHead::Physics::CollisionBody;
using PopHead::Physics::CollisionAxis;

void PhysicsEngine::addStaticBody(CollisionBody* staticBodyPtr)
{
    mStaticBodies.emplace_back(std::move(staticBodyPtr));
}

void PhysicsEngine::addKinematicBody(CollisionBody* kinematicBodyPtr)
{
    mKinematicBodies.emplace_back(std::move(kinematicBodyPtr));
}

void PhysicsEngine::removeStaticBody(CollisionBody* staticBodyPtr)
{
    for(auto it = mStaticBodies.begin(); it != mStaticBodies.end(); ++it){
        if(*it == staticBodyPtr){
            mStaticBodies.erase(it);
            break;
        }
    }
}

void PhysicsEngine::removeKinematicBody(CollisionBody* kinematicBodyPtr)
{
    for(auto it = mKinematicBodies.begin(); it != mKinematicBodies.end(); ++it){
        if(*it == kinematicBodyPtr){
            mKinematicBodies.erase(it);
            break;
        }
    }
}

void PhysicsEngine::clear() noexcept
{
	mStaticBodies.clear();
	mKinematicBodies.clear();
}

void PhysicsEngine::update(sf::Time delta)
{
    for(auto kinematicBody : mKinematicBodies)
    {
		kinematicBody->movePhysics();
		handleStaticCollisionsForThisKinematicBody(kinematicBody);
		kinematicBody->setPositionOfGraphicRepresentation();
		kinematicBody->setPreviousPositionToCurrentPosition();
    }
}

void PhysicsEngine::handleStaticCollisionsForThisKinematicBody(CollisionBody* kinematicBody)
{
	for (const auto& staticBody : mStaticBodies) {
		CollisionAxis axis = getAxisOfCollision(kinematicBody, staticBody);
		if(axis != CollisionAxis::none)
			kinematicBody->setPositionToPreviousPosition(axis);
	}
}

auto PhysicsEngine::getAxisOfCollision(CollisionBody* kinematicBody, CollisionBody* staticBody) -> CollisionAxis
{
	if (isThereCollision(kinematicBody->mRect, staticBody->mRect))
	{
		if(isBodyBetweenTopAndBottomAxisesOfAnotherBody(kinematicBody, staticBody)){
			std::cout << "X" << std::endl;
			return CollisionAxis::x;
		}
		else {
			std::cout << "Y" << std::endl; 
			return CollisionAxis::y;
		}
	}
	else {
		return CollisionAxis::none;
	}
}

bool PhysicsEngine::isBodyBetweenTopAndBottomAxisesOfAnotherBody(CollisionBody* bodyA, CollisionBody* bodyB)
{
	return
		(bodyA->mRect.top + bodyA->mRect.height > bodyB->mRect.top &&
		 bodyA->mRect.top < bodyB->mRect.top + bodyB->mRect.height);
}

bool PhysicsEngine::isThereCollision(sf::FloatRect A, sf::FloatRect B)
{
	//AABB collision detection algorithm
	return(
	A.left < B.left + B.width &&
	A.left + A.width > B.left &&
	A.top < B.top + B.height &&
	A.top + A.height > B.top);
}

