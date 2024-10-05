#pragma once
#include "xaudio2.h"
#include <fstream>
#include "wrl.h"


#pragma comment(lib,"xaudio.lib")
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
		WAVEFORMAT fmt;			// 波形フォーマット
	};

public: // メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();




private: // メンバ変数
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	IXAudio2MasteringVoice* masterVoice_;
};

