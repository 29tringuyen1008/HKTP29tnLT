#pragma once
#include "Object.h"
#include "AudioEnum.h"
#include "Array.h"
#include "../Core/Windows.h"
#include "../Core/Misc.h"

#ifdef _WIN32 || _WIN64
#include <Windows.h>
#include <tchar.h>
#include <xaudio2.h>
#include <dsound.h>

#pragma comment(lib, "xaudio2.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "winmm.lib")
#endif

#define TN_AUDIO_ERR_FILENOTFOUND -1
#define TN_AUDIO_ERR_FILEEMPTY -2
#define TN_AUDIO_ERR_WAVE_FORMAT -3
#define TN_AUDIO_ERR_MEMORY -4
#define TN_AUDIO_ERR_INIT -5

enum class AudioAPIType
{
	API_DirectSound = 1,
	API_XAudio2 = 2,
};

namespace tn {
	class Sound;
	class SoundBuffer;

	class SoundFramework
	{
	public:
		SoundFramework();
		AudioAPIType APIType = AudioAPIType::API_XAudio2;
		//Chỉ dành cho DirectSound
		tn::Window* Window = nullptr;

		int Initialize();
		void Release();

		void SetGlobalVolume(float Volume);
		float GetGlobalVolume();
	private:
		friend class SoundBuffer;
		friend class Sound;
		float GlobalVolume = 0.0f;

		bool Initialized = false;
		AudioAPIType CurrentAPIType = AudioAPIType::API_XAudio2;

		IDirectSound8* DirectSoundStructure = nullptr;
		IXAudio2* m_XAudio2 = nullptr;
		IXAudio2MasteringVoice* m_MasterVoice = nullptr;
		tn::ReferenceArray<Sound> SoundsInFramework;
	};

	struct AudioDescription {
		std::wstring Filename;
		bool EnableLoop = false;
		//tn::Window* Window;
		//Chỉ dành cho DirectSound
		bool Windowed = false;
		AudioAPIType APIType = AudioAPIType::API_XAudio2;
	};

	class SoundBuffer : public Object
	{
	public:
		SoundBuffer(SoundFramework* Framework);

		void SetDesc(AudioDescription desc);
		void GetDesc(AudioDescription* output);
		AudioDescription GetDesc();

		float GetLength();
		unsigned long GetSize();
		unsigned long GetChunkSize();
		BYTE*& GetData();

		int Initialize();

		ULONG AddRef();
		void Release() override;
	private:
		friend class SoundFramework;
		friend class Sound;

		int InitializeDirectSound();
		int InitializeXAudio2();

		SoundFramework* CurrentFramework = nullptr;
		AudioDescription CurrentDescription = {};

		HANDLE hFile = nullptr;
		WAVEFORMATEX waveFormat = {};
		DWORD FileSize = 0; DWORD ChunkSize = 0; float SoundLength = 0;
		BYTE* audioData = nullptr;

		//IDirectSound8* DirectSoundStructure = nullptr;
		IDirectSoundBuffer* DirectSoundBuffer = nullptr;

		//IXAudio2* m_XAudio2 = nullptr;
		//IXAudio2MasteringVoice* m_MasterVoice = nullptr;
		XAUDIO2_BUFFER XAudio2Buffer = { 0 };

		ULONG Refs = 0;
	};

	class Sound : public Object
	{
	public:
		Sound();

		int SetBuffer(SoundBuffer* Buffer);

		void Play();
		void Pause();
		void Stop();

		void SetVolume(float Volume);
		void GetOverallVolume(float* Output);
		float GetOverallVolume();
		float GetLocalVolume();

		float GetPosition();
		void SetPosition(float Seconds);

		bool IsCurrentlyPlaying();
		bool IsCurrentlyPaused() const;

		void Release() override;
	private:
		HANDLE ThreadHandle = nullptr;
		float LocalVolume = 1.0f;

		bool Paused = false;
		DWORD DirectSoundCurrentPos = 0;

		void* pAudioPtr1 = nullptr;
		DWORD audioBytes1 = 0;
		void* pAudioPtr2 = nullptr;
		DWORD audioBytes2 = 0;

		IDirectSoundBuffer* DirectSoundSecondaryBuffer = nullptr;
		IXAudio2SourceVoice* pSourceVoice = nullptr;

		SoundFramework* CurrentFramework = nullptr;
		SoundBuffer* CurrentBuffer = nullptr;
	};
}