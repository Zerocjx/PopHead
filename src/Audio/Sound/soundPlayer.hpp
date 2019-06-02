#ifndef POPHEAD_AUDIO_SOUNDPLAYER_H_
#define POPHEAD_AUDIO_SOUNDPLAYER_H_

#include "Resources/resourceHolder.hpp"
#include "Audio/Sound/SoundData/soundDataHolder.hpp"
#include <SFML/Audio.hpp>
#include <list>

namespace PopHead {
namespace Audio {

class SoundPlayer
{
public:
	SoundPlayer();

	void playAmbientSound(const std::string& filePath);
	void playSpatialSound(const std::string& filePath, sf::Vector2f soundPosition);
	void setListenerPosition(const sf::Vector2f& listenerPosition) { mListenerPosition = listenerPosition; }
	void setVolume(float volume);
	void removeEverySound();

private:
	void loadEverySound();
	void removeStoppedSounds();

private:
	Resources::SoundBufferHolder mSoundBuffers;
	SoundDataHolder mSoundDataHolder;
	std::list<sf::Sound> mSounds;
	sf::Vector2f mListenerPosition;
	float mVolume;
};

}}

#endif // !POPHEAD_AUDIO_SOUNDPLAYER_H_
