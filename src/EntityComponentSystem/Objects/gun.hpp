#pragma once

#include "EntityComponentSystem/Objects/character.hpp"

namespace ph {

enum class ShotDirection 
{
	east, northEast, north, northWest, west, southWest, south, southEast
};

class Bullet
{
public:
	Bullet(const Entity& opponentsNode, const sf::Vector2f direction, const sf::Vector2f startPosition,
           const float damage, const unsigned range);

private:
	auto getCharacterWhoWasShot() -> Character*;
	bool wasEnemyShot(Character&);
	bool isBulletStillInItsRange();
	void makeSpriteOfShot();
	void dealDamage();

private:
	const sf::Vector2f mDirection;
	const sf::Vector2f mStartPosition;
	unsigned mTraveledDistance;
	const Entity& mEnemiesNode;
	const float mDamage;
	const unsigned mRange;
};

class Gun : public Object
{
public:
	Gun(GameData* const, const float damage);

	void shoot(const ShotDirection) const;
	auto getShotDirectionVector(const ShotDirection) const -> const sf::Vector2f;
	void draw(sf::RenderTarget&, const sf::RenderStates) const override;

private:
	const float mDamage;
};

}