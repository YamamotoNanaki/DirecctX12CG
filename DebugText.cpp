#include "DebugText.h"

void IF::DebugText::Initialize(unsigned int texNum)
{
	for (int i = 0; i < _countof(sprites); i++)
	{
		sprites[i].Initialize(texNum);
	}
}

void IF::DebugText::Print(float x, float y, float scale, const std::string& text, ...)
{
	posX = x;
	posY = y;
	size = scale;

	for (int i = 0; i < text.size(); i++)
	{
		if (spriteIndex >= maxCharCount)break;

		const unsigned char& character = text[i];

		int fontIndex = character - 32;
		if (character >= 0x7f)fontIndex = 0;

		int fontIndexY = (fontIndex - 1) / fontLineCount;
		int fontIndexX = (fontIndex - 1) % fontLineCount;
		sprites[spriteIndex].SetPosition({ posX + fontWidth * size * i,posY });
		sprites[spriteIndex].SetTextureRect({ (float)fontIndex * fontWidth,(float)fontIndexY * fontHeight },
			{ (float)fontWidth,(float)fontHeight });
		sprites[spriteIndex].SetSize({ fontWidth * size,fontHeight * size });
		spriteIndex++;
	}
}

void IF::DebugText::Draw(std::vector<D3D12_VIEWPORT>viewport)
{
	// 全ての文字のスプライトについて
	for (int i = 0; i < spriteIndex; i++)
	{
		sprites[i].Update();
		// スプライト描画
		sprites[i].Draw(viewport);
	}

	spriteIndex = 0;
}
