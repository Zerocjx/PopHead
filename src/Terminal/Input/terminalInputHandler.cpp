#include "TerminalInputHandler.hpp"

#include "Base/gameData.hpp"

ph::TerminalInputHandler::TerminalInputHandler(TerminalSharedData terminalSharedData)
	:mTerminalSharedData(terminalSharedData)
	,mContent(mTerminalSharedData->mContent)
{
}

void ph::TerminalInputHandler::handleInput()
{
	if(mTerminalSharedData->mIsVisible){
		handleKeyboardCharactersInput();
		handleBackspace();
		handleEnter();
	}

	showOrHideCommandPromptInput();

	mTerminalSharedData->mText.setString(mTerminalSharedData->mContent);
}

void ph::TerminalInputHandler::handleKeyboardCharactersInput()
{
	auto& keyboard = mGameData->getInput().getKeyboard();

	if (keyboard.isKeyJustPressed(sf::Keyboard::A)) mContent += "a";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::B)) mContent += "b";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::C)) mContent += "c";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::D)) mContent += "d";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::E)) mContent += "e";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::F)) mContent += "f";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::G)) mContent += "g";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::H)) mContent += "h";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::I)) mContent += "i";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::J)) mContent += "j";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::K)) mContent += "k";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::L)) mContent += "l";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::M)) mContent += "m";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::N)) mContent += "n";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::O)) mContent += "o";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::P)) mContent += "p";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::Q)) mContent += "q";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::R)) mContent += "r";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::S)) mContent += "s";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::T)) mContent += "t";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::U)) mContent += "u";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::V)) mContent += "v";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::W)) mContent += "w";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::X)) mContent += "x";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::Y)) mContent += "y";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::Z)) mContent += "z";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::Num1)) mContent += "1";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::Num2)) mContent += "2";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::Num3)) mContent += "3";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::Num4)) mContent += "4";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::Num5)) mContent += "5";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::Num6)) mContent += "6";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::Num7)) mContent += "7";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::Num8)) mContent += "8";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::Num9)) mContent += "9";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::Num0)) mContent += "0";
	else if (keyboard.isKeyJustPressed(sf::Keyboard::Space)) mContent += " ";

	else if (keyboard.isKeyJustPressed(sf::Keyboard::Up)) mContent += mTerminalSharedData->mLastCommand;
	else if (keyboard.isKeyJustPressed(sf::Keyboard::Down))	mContent.clear(); 
}

void ph::TerminalInputHandler::handleBackspace()
{
	auto& keyboard = mGameData->getInput().getKeyboard();
	if(keyboard.isKeyJustPressed(sf::Keyboard::BackSpace)) {
		if(mContent.size() > 0)
			mContent.pop_back();
	}
}

void ph::TerminalInputHandler::handleEnter()
{
	mIsEnterClicked = false;
	auto& keyboard = mGameData->getInput().getKeyboard();
	if(keyboard.isKeyJustPressed(sf::Keyboard::Enter))
		mIsEnterClicked = true;
}

void ph::TerminalInputHandler::showOrHideCommandPromptInput()
{
	auto& keyboard = mGameData->getInput().getKeyboard();
	if(keyboard.isKeyJustPressed(sf::Keyboard::Tab)) {
		bool& isVisible = mTerminalSharedData->mIsVisible;
		auto& actionManager = mGameData->getInput().getAction();
		actionManager.setEnabled(isVisible);
		isVisible = !isVisible;
		setKeyRepeatEnabled(isVisible);
	}
}

void ph::TerminalInputHandler::setKeyRepeatEnabled(bool enabled)
{
	auto& window = mGameData->getRenderer().getWindow();
	window.setKeyRepeatEnabled(enabled);
}