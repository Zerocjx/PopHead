#include "terminalRenderer.hpp"
#include "Renderer/renderer.hpp"

namespace ph {

TerminalRenderer::TerminalRenderer(TerminalSharedData terminalSharedData)
	:mTerminalSharedData(terminalSharedData)
{
}

void TerminalRenderer::pushOutputLine(const OutputLine& line)
{
	if(mOutputLines.size() >= 12)
		mOutputLines.pop_back();
	mOutputLines.emplace_front(line);
}

void TerminalRenderer::update() const
{
	if(mTerminalSharedData->isVisible) {
		Renderer::submitQuad(nullptr, nullptr, &sf::Color(0, 0, 0, 230), nullptr, {0.f, 660.f}, {1920.f, 420.f}, 5, 0.f, {}, ProjectionType::gui);
		Renderer::submitQuad(nullptr, nullptr, &sf::Color::Black, nullptr, {0.f, 725.f}, {1920.f, 10.f}, 4, 0.f, {}, ProjectionType::gui);
		Renderer::submitText(mTerminalSharedData->content.c_str(), "consola.ttf", {5.f, 660.f}, 50.f, sf::Color::White, 0, ProjectionType::gui);

		float posY = 723.f;
		for(int i = 0; i < mOutputLines.size(); ++i, posY += 14.f)
			Renderer::submitText(mOutputLines[i].mText.c_str(), "consola.ttf", {5.f, posY}, 20.f, mOutputLines[i].mColor, 0, ProjectionType::gui);
	}
}

}

