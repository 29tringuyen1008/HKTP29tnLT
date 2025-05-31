#include "Audio.h"

constexpr auto fourccRIFF = 'FFIR';
constexpr auto fourccDATA = 'atad';
constexpr auto fourccFMT = ' tmf';
constexpr auto fourccWAVE = 'EVAW';
constexpr auto fourccXWMA = 'AMWX';
constexpr auto fourccDPDS = 'sdpd';

///////////////////////////////////////////////////////////////////////////////////////////

HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition)
{
	HRESULT hr = S_OK;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());

	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD bytesRead = 0;
	DWORD dwOffset = 0;

	while (hr == S_OK)
	{
		DWORD dwRead;
		if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL)) {
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL)) {
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		switch (dwChunkType)
		{
		case fourccRIFF:
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL)) {
				hr = HRESULT_FROM_WIN32(GetLastError());
			}
			break;

		default:
			if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT)) {
				return HRESULT_FROM_WIN32(GetLastError());
			}
		}

		dwOffset += sizeof(DWORD) * 2;

		if (dwChunkType == fourcc)
		{
			dwChunkSize = dwChunkDataSize;
			dwChunkDataPosition = dwOffset;
			return S_OK;
		}

		dwOffset += dwChunkDataSize;

		if (bytesRead >= dwRIFFDataSize) return S_FALSE;
	}

	return S_OK;
}

HRESULT ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset)
{
	HRESULT hr = S_OK;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());
	DWORD dwRead;
	if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
		hr = HRESULT_FROM_WIN32(GetLastError());
	return hr;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////

tn::SoundFramework::SoundFramework()
{
	
}

