#include "TerminalInputHandler.hpp"
#include "Events/actionEventManager.hpp"
#include "ECS/System.hpp"

namespace ph {

TerminalInputHandler::TerminalInputHandler(TerminalSharedData terminalSharedData, sf::Window& window)
	:mSharedData(terminalSharedData)
	,mWindow(window)
	,mIsEnterClicked(false)
	,mIndexOfCurrentLastCommand(-1)
{
}

void TerminalInputHandler::handleEvent(Event& phEvent)
{
	sf::Event* e = std::get_if<sf::Event>(&phEvent);
	if(!e)
		return;

	if(mSharedData->isVisible && e->type == sf::Event::TextEntered)
	{
		char key = static_cast<char>(e->text.unicode);
		if (!iscntrl(key))
			mSharedData->content += key;
	}

	if (e->type == sf::Event::KeyPressed)
	{
		switch (e->key.code)
		{
		case sf::Keyboard::Tab: 
			if(e->key.control)
				showOrHideCommandPrompt();
			break;

		case sf::Keyboard::BackSpace:
			if (mSharedData->content.size() > 0)
				mSharedData->content.pop_back();
			break;

		case sf::Keyboard::Enter:
			mIsEnterClicked = true;
			updateLastCommands();
			break;

		case sf::Keyboard::Up:
			if (mIndexOfCurrentLastCommand + 1 < static_cast<int>(mSharedData->lastCommands.size()))
			{
				++mIndexOfCurrentLastCommand;
				if (mIndexOfCurrentLastCommand >= 0)
					mSharedData->content = mSharedData->lastCommands[mIndexOfCurrentLastCommand];
			}
			break;
		case sf::Keyboard::Down:
			if (mIndexOfCurrentLastCommand > -1)
			{
				--mIndexOfCurrentLastCommand;
				if (mIndexOfCurrentLastCommand == -1)
					mSharedData->content.clear();
				else
					mSharedData->content = mSharedData->lastCommands[mIndexOfCurrentLastCommand];
			}
		}
	}
}

void TerminalInputHandler::update()
{
	mIsEnterClicked = false;
}

void TerminalInputHandler::updateLastCommands()
{
	mIndexOfCurrentLastCommand = -1;
	auto& content = mSharedData->content;
	auto& lastCommands = mSharedData->lastCommands;
	if(content.size() != 0) {
		lastCommands.emplace_front(content);
		if(lastCommands.size() > 10)
			lastCommands.pop_back();
	}
}

void TerminalInputHandler::showOrHideCommandPrompt()
{
	bool& isVisible = mSharedData->isVisible;
	ActionEventManager::setEnabled(isVisible);
	isVisible = !isVisible;
	mWindow.setKeyRepeatEnabled(isVisible);
	system::System::setPause(isVisible);
}

}
