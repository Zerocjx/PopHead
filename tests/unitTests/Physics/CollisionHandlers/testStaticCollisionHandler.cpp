#include <catch.hpp>

#include "Physics/CollisionHandlers/staticCollisionHandler.hpp"
#include "Physics/CollisionBody/collisionBody.hpp"

namespace ph {

TEST_CASE("Static collision is properly handled", "[Physics][StaticCollisionHandler]") 
{
	StaticCollisionHandler staticCollisionHandler;
	
	CollisionBody kinematicBody({0, 0, 10, 10}, 25);
	kinematicBody.actionsAtTheEndOfPhysicsLoopIteration();

	SECTION("After moving right") {
		kinematicBody.move({10, 0});
		CollisionBody staticBody({15, -10, 20, 30}, 0);
		staticCollisionHandler(kinematicBody, staticBody);
		CHECK(Approx(kinematicBody.getPosition().x).margin(0.001f) == 5.f);
		CHECK(Approx(kinematicBody.getPosition().y).margin(0.001f) == 0.f);
	}
	SECTION("After moving left") {
		kinematicBody.move({-10, 0});
		CollisionBody staticBody({-25, -10, 20, 30}, 0);
		staticCollisionHandler(kinematicBody, staticBody);
		CHECK(Approx(kinematicBody.getPosition().x).margin(0.001f) == -5.f);
		CHECK(Approx(kinematicBody.getPosition().y).margin(0.001f) == 0.f);
	}
	SECTION("After moving down") {
		kinematicBody.move({0, 10});
		CollisionBody staticBody({-10, 15, 30, 20}, 0);
		staticCollisionHandler(kinematicBody, staticBody);
		CHECK(Approx(kinematicBody.getPosition().x).margin(0.001f) == 0.f);
		CHECK(Approx(kinematicBody.getPosition().y).margin(0.001f) == 5.f);
	}
	SECTION("After moving up") {
		kinematicBody.move({0, -10});
		CollisionBody staticBody({-10, -25, 30, 20}, 0);
		staticCollisionHandler(kinematicBody, staticBody);
		CHECK(Approx(kinematicBody.getPosition().x).margin(0.001f) == 0.f);
		CHECK(Approx(kinematicBody.getPosition().y).margin(0.001f) == -5.f);
	}

	SECTION("After moving diagonally down right (wall is to the right from kinematic body)") {
		kinematicBody.move({10, 10});
		CollisionBody staticBody({15, -10, 20, 30}, 0);
		staticCollisionHandler(kinematicBody, staticBody);
		CHECK(Approx(kinematicBody.getPosition().x).margin(0.001f) == 5.f);
		CHECK(Approx(kinematicBody.getPosition().y).margin(0.001f) == 10.f);
	}
	SECTION("After moving diagonally up right (wall is to the right from kinematic body)") {
		kinematicBody.move({10, -10});
		CollisionBody staticBody({15, -10, 20, 30}, 0);
		staticCollisionHandler(kinematicBody, staticBody);
		CHECK(Approx(kinematicBody.getPosition().x).margin(0.001f) == 5.f);
		CHECK(Approx(kinematicBody.getPosition().y).margin(0.001f) == -10.f);
	}

	SECTION("After moving diagonally down left (wall is to the left from kinematic body)") {
		kinematicBody.move({-10, 10});
		CollisionBody staticBody({-25, -10, 20, 30}, 0);
		staticCollisionHandler(kinematicBody, staticBody);
		CHECK(Approx(kinematicBody.getPosition().x).margin(0.001f) == -5.f);
		CHECK(Approx(kinematicBody.getPosition().y).margin(0.001f) == 10.f);
	}
	SECTION("After moving diagonally up left (wall is to the left from kinematic body)") {
		kinematicBody.move({-10, -10});
		CollisionBody staticBody({-25, -10, 20, 30}, 0);
		staticCollisionHandler(kinematicBody, staticBody);
		CHECK(Approx(kinematicBody.getPosition().x).margin(0.001f) == -5.f);
		CHECK(Approx(kinematicBody.getPosition().y).margin(0.001f) == -10.f);
	}

	SECTION("After moving diagonally down left (wall is down from kinematic body)") {
		kinematicBody.move({-10, 10});
		CollisionBody staticBody({-10, 15, 30, 20}, 0);
		staticCollisionHandler(kinematicBody, staticBody);
		CHECK(Approx(kinematicBody.getPosition().x).margin(0.001f) == -10.f);
		CHECK(Approx(kinematicBody.getPosition().y).margin(0.001f) == 5.f);
	}
	SECTION("After moving diagonally down right (wall is down from kinematic body)") {
		kinematicBody.move({10, 10});
		CollisionBody staticBody({-10, 15, 30, 20}, 0);
		staticCollisionHandler(kinematicBody, staticBody);
		CHECK(Approx(kinematicBody.getPosition().x).margin(0.001f) == 10.f);
		CHECK(Approx(kinematicBody.getPosition().y).margin(0.001f) == 5.f);
	}
	
	SECTION("After moving diagonally up left (wall is up from kinematic body)") {
		kinematicBody.move({-10, -10});
		CollisionBody staticBody({-10, -25, 30, 20}, 0);
		staticCollisionHandler(kinematicBody, staticBody);
		CHECK(Approx(kinematicBody.getPosition().x).margin(0.001f) == -10.f);
		CHECK(Approx(kinematicBody.getPosition().y).margin(0.001f) == -5.f);
	}
	SECTION("After moving diagonally up right (wall is up from kinematic body)") {
		kinematicBody.move({10, -10});
		CollisionBody staticBody({-10, -25, 30, 20}, 0);
		staticCollisionHandler(kinematicBody, staticBody);
		CHECK(Approx(kinematicBody.getPosition().x).margin(0.001f) == 10.f);
		CHECK(Approx(kinematicBody.getPosition().y).margin(0.001f) == -5.f);
	}
}

}
