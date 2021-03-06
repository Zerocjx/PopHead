#pragma once

#include "scene.hpp"
#include "Events/event.hpp"
#include "ECS/entitiesTemplateStorage.hpp"
#include <SFML/System.hpp>
#include <memory>

namespace ph {

class GameData;
class Texture;

class SceneManager
{
public:
    SceneManager();

    void replaceScene(const std::string& sceneSourceCodeFilePath);
    void replaceScene(const std::string& sceneSourceCodeFilePath, const sf::Vector2f& playerPosition);
    void popScene();
    
	void changingScenesProcess();

	bool hasPlayerPositionForNextScene() const;
	const sf::Vector2f& getPlayerPositionForNextScene() const;

private:
	void replaceAction();
	void popAction();

public:
	void handleEvent(const Event& event);
    void update(sf::Time dt);

	Scene& getScene() { return *mScene.get(); }
	void setGameData(GameData* const);

	std::string getCurrentMapName() const { return mCurrentSceneFile; }

private:
	EntitiesTemplateStorage mEntitiesTemplateStorage;
    std::unique_ptr<Scene> mScene;
	PlayerStatus mLastPlayerStatus;
	std::string mFileOfSceneToMake;
	std::string mCurrentSceneFile;
    GameData* mGameData;
	Texture* mTilesetTexture;
	sf::Vector2f mPlayerPositionForNextScene;
    bool mIsReplacing;
    bool mIsPopping;
	bool mHasPlayerPositionForNextScene;
};

}