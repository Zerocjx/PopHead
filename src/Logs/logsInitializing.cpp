#include "logsInitializing.hpp"

#include "ConcreteHandlers/consoleHandler.hpp"
#include "ConcreteHandlers/fileHandler.hpp"
#include "ConcreteHandlers/terminalHandler.hpp"

#include "logger.hpp"

#include "Utilities/xml.hpp"

namespace ph {

	void initializeLogsModule(const std::string& configFileName, Terminal* terminal)
	{
		Xml document;
		document.loadFromFile(configFileName);

		auto mainTag = document.getChild("handlers");
		auto handlers = mainTag.getChildren("handler");

		for (const auto& handlerTag : handlers)
		{
			std::string type = handlerTag.getAttribute("type").toString();

			std::unique_ptr<Handler> handler;
			if (type == "fileHandler")
				handler.reset(new FileHandler("logs\\log"));
			else if (type == "consoleHandler")
				handler.reset(new ConsoleHandler());
			else if (type == "terminalHandler")
				handler.reset(new TerminalHandler(terminal));

			if (!handler)
				continue;

			auto filesTag = handlerTag.getChild("filepaths");

			if (filesTag.hasAttribute("all"))
				handler->enableAllPaths();
			else
			{
				auto enabledPaths = filesTag.getChildren("enable");

				for (const auto& pathTag : enabledPaths)
				{
					std::string path = pathTag.toString();
					handler->setPathFilter(path, true);
				}

				auto disabledPaths = filesTag.getChildren("disable");

				for (const auto& pathTag : disabledPaths)
				{
					std::string path = pathTag.toString();
					handler->setPathFilter(path, false);
				}
			}

			auto logLevelsTag = handlerTag.getChild("levels");

			if (logLevelsTag.hasAttribute("all"))
				handler->enableAllLogLevels();
			else
			{
				auto logLevels = logLevelsTag.getChildren("level");

				for (const auto& logLevel : logLevels)
				{
					std::string levelStr = logLevel.toString();

					if (levelStr == "Info")
						handler->setLogLevelFilter(LogLevel::Info, true);
					if (levelStr == "Error")
						handler->setLogLevelFilter(LogLevel::Error, true);
					if (levelStr == "Warning")
						handler->setLogLevelFilter(LogLevel::Warning, true);
					if (levelStr == "Critical")
						handler->setLogLevelFilter(LogLevel::Critical, true);
				}
			}

			Logger::addLogsHandler(std::move(handler));
		}
	}
}
