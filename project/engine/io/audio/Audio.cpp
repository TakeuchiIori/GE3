#include "Audio.h"
#include "assert.h"

void Audio::Initialize()
{
	// xAudioエンジンのインスタンスを生成
	hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	// マスターボイスを生成
	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);


}

Audio::SoundData Audio::LoadWave(const char* filename)
{
	
	// ファイル入力ストリームのインスタンス
	std::ifstream file;
	// .wavファイルをバイナリモードで開く
	file.open(filename, std::ios_base::binary);
	// ファイルオープン失敗を検出
	assert(file.is_open());


	// RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	// ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	// タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}

	// チャンクのループを開始
	ChunkHeader chunkHeader;
	FormatChunk format = {};

	while (file.read((char*)&chunkHeader, sizeof(chunkHeader))) {
		// チャンクIDが "fmt " かを確認
		if (strncmp(chunkHeader.id, "fmt ", 4) == 0) {
			// Formatチャンクのサイズを確認し、データを読み込む
			assert(chunkHeader.size <= sizeof(format.fmt));
			format.chunk = chunkHeader; // チャンクヘッダーをコピー
			file.read((char*)&format.fmt, chunkHeader.size); // fmtのデータを読み込み
			break;
		}
		else {
			// 次のチャンクに移動
			file.seekg(chunkHeader.size, std::ios_base::cur);
		}
	}

	// "fmt "チャンクが見つからなかった場合のエラー処理
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		//std::cerr << "Error: 'fmt ' chunk not found!" << std::endl;
		assert(0);
	}



	// Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	// JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK", 4) == 0) {
		// 読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}
	
	// Dataチャンクのデータ部（波形データの読み込み）
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	// Waveファイルを閉じる
	file.close();


	// returnする為のデータ
	SoundData soundData = {};

	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;
	
	return soundData;
}

void Audio::SoundUnload(Microsoft::WRL::ComPtr<IXAudio2> xAudio2,SoundData* soundData)
{
	xAudio2.Reset();

	// バッファのメモリを解放
	delete[] soundData->pBuffer;

	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

// 音声再生
void Audio::SoundPlayWave(IXAudio2* xAudio2, const Audio::SoundData& soundData) {
	HRESULT hr;

	// 波形フォーマットを元にSourceVoiceの生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	hr = xAudio2->CreateSourceVoice(&pSourceVoice, reinterpret_cast<const WAVEFORMATEX*>(&soundData.wfex));
	assert(SUCCEEDED(hr));

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	// 波形データの再生
	hr = pSourceVoice->SubmitSourceBuffer(&buf);
	hr = pSourceVoice->Start();
}

// サウンド停止
void Audio::SoundStop(IXAudio2SourceVoice* pSourceVoice)
{
	// サウンド停止
	pSourceVoice->Stop();
	// バッファをフラッシュして再生をリセット
	pSourceVoice->FlushSourceBuffers();
}

/// 音量を設定
void Audio::SetVolume(IXAudio2SourceVoice* pSourceVoice, float volume)
{
	// 0.0f ～ 1.0f の範囲で音量を設定
	HRESULT hr = pSourceVoice->SetVolume(volume);
	assert(SUCCEEDED(hr) && "Failed to set volume");
}