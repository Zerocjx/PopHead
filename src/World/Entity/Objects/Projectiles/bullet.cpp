#include "bullet.hpp"

using PopHead::World::Entity::Bullet;

Bullet::Bullet(Base::GameData* gameData, const std::string& name, float damage, float range)
:Projectile(gameData, name, damage, range)
{
}

void Bullet::dealDamage(Object&)
{

}