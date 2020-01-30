#pragma once 

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Transform.hpp>
#include "Utilities/rect.hpp"

namespace ph { 

class Camera
{
public:
	Camera();
	Camera(FloatRect& rect);
	Camera(sf::Vector2f center, sf::Vector2f size);

	void setCenter(sf::Vector2f center);
	void setCenterSmoothly(sf::Vector2f center, float speed);
	void move(sf::Vector2f offset);
	sf::Vector2f getCenter() { return mCenter; }

	void setRotation(float rotation);
	void rotate(float rotation);
	float getRotation() { return mRotation; }

	void setSize(sf::Vector2f size);
	void zoom(float factor);
	sf::Vector2f getSize() { return mSize; }

	const sf::Transform& getViewProjectionMatrix4x4();

	FloatRect getBounds();

private:
	sf::Transform mViewProjectionMatrix;
	sf::Vector2f mCenter;
	sf::Vector2f mSize;
	float mRotation;
	bool mViewProjectionMatrixNeedsUpdate;
};

}
