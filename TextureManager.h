#pragma once

// テクスチャマネージャー
class TextureManager
{
public:






private:
	static TextureManager* instance;

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = delete;
	TextureManager& operator = (TextureManager&) = delete;



};

