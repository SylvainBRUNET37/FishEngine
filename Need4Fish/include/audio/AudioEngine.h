#ifndef AUDIO_ENGINE_H
#define AUDIO_ENGINE_H

#include <windows.h>
#include <xaudio2.h>
#include <vector>
#include <string>
#include <unordered_map>

class Sound
{
public:
	explicit Sound(const std::string& filePath, bool isLooped = false);

	void Play() const;

private:
	void Load(const std::string& filePath, bool isLooped = false);

	WAVEFORMATEX wfx{};
	XAUDIO2_BUFFER buffer{};

	std::vector<BYTE> audioData;
};

class AudioEngine
{
public:
	// File path + isLooped boolean
	explicit AudioEngine(const std::vector<std::pair<std::string, bool>>& filePaths);
	~AudioEngine();

	AudioEngine(const AudioEngine&) = delete;
	AudioEngine& operator=(const AudioEngine&) = delete;

	AudioEngine(AudioEngine&& other) = delete;
	AudioEngine& operator=(const AudioEngine&&) = delete;

	[[nodiscard]] IXAudio2* GetEngine() const { return xaudio; }
	[[nodiscard]] IXAudio2MasteringVoice* GetMasterVoice() const { return masterVoice; }

	void PlaySoundEffect(const std::string& filePath)
	{
		const auto& soundIt = sounds.find(filePath);

		if (soundIt != sounds.end())
			soundIt->second.Play();
	}

private:
	std::unordered_map<std::string, Sound> sounds;

	IXAudio2* xaudio{};
	IXAudio2MasteringVoice* masterVoice{};
};

#endif