int tn::SoundFramework::Initialize()
{
	if (this->APIType == AudioAPIType::API_DirectSound)
	{
		this->CurrentAPIType = AudioAPIType::API_DirectSound;
		HRESULT hr = DirectSoundCreate8(nullptr, &DirectSoundStructure, nullptr);
		if (FAILED(hr)) {
			return TN_AUDIO_ERR_INIT;
		}

		if (this->Window != nullptr)
		{
			DirectSoundStructure->SetCooperativeLevel(this->Window->GetHWND(), DSSCL_PRIORITY);
		}
		else
		{
			DirectSoundStructure->SetCooperativeLevel(GetConsoleWindow(), DSSCL_PRIORITY);
		}
	}
	else if (this->APIType == AudioAPIType::API_XAudio2)
	{
		this->CurrentAPIType = AudioAPIType::API_XAudio2;
		XAudio2Create(&m_XAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
		m_XAudio2->CreateMasteringVoice(&m_MasterVoice);
		m_MasterVoice->SetVolume(1.0f);
	}

	this->GlobalVolume = 1.0f;
	Initialized = true;
}

void tn::SoundFramework::Release()
{
	if (this->CurrentAPIType == AudioAPIType::API_DirectSound)
	{
		DirectSoundStructure->Release();
		DirectSoundStructure = nullptr;
	}
	else if (this->CurrentAPIType == AudioAPIType::API_XAudio2)
	{
		m_MasterVoice->DestroyVoice();
		m_XAudio2->Release();
		m_XAudio2 = nullptr;
	}

	delete this;
}

void tn::SoundFramework::SetGlobalVolume(float Volume)
{
	if (Volume > 1.0f) Volume = 1.0f;
	else if (Volume < 0.0f) Volume = 0.0f;
	this->GlobalVolume = Volume;
	for (size_t i = 0; i < this->SoundsInFramework.GetSize(); i++)
	{
		if (Sound* sound = this->SoundsInFramework[i])
		{
			sound->SetVolume(sound->GetLocalVolume());
		}
	}
}

float tn::SoundFramework::GetGlobalVolume()
{
	return this->GlobalVolume;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

tn::SoundBuffer::SoundBuffer(SoundFramework* Framework)
{
	this->CurrentFramework = Framework;
}

ULONG tn::SoundBuffer::AddRef()
{
	return this->Refs += 1;
}

void tn::SoundBuffer::Release()
{
	if (this->Refs <= 0)
	{
		tn::SafeReleaseWinSys(&DirectSoundBuffer);
		delete this;
	}
	else this->Refs -= 1;
}

void tn::SoundBuffer::SetDesc(AudioDescription desc)
{
	this->CurrentDescription = desc;
}

void tn::SoundBuffer::GetDesc(AudioDescription* output)
{
	(*output) = this->CurrentDescription;
}

tn::AudioDescription tn::SoundBuffer::GetDesc()
{
	return this->CurrentDescription;
}

float tn::SoundBuffer::GetLength()
{
	return this->SoundLength;
}

unsigned long tn::SoundBuffer::GetSize()
{
	return this->FileSize;
}

unsigned long tn::SoundBuffer::GetChunkSize()
{
	return this->ChunkSize;
}

BYTE*& tn::SoundBuffer::GetData()
{
	return this->audioData;
}

///

int tn::SoundBuffer::Initialize()
{
	hFile = CreateFileW(this->CurrentDescription.Filename.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
	if (hFile == INVALID_HANDLE_VALUE) {
		//std::cerr << "Failed to open file!" << std::endl;
		return TN_AUDIO_ERR_FILENOTFOUND;
	}
	this->FileSize = GetFileSize(hFile, nullptr);

	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN)) {
		return TN_AUDIO_ERR_FILEEMPTY;
	}

	switch (this->CurrentFramework->CurrentAPIType)
	{
	case AudioAPIType::API_DirectSound:
		return this->InitializeDirectSound();
	case AudioAPIType::API_XAudio2:
		return this->InitializeXAudio2();
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////

int tn::SoundBuffer::InitializeDirectSound()
{
	DWORD chunkSize = 0;
	DWORD chunkDataPosition = 0;

	if (FAILED(FindChunk(hFile, fourccFMT, chunkSize, chunkDataPosition))) {
		std::cerr << "Failed to locate 'fmt ' chunk!" << std::endl;
		CloseHandle(hFile);
		return TN_AUDIO_ERR_WAVE_FORMAT;
	}

	if (FAILED(ReadChunkData(hFile, &this->waveFormat, sizeof(WAVEFORMATEX), chunkDataPosition))) {
		std::cerr << "Failed to read 'fmt ' chunk!" << std::endl;
		CloseHandle(hFile);
		return TN_AUDIO_ERR_WAVE_FORMAT;
	}

	if (FAILED(FindChunk(hFile, fourccDATA, chunkSize, chunkDataPosition))) {
		std::cerr << "Failed to locate 'data' chunk!" << std::endl;
		CloseHandle(hFile);
		return TN_AUDIO_ERR_WAVE_FORMAT;
	}
	audioData = new BYTE[chunkSize];
	if (!audioData) {
		CloseHandle(hFile);
		return TN_AUDIO_ERR_MEMORY;
	}
	if (FAILED(ReadChunkData(hFile, this->audioData, chunkSize, chunkDataPosition)))
	{
		delete[] audioData;
		CloseHandle(hFile);
		return TN_AUDIO_ERR_MEMORY;
	}

	this->ChunkSize = chunkSize;

	WAVEFORMATEX wf = {};
	wf.wFormatTag = WAVE_FORMAT_PCM;
	wf.nChannels = this->waveFormat.nChannels;
	wf.nSamplesPerSec = this->waveFormat.nSamplesPerSec;
	wf.wBitsPerSample = this->waveFormat.wBitsPerSample;
	wf.nBlockAlign = this->waveFormat.nBlockAlign; //(wf.wBitsPerSample / 8) * wf.nChannels;
	wf.nAvgBytesPerSec = this->waveFormat.nAvgBytesPerSec; //wf.nSamplesPerSec * wf.nBlockAlign;
	this->waveFormat = wf;

	this->SoundLength = static_cast<float>(this->ChunkSize) / static_cast<float>(this->waveFormat.nAvgBytesPerSec);
	//std::cout << this->SoundLength << " seconds" << std::endl;

	DSBUFFERDESC dsbd = {};
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;
	//dsbd.dwBufferBytes = this->ChunkSize;
	//dsbd.lpwfxFormat = &waveFormat;

	/*
	HRESULT hr = 0;
	hr = this->CurrentFramework->DirectSoundStructure->CreateSoundBuffer(&dsbd, &DirectSoundBuffer, nullptr);
	if (FAILED(hr)) {
		return TN_AUDIO_ERR_INIT;
	}

	hr = DirectSoundBuffer->SetFormat(&wf);
	if (FAILED(hr)) {
		return TN_AUDIO_ERR_INIT;
	}*/

	CloseHandle(hFile);
	return 0;
}

int tn::SoundBuffer::InitializeXAudio2()
{
	//XAudio2Create(&m_XAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	//m_XAudio2->CreateMasteringVoice(&m_MasterVoice);
	//m_MasterVoice->SetVolume(1.0f);

	DWORD dwChunkSize;
	DWORD dwChunkPosition;
	//check the file type, should be fourccWAVE or 'XWMA'

	FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);

	DWORD filetype;
	ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
	if (filetype != fourccWAVE) {
		return -1;
	}

	if (FAILED(FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition))) return -1;
	if (FAILED(ReadChunkData(hFile, &this->waveFormat, dwChunkSize, dwChunkPosition))) return -1;
	if (FAILED(FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition))) return -1;

	BYTE* pDataBuffer = new BYTE[dwChunkSize];
	if (FAILED(ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition))) return -1;

	XAudio2Buffer.AudioBytes = dwChunkSize;
	XAudio2Buffer.pAudioData = pDataBuffer;
	XAudio2Buffer.Flags = XAUDIO2_END_OF_STREAM;
	if (this->CurrentDescription.EnableLoop == true) XAudio2Buffer.LoopCount = XAUDIO2_LOOP_INFINITE;

	this->ChunkSize = dwChunkSize;
	this->SoundLength = static_cast<float>(this->ChunkSize) / static_cast<float>(this->waveFormat.nAvgBytesPerSec);

	CloseHandle(hFile);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////

