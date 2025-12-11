#include "pch.h"
#include "audio/AudioEngine.h"

#include <format>
#include <fstream>

#include "Locator.h"
#include "rendering/utils/Util.h"

using namespace std;

AudioEngine::AudioEngine(const std::vector<std::string>& filePaths)
{
    DXEssayer(CoInitializeEx(nullptr, COINIT_MULTITHREADED), "Error while creating a sound");
    DXEssayer(XAudio2Create(&xaudio, 0, XAUDIO2_USE_DEFAULT_PROCESSOR), "Error while creating a sound");
    DXEssayer(xaudio->CreateMasteringVoice(&masterVoice), "Error while creating a sound");

    for (const auto& filePath : filePaths)
		sounds.emplace(filePath, Sound{ filePath });
}

AudioEngine::~AudioEngine()
{
    if (masterVoice) 
        masterVoice->DestroyVoice();
    if (xaudio) 
        xaudio->Release();

    CoUninitialize();
}

Sound::Sound(const std::string& filePath)
{
    Load(filePath);
}

void Sound::Play() const
{
    const auto& engine = Locator::Get<AudioEngine>();

    IXAudio2SourceVoice* voice = nullptr;
    DXEssayer(engine.GetEngine()->CreateSourceVoice(&voice, &wfx), "Error while playing a sound");

    DXEssayer(voice->SubmitSourceBuffer(&buffer), "Error while playing a sound");
    DXEssayer(voice->Start(0), "Error while playing a sound");
}

void Sound::Load(const std::string& filePath)
{
	ifstream soundFile{ filePath, std::ios::binary };
    if (not soundFile)
        throw runtime_error{format("Could not load sound file: {}", filePath)};

    char chunkId[4];
    uint32_t chunkSize;

    // RIFF header
    soundFile.read(chunkId, 4);
    soundFile.read(reinterpret_cast<char*>(&chunkSize), 4);
    soundFile.read(chunkId, 4);

    // fmt chunk
    soundFile.read(chunkId, 4);
    soundFile.read(reinterpret_cast<char*>(&chunkSize), 4);
    soundFile.read(reinterpret_cast<char*>(&wfx), chunkSize);

    // data chunk
    soundFile.read(chunkId, 4);
    soundFile.read(reinterpret_cast<char*>(&chunkSize), 4);

    audioData.resize(chunkSize);
    soundFile.read(reinterpret_cast<char*>(audioData.data()), chunkSize);

    buffer.AudioBytes = chunkSize;
    buffer.pAudioData = audioData.data();
    buffer.Flags = XAUDIO2_END_OF_STREAM;
}
