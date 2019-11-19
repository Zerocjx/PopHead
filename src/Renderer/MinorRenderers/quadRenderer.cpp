#include "quadRenderer.hpp"
#include "Renderer/texture.hpp"
#include "Renderer/Shaders/shaderLibary.hpp"
#include "Renderer/openglErrors.hpp"
#include "Utilities/cast.hpp"
#include <GL/glew.h>
#include <algorithm>

namespace ph {

void QuadRenderer::init()
{
	auto& sl = ShaderLibrary::getInstance();
	sl.loadFromFile("instancedSprite", "resources/shaders/instancedSprite.vs.glsl", "resources/shaders/instancedSprite.fs.glsl");
	mDefaultInstanedSpriteShader = sl.get("instancedSprite");

	unsigned quadIndices[] = {0, 1, 3, 1, 2, 3};
	mQuadIBO.init();
	mQuadIBO.setData(quadIndices, sizeof(quadIndices));

	GLCheck( glGenVertexArrays(1, &mInstancedVAO) );
	GLCheck( glBindVertexArray(mInstancedVAO) );

	mQuadIBO.bind();

	GLCheck( glGenBuffers(1, &mInstancedQuadsDataVBO) );
	GLCheck( glBindBuffer(GL_ARRAY_BUFFER, mInstancedQuadsDataVBO) );

	GLCheck( glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(QuadData), (void*) offsetof(QuadData, position)) );
	GLCheck( glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QuadData), (void*) offsetof(QuadData, size)) );
	GLCheck( glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(QuadData), (void*) offsetof(QuadData, rotation)) );
	GLCheck( glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(QuadData), (void*) offsetof(QuadData, color)) );
	GLCheck( glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(QuadData), (void*) offsetof(QuadData, textureRect)) );
	GLCheck( glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(QuadData), (void*) offsetof(QuadData, textureSlotRef)) );

	for(int i = 0; i < 6; ++i)
		glEnableVertexAttribArray(i);

	for(int i = 0; i < 6; ++i)
		glVertexAttribDivisor(i, 1);

	mWhiteTexture = new Texture;
	unsigned whiteData = 0xffffffff;
	mWhiteTexture->setData(&whiteData, sizeof(unsigned), sf::Vector2i(1, 1));

	// allocate instanced vectors
	mInstancedQuadsData.reserve(3000);
	mInstancedTextures.reserve(32);

	mDefaultInstanedSpriteShader->bind();
	int textures[32];
	for(int i = 0; i < 32; ++i)
		textures[i] = i;
	mDefaultInstanedSpriteShader->setUniformIntArray("textures", 32, textures);
}

void QuadRenderer::shutDown()
{
	delete mWhiteTexture;
	mQuadIBO.remove();
	GLCheck( glDeleteBuffers(1, &mInstancedQuadsDataVBO) );
	GLCheck( glDeleteVertexArrays(1, &mInstancedVAO) );
}

void QuadRenderer::setDebugNumbersToZero()
{
	mNumberOfDrawCalls = 0;
	mNumberOfDrawnSprites = 0;
	mNumberOfDrawnTextures = 0;
}

// TODO_ren: Support custom shaders for instanced rendering

void QuadRenderer::submitQuad(const Texture* texture, const IntRect* textureRect, const sf::Color* color,
                              sf::Vector2f position, sf::Vector2f size, float rotation)
{
	// culling
	if(!isInsideScreen(position, size))
		return;

	// submit data
	QuadData quadData;

	quadData.position = position;
	quadData.size = size;
	quadData.rotation = rotation;
	quadData.color = color ? Cast::toNormalizedColorVector4f(*color) : Cast::toNormalizedColorVector4f(sf::Color::White);
	quadData.textureRect = textureRect ? getNormalizedTextureRect(textureRect, size) : FloatRect(0.f, 0.f, 1.f, 1.f);
	
	if(!texture)
		texture = mWhiteTexture;
	auto textureSlotOfThisTexture = getTextureSlotToWhichThisTextureIsBound(texture);
	if(textureSlotOfThisTexture)
		quadData.textureSlotRef = *textureSlotOfThisTexture;
	else {
		const float textureSlotID = static_cast<float>(mInstancedTextures.size());
		quadData.textureSlotRef = textureSlotID;
		mInstancedTextures.emplace_back(texture);
	}

	mInstancedQuadsData.emplace_back(quadData);
}