tn::Sound::Sound()
{
	this->CurrentBuffer = nullptr;
	DirectSoundSecondaryBuffer = nullptr;
	LocalVolume = 1.0f;
}

int tn::Sound::SetBuffer(SoundBuffer* Buffer)
{
	if (Buffer == nullptr) return 1;

	this->CurrentBuffer = Buffer;
	if (this->CurrentFramework != nullptr)
	{
		this->CurrentFramework->SoundsInFramework.Remove(this);
	}
	this->CurrentFramework = Buffer->CurrentFramework;
	this->CurrentFramework->SoundsInFramework.Add(this);

	if (this->CurrentBuffer->CurrentDescription.APIType == AudioAPIType::API_DirectSound)
	{
		/*WAVEFORMATEX wf = {};
		wf.wFormatTag = WAVE_FORMAT_PCM;
		wf.nChannels = this->CurrentBuffer->waveFormat.nChannels;
		wf.nSamplesPerSec = this->CurrentBuffer->waveFormat.nSamplesPerSec;
		wf.wBitsPerSample = this->CurrentBuffer->waveFormat.wBitsPerSample;
		wf.nBlockAlign = this->CurrentBuffer->waveFormat.nBlockAlign;
		wf.nAvgBytesPerSec = this->CurrentBuffer->waveFormat.nAvgBytesPerSec;*/

		DSBUFFERDESC dsbd2 = {};
		dsbd2.dwSize = sizeof(DSBUFFERDESC);
		dsbd2.dwFlags = DSBCAPS_CTRLVOLUME | ((this->CurrentFramework->Window == nullptr or this->CurrentBuffer->CurrentDescription.Windowed == false) ? DSBCAPS_GLOBALFOCUS : 0);
		dsbd2.dwBufferBytes = this->CurrentBuffer->ChunkSize;
		dsbd2.lpwfxFormat = &this->CurrentBuffer->waveFormat;

		tn::SafeReleaseWinSys(&this->DirectSoundSecondaryBuffer);
		HRESULT hr = this->CurrentFramework->DirectSoundStructure->CreateSoundBuffer(&dsbd2, &DirectSoundSecondaryBuffer, nullptr);
		if (FAILED(hr)) {
			return -1;
		}

		if (SUCCEEDED(DirectSoundSecondaryBuffer->Lock(0, this->CurrentBuffer->ChunkSize, &pAudioPtr1, &audioBytes1, &pAudioPtr2, &audioBytes2, 0))) {
			memcpy_s(pAudioPtr1, audioBytes1, this->CurrentBuffer->audioData, audioBytes1);
			if (pAudioPtr2) {
				memcpy_s(pAudioPtr2, audioBytes2, this->CurrentBuffer->audioData + audioBytes1, audioBytes2);
			}
			DirectSoundSecondaryBuffer->Unlock(pAudioPtr1, audioBytes1, pAudioPtr2, audioBytes2);
		}
	}

	return 0;
}

