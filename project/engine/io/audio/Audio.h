#pragma once
#include "xaudio2.h"
#include <fstream>
#include "wrl.h"


#pragma comment(lib,"xaudio2.lib")

class Audio
{

public: // インナークラス
	// チャンクヘッダ
	struct ChunkHeader
	{
		char id[4];		 // チャンク毎のID
		int32_t size;	 // チャンクサイズ
	};
	// RIFFヘッダチャンク
	struct RiffHeader
	{
		ChunkHeader chunk;		// "RIFF"
		char type[4];			// "WAVE"
	};
	// FMTチャンク
	struct FormatChunk
	{
		ChunkHeader chunk;		// "fmt"
		WAVEFORMATEX fmt;			// 波形フォーマット
	};
	// 音声データ
	struct SoundData
	{
		// 波形フォーマット
		WAVEFORMATEX wfex;
		// バッファの先頭アドレス
		BYTE* pBuffer;
		// バッファサイズ
		unsigned int bufferSize;
	};

public: // メンバ関数



	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 音声データ読み込み
	/// </summary>
	/// <param name="filename"></param>
	/// <returns></returns>
	SoundData LoadWave(const char* filename);

	/// <summary>
	/// 音声データの解放
	/// </summary>
	/// <param name="soundData"></param>
	void SoundUnload(Microsoft::WRL::ComPtr<IXAudio2> xAudio2,SoundData* soundData);

	/// <summary>
	/// サウンド再生
	/// </summary>
	/// <param name="xAudio2"></param>
	/// <param name="soundData"></param>
	void SoundPlayWave(IXAudio2* xAudio2,const SoundData& soundData);

	/// <summary>
	/// サウンド停止
	/// </summary>
	void SoundStop(IXAudio2SourceVoice* pSourceVoice);

	/// <summary>
	/// 音量を設定
	/// </summary>
	/// <param name="pSourceVoice">音声ソース</param>
	/// <param name="volume">設定する音量 (0.0f～1.0f)</param>
	void SetVolume(IXAudio2SourceVoice* pSourceVoice, float volume);

public: 
	Microsoft::WRL::ComPtr<IXAudio2> GetXAudio2() const {return xAudio2_;}

private: 

	static Audio* instance;

	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	IXAudio2MasteringVoice* masterVoice_;
	HRESULT hr;;
};