bool QuadRenderer::isInsideScreen(sf::Vector2f position, sf::Vector2f size)
{
	return mScreenBounds->doPositiveRectsIntersect(sf::FloatRect(position.x, position.y, size.x, size.y));
}

auto QuadRenderer::getTextureSlotToWhichThisTextureIsBound(const Texture* texture) -> std::optional<int>
{
	for(size_t i = 0; i < mInstancedTextures.size(); ++i)
		if(mInstancedTextures[i] == texture)
			return i;
	return std::nullopt;
}

auto QuadRenderer::getNormalizedTextureRect(const IntRect* pixelTextureRect, sf::Vector2f size) -> FloatRect
{
	return FloatRect(
		pixelTextureRect->left / size.x, pixelTextureRect->top / size.y,
		pixelTextureRect->width / size.x, pixelTextureRect->height / size.y
	);
}

void QuadRenderer::flush()
{
	mNumberOfDrawnSprites += mInstancedQuadsData.size();
	mNumberOfDrawnTextures += mInstancedTextures.size();

	mDefaultInstanedSpriteShader->bind();
	mDefaultInstanedSpriteShader->setUniformMatrix4x4("viewProjectionMatrix", mViewProjectionMatrix);

	std::sort(mInstancedQuadsData.begin(), mInstancedQuadsData.end(), [](const QuadData& a, const QuadData& b) {
		return a.textureSlotRef < b.textureSlotRef;
	});

	while(areThereTextureSlotRefsGreaterThen31())
		subtract32FromAllTextureSlotRefsGreaterThen31();

	for(int i = 0; i < (mInstancedTextures.size() > 32 ? 32 : mInstancedTextures.size()); ++i)
		mInstancedTextures[i]->bind(i);

	for(size_t i = 0; i < mInstancedQuadsData.size(); ++i)
	{
		if(i == mInstancedQuadsData.size() - 1)
		{
			drawCall(i + 1);

			mInstancedQuadsData.clear();
			mInstancedTextures.clear();
			break;
		}
		else if (mInstancedQuadsData[i].textureSlotRef == 0 && mInstancedQuadsData[i == 0 ? i : i - 1].textureSlotRef == 31) 
		{
			drawCall(i + 1);

			mInstancedQuadsData.erase(mInstancedQuadsData.begin(), mInstancedQuadsData.begin() + i);
			
			mInstancedTextures.erase(
				mInstancedTextures.begin(),
				mInstancedTextures.size() > 32 ? mInstancedTextures.begin() + 32 : mInstancedTextures.end()
			);

			i = 0;
		}
	}
}

bool QuadRenderer::areThereTextureSlotRefsGreaterThen31()
{
	for(QuadData& quadData : mInstancedQuadsData)
		if(quadData.textureSlotRef > 31)
			return true;
	return false;
}

void QuadRenderer::subtract32FromAllTextureSlotRefsGreaterThen31()
{
	for(QuadData& quadData : mInstancedQuadsData)
		if(quadData.textureSlotRef > 31)
			quadData.textureSlotRef -= 32;
}

void QuadRenderer::drawCall(unsigned nrOfInstances)
{
	GLCheck( glBindBuffer(GL_ARRAY_BUFFER, mInstancedQuadsDataVBO) );
	GLCheck( glBufferData(GL_ARRAY_BUFFER, nrOfInstances * sizeof(QuadData), mInstancedQuadsData.data(), GL_STATIC_DRAW) );

	GLCheck( glBindVertexArray(mInstancedVAO) );
	GLCheck( glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, nrOfInstances) );

	++mNumberOfDrawCalls;
}

}