void tn::Sound::Play()
{
	if (this->CurrentBuffer != nullptr)
	{
		if (this->CurrentBuffer->CurrentDescription.APIType == AudioAPIType::API_DirectSound)
		{
			if (this->DirectSoundSecondaryBuffer != nullptr)
			{
				this->DirectSoundSecondaryBuffer->Stop();
				this->DirectSoundSecondaryBuffer->SetCurrentPosition(0);
				this->DirectSoundSecondaryBuffer->Restore();
				this->SetVolume(this->LocalVolume);
				if (this->CurrentBuffer->CurrentDescription.EnableLoop == true)
				{
					this->DirectSoundSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
				}
				else
				{
					this->DirectSoundSecondaryBuffer->Play(0, 0, 0);
				}
			}
		}
		else if (this->CurrentBuffer->CurrentDescription.APIType == AudioAPIType::API_XAudio2)
		{
			if (this->CurrentFramework->m_XAudio2 != nullptr)
			{
				if (this->Paused == false)
				{
					if (this->IsCurrentlyPlaying() == true)
					{
						this->pSourceVoice->Stop();
						this->pSourceVoice->DestroyVoice();
						this->pSourceVoice = nullptr;
					}

					if (this->pSourceVoice == nullptr) {
						this->CurrentFramework->m_XAudio2->CreateSourceVoice(&this->pSourceVoice,
							(WAVEFORMATEX*)&this->CurrentBuffer->waveFormat, 0, 2.0f, nullptr);
					}

					if (this->pSourceVoice != nullptr) {
						this->SetVolume(this->LocalVolume);
						this->pSourceVoice->SubmitSourceBuffer(&this->CurrentBuffer->XAudio2Buffer);
						this->pSourceVoice->Start(0);
					}
				}
				else
				{
					if (this->pSourceVoice) {
						this->pSourceVoice->Start(0);
					}
				}
			}
		}
	}

	this->Paused = false;
}

void tn::Sound::Pause()
{
	this->Paused = true;

	if (this->CurrentBuffer != nullptr)
	{
		if (this->CurrentBuffer->CurrentDescription.APIType == AudioAPIType::API_DirectSound)
		{
			if (this->DirectSoundSecondaryBuffer != nullptr)
			{
				this->DirectSoundSecondaryBuffer->GetCurrentPosition(&this->DirectSoundCurrentPos, nullptr);
				this->DirectSoundSecondaryBuffer->Stop();
			}
		}
		else if (this->CurrentBuffer->CurrentDescription.APIType == AudioAPIType::API_XAudio2)
		{
			if (this->pSourceVoice) {
				this->pSourceVoice->Stop(0);
			}
		}
	}
}

