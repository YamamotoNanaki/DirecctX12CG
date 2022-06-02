#pragma once
#include <xaudio2.h>
#include <fstream>
#include <wrl.h>

#pragma comment(lib,"xaudio2.lib")

namespace IF
{
	namespace soundwave
	{
		struct ChunkHeader
		{
			char id[4];
			int32_t size;
		};

		struct RiffHeader
		{
			ChunkHeader chunk;
			char type[4];
		};

		struct FormatChunk
		{
			ChunkHeader chank;
			WAVEFORMATEX fmt;
		};

		struct SoundData
		{
			WAVEFORMATEX wfex;
			BYTE* pBuffer;
			unsigned int bufferSize;
		};
	}

	class Sound
	{
	private:
		Microsoft::WRL::ComPtr<IXAudio2> xAudio;
		IXAudio2MasteringVoice* masterVoice = nullptr;

	public:
		void Initialize();
		soundwave::SoundData LoadWave(const char* filename);
		void SoundUnLoad(soundwave::SoundData* soundData);
		void SoundPlay(const soundwave::SoundData& soundData);
		~Sound();
	};
}
