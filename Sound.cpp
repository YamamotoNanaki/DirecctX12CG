#include "Sound.h"
#include <cassert>

using namespace IF::soundwave;

void IF::Sound::Initialize()
{
	HRESULT result = XAudio2Create(&xAudio, 0, XAUDIO2_ANY_PROCESSOR);
	assert(SUCCEEDED(result) && "サウンドの初期化に失敗しました");
	result = xAudio->CreateMasteringVoice(&masterVoice);
	assert(SUCCEEDED(result) && "サウンドの初期化に失敗しました");
}

SoundData IF::Sound::LoadWave(const char* filename)
{
	std::ifstream file;

	file.open(filename, std::ios_base::binary);
	assert(file.is_open() && "ファイルが開けません");

	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0)assert(0 && "ファイルがRIFFではありません");
	if (strncmp(riff.type, "WAVE", 4) != 0)assert(0 && "フォーマットがWAVEではありません");

	FormatChunk format;

	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chank.id, "fmt ", 4) != 0)assert(0 && "チャンクの確認に失敗しました");
	assert(format.chank.size <= sizeof(format.fmt) && "40byteは対応してません");
	file.read((char*)&format.fmt, format.chank.size);

	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	if (strncmp(data.id, "JUNK ", 4) == 0)
	{
		file.seekg(data.size, std::ios_base::cur);
		file.read((char*)&data, sizeof(data));
	}
	if (strncmp(data.id, "data ", 4) != 0)assert(0 && "データの確認に失敗しました");

	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	file.close();

	SoundData soundData = {};
	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;

	return soundData;
}

void IF::Sound::SoundUnLoad(SoundData* soundData)
{
	delete[]soundData->pBuffer;

	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

void IF::Sound::SoundPlay(const SoundData& soundData)
{
	HRESULT result;

	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
	assert(SUCCEEDED(result));

	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->Start();
}

IF::Sound::~Sound()
{
	xAudio.Reset();
}