void tn::Sound::Stop()
{
	if (this->CurrentBuffer != nullptr)
	{
		if (this->CurrentBuffer->CurrentDescription.APIType == AudioAPIType::API_DirectSound)
		{
			if (this->DirectSoundSecondaryBuffer != nullptr)
			{
				this->DirectSoundSecondaryBuffer->Stop();
				this->DirectSoundCurrentPos = 0;
				this->DirectSoundSecondaryBuffer->SetCurrentPosition(0);
			}
		}
		else if (this->CurrentBuffer->CurrentDescription.APIType == AudioAPIType::API_XAudio2)
		{
			if (this->pSourceVoice) {
				this->pSourceVoice->Stop();
				this->pSourceVoice->ExitLoop();
				this->pSourceVoice->DestroyVoice();
			}
		}

		CloseHandle(this->ThreadHandle);
	}

	this->Paused = false;
}

void tn::Sound::SetVolume(float Volume)
{
	this->LocalVolume = Volume;
	if (this->CurrentBuffer != nullptr)
	{
		if (this->CurrentBuffer->CurrentDescription.APIType == AudioAPIType::API_DirectSound)
		{
			if (this->DirectSoundSecondaryBuffer != nullptr)
			{
				float vol = tn::math::clamp(Volume, 0.0001f, 1.0f);

				LONG value = static_cast<LONG>(20.0f * std::log10f(vol * this->CurrentFramework->GlobalVolume));

				if (value < DSBVOLUME_MIN) value = DSBVOLUME_MIN;
				if (value > 0) value = 0;

				this->DirectSoundSecondaryBuffer->SetVolume(value);
			}
		}
		else if (this->CurrentBuffer->CurrentDescription.APIType == AudioAPIType::API_XAudio2)
		{
			if (this->pSourceVoice) {
				this->pSourceVoice->SetVolume(Volume * this->CurrentFramework->GlobalVolume);
			}
		}
	}
}

void tn::Sound::GetOverallVolume(float* Output)
{
	if (this->CurrentBuffer != nullptr)
	{
		if (this->CurrentBuffer->CurrentDescription.APIType == AudioAPIType::API_DirectSound)
		{
			if (this->DirectSoundSecondaryBuffer != nullptr)
			{
				LONG rawvol = 0;
				this->DirectSoundSecondaryBuffer->GetVolume(&rawvol);
				(*Output) = rawvol;
			}
		}
		else if (this->CurrentBuffer->CurrentDescription.APIType == AudioAPIType::API_XAudio2)
		{
			if (this->pSourceVoice) {
				this->pSourceVoice->GetVolume(Output);
			}
		}
	}
}

float tn::Sound::GetOverallVolume()
{
	LONG rawvol = 0;
	if (this->CurrentBuffer != nullptr)
	{
		if (this->CurrentBuffer->CurrentDescription.APIType == AudioAPIType::API_DirectSound)
		{
			if (this->DirectSoundSecondaryBuffer != nullptr)
			{
				this->DirectSoundSecondaryBuffer->GetVolume(&rawvol);
			}
			return static_cast<float>(rawvol);
		}
		else if (this->CurrentBuffer->CurrentDescription.APIType == AudioAPIType::API_XAudio2)
		{
			float output = 0.0f;
			if (this->pSourceVoice) {
				this->pSourceVoice->GetVolume(&output);
			}
			return output;
		}
	}
	return static_cast<float>(rawvol);
}

float tn::Sound::GetLocalVolume()
{
	return this->LocalVolume;
}

float tn::Sound::GetPosition()
{
	float timeInSeconds = 0.0f;
	if (this->CurrentBuffer != nullptr)
	{
		if (this->CurrentBuffer->CurrentDescription.APIType == AudioAPIType::API_DirectSound)
		{
			if (this->DirectSoundSecondaryBuffer != nullptr)
			{
				DWORD dwPlayCursor, dwWriteCursor;
				this->DirectSoundSecondaryBuffer->GetCurrentPosition(&dwPlayCursor, &dwWriteCursor);
				timeInSeconds = ((float)dwPlayCursor) / this->CurrentBuffer->waveFormat.nAvgBytesPerSec;
			}
		}
		else if (this->CurrentBuffer->CurrentDescription.APIType == AudioAPIType::API_XAudio2)
		{
			XAUDIO2_VOICE_STATE state;

			if (this->pSourceVoice) {
				this->pSourceVoice->GetState(&state);
				timeInSeconds = (float)state.SamplesPlayed / this->CurrentBuffer->waveFormat.nSamplesPerSec;
			}
		}
	}

	return timeInSeconds;
}

