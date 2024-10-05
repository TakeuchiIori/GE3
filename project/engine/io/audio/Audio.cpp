#include "Audio.h"

void Audio::Initialize()
{
	// xAudioエンジンのインスタンスを生成
	HRESULT hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	// マスターボイスを生成
	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);


}
