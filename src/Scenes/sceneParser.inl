#include "sceneParser.hpp"
#include "Utilities/xml.hpp"
#include "Logs/logs.hpp"
#include "gameData.hpp"

namespace ph {

template<typename GuiParser, typename MapParser, typename ResourcesParser, typename AudioParser, typename EnttParser>
SceneParser<GuiParser, MapParser, ResourcesParser, AudioParser, typename EnttParser>
	::SceneParser(GameData* const gameData, CutSceneManager& cutSceneManager, EntitiesTemplateStorage& templateStorage,
                  entt::registry& gameRegistry, const std::string& sceneFileName, TextureHolder& textureHolder)
{
	PH_LOG_INFO("Scene linking file (" + sceneFileName + ") is being parsed.");

	Xml sceneFile;
	sceneFile.loadFromFile(sceneFileName);
	const auto sceneLinksNode = sceneFile.getChild("scenelinks");

	//parse<ResourcesParser>(gameData, sceneLinksNode, "neededResources");
	//parse<MapParser>(gameData, sceneLinksNode, "map");
	//parse<GuiParser>(gameData, sceneLinksNode, "gui");	
	parse<AudioParser>(gameData, sceneLinksNode, "audio");
	//parseGameObjects(gameData, root, cutSceneManager, sceneLinksNode);
	parseEcsEntities(sceneLinksNode, templateStorage, gameRegistry, textureHolder);
}

template<typename GuiParser, typename MapParser/*, typename GameObjectsParser*/, typename ResourcesParser, typename AudioParser, typename EnttParser>
template<typename Parser>
void SceneParser<GuiParser, MapParser/*, GameObjectsParser*/, ResourcesParser, AudioParser, typename EnttParser>
	::parse(GameData* const gameData, const Xml& sceneLinksNode, const std::string& categoryName)
{
	const auto categoryNode = sceneLinksNode.getChildren(categoryName);
	if (categoryNode.size() == 1)
	{
		const std::string categoryFilePath = "scenes/" + categoryName + "/" + categoryNode[0].getAttribute("filename").toString();
		Parser categoryParser;
		categoryParser.parseFile(gameData, categoryFilePath);
	}
}


template<typename GuiParser, typename MapParser, typename ResourcesParser, typename AudioParser, typename EnttParser>
void SceneParser<GuiParser, MapParser, ResourcesParser, AudioParser, EnttParser>
::parseEcsEntities(const Xml& sceneLinksNode, EntitiesTemplateStorage& templateStorage, entt::registry& gameRegistry,
                   TextureHolder& textureHolder)
{
	const auto entitiesNode = sceneLinksNode.getChildren("ecsObjects");
	if (entitiesNode.size() == 1)
	{
		const std::string entitiesFilePath = "scenes/ecs/" + entitiesNode[0].getAttribute("filename").toString();
		EnttParser parser;
		parser.parseFile(entitiesFilePath, templateStorage, gameRegistry, textureHolder);
	}
}

//template<typename GuiParser, typename MapParser/*, typename GameObjectsParser*/, typename ResourcesParser, typename AudioParser, typename EnttParser>
//void SceneParser<GuiParser, MapParser/*, GameObjectsParser*/, ResourcesParser, AudioParser, typename EnttParser>
//	::parseGameObjects(GameData* const gameData/*, GameObject& root*/, CutSceneManager& cutSceneManager, const Xml& sceneLinksNode)
//{
//	const auto gameObjectsNode = sceneLinksNode.getChildren("map");
//	if (gameObjectsNode.size() == 1)
//	{
//		//const std::string gameObjectsFileName = "scenes/map/" + gameObjectsNode[0].getAttribute("filename").toString();
//		//GameObjectsParser gameObjectsParser(gameData, root, cutSceneManager);
//		//gameObjectsParser.parseFile(gameObjectsFileName);
//	}
//}
//
}