void tn::Sound::SetPosition(float Seconds)
{
	if (this->CurrentBuffer != nullptr)
	{
		if (this->CurrentBuffer->CurrentDescription.APIType == AudioAPIType::API_DirectSound)
		{
			if (this->DirectSoundSecondaryBuffer != nullptr)
			{
				DWORD byteOffset = static_cast<DWORD>(Seconds * this->CurrentBuffer->waveFormat.nAvgBytesPerSec);
				byteOffset -= byteOffset % this->CurrentBuffer->waveFormat.nBlockAlign;
				if (byteOffset > this->CurrentBuffer->ChunkSize)
					byteOffset = this->CurrentBuffer->ChunkSize - this->CurrentBuffer->waveFormat.nBlockAlign;

				this->DirectSoundSecondaryBuffer->SetCurrentPosition(byteOffset);
			}
		}
		else if (this->CurrentBuffer->CurrentDescription.APIType == AudioAPIType::API_XAudio2)
		{
			float output = 0.0f;
			if (this->pSourceVoice) {
				this->pSourceVoice->Stop(0);
				this->pSourceVoice->FlushSourceBuffers();

				DWORD byteOffset = static_cast<DWORD>(Seconds * this->CurrentBuffer->waveFormat.nAvgBytesPerSec);
				byteOffset -= byteOffset % this->CurrentBuffer->waveFormat.nBlockAlign;

				if (byteOffset > this->CurrentBuffer->ChunkSize)
					byteOffset = this->CurrentBuffer->ChunkSize - this->CurrentBuffer->waveFormat.nBlockAlign;

				XAUDIO2_BUFFER buffer = {};
				buffer.AudioBytes = this->CurrentBuffer->ChunkSize - byteOffset;
				buffer.pAudioData = this->CurrentBuffer->audioData + byteOffset;
				buffer.Flags = XAUDIO2_END_OF_STREAM;

				this->pSourceVoice->SubmitSourceBuffer(&buffer);
				this->pSourceVoice->Start(0);
			}
		}
	}
}

bool tn::Sound::IsCurrentlyPlaying()
{
	bool result = false;

	if (this->Paused == false)
	{
		if (this->CurrentBuffer != nullptr)
		{
			if (this->CurrentBuffer->CurrentDescription.APIType == AudioAPIType::API_DirectSound)
			{
				if (this->DirectSoundSecondaryBuffer != nullptr)
				{
					DWORD status = 0;
					this->DirectSoundSecondaryBuffer->GetStatus(&status);
					if (status & DSBSTATUS_PLAYING)
					{
						result = true;
					}
				}
			}
			else if (this->CurrentBuffer->CurrentDescription.APIType == AudioAPIType::API_XAudio2)
			{
				if (this->pSourceVoice) {
					XAUDIO2_VOICE_STATE state;
					pSourceVoice->GetState(&state);

					if (state.BuffersQueued > 0) {
						result = true;
					}
				}
			}
		}
	}

	return result;
}

bool tn::Sound::IsCurrentlyPaused() const
{
	return this->Paused;
}

void tn::Sound::Release()
{
	if (this->ThreadHandle != nullptr)
	{
		CloseHandle(this->ThreadHandle);
	}

	if (this->CurrentBuffer != nullptr)
	{
		if (this->CurrentBuffer->CurrentDescription.APIType == AudioAPIType::API_DirectSound)
		{
			tn::SafeReleaseWinSys(&this->DirectSoundSecondaryBuffer);
		}
		else if (this->CurrentBuffer->CurrentDescription.APIType == AudioAPIType::API_XAudio2)
		{
			if (this->pSourceVoice) {
				this->pSourceVoice->DestroyVoice();
			}
		}
	}
}