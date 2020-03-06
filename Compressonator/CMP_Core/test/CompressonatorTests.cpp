#include <map>
#include <array>
#include "../source/CMP_Core.h"
// incudes all compressed 4x4 blocks
#include "BlockConstants.h"
#include "../../../Common/Lib/Ext/Catch2/catch.hpp"
#include "CompressonatorTests.h"

static const int BC1_BLOCK_SIZE = 8;
static const int BC2_BLOCK_SIZE = 16;
static const int BC3_BLOCK_SIZE = 16;
static const int DECOMPRESSED_BLOCK_SIZE = 64;
static const int STRIDE_DECOMPRESSED = 16;

static const std::map<std::string, std::array<unsigned char, 4>> colorValues{
	{ "Red_Ignore_Alpha", { 0xff, 0x0, 0x0, 0xff }},
	{ "Green_Ignore_Alpha" , { 0x0, 0xff, 0x0, 0xff }},
	{ "Blue_Ignore_Alpha" , { 0x0, 0x0, 0xff, 0xff }},
	{ "White_Ignore_Alpha" , { 0xff, 0xff, 0xff, 0xff }},
	{ "Black_Ignore_Alpha" , { 0x0, 0x0, 0x0, 0xff }},
	{ "Red_Blue_Ignore_Alpha" , { 0xff, 0x0, 0xff, 0xff }},
	{ "Red_Green_Ignore_Alpha" , { 0xff, 0xff, 0x0, 0xff }},
	{ "Green_Blue_Ignore_Alpha", { 0x0, 0xff, 0xff, 0xff }},

	{ "Red_Half_Alpha" , { 0xff, 0x0, 0x0, 0x7b }},
	{ "Green_Half_Alpha" , { 0x0, 0xff, 0x0, 0x7b }},
	{ "Blue_Half_Alpha" , { 0x0, 0x0, 0xff, 0x7b }},
	{ "White_Half_Alpha" , { 0xff, 0xff, 0xff, 0x7b }},
	{ "Black_Half_Alpha" , { 0x0, 0x0, 0x0, 0x7b }},
	{ "Red_Blue_Half_Alpha" , { 0xff, 0x0, 0xff, 0x7b }},
	{ "Red_Green_Half_Alpha", { 0xff, 0xff, 0x0, 0x7b }},
	{ "Green_Blue_Half_Alpha" , { 0x0, 0xff, 0xff, 0x7b }},

	{ "Red_Full_Alpha" , { 0xff, 0x0, 0x0, 0x0 }},
	{ "Green_Full_Alpha" , { 0x0, 0xff, 0x0, 0x0 }},
	{ "Blue_Full_Alpha" ,  { 0x0, 0x0, 0xff, 0x0 }},
	{ "White_Full_Alpha" , { 0xff, 0xff, 0xff, 0x0 }},
	{ "Black_Full_Alpha" , { 0x0, 0x0, 0x0, 0x0 }},
	{ "Red_Blue_Full_Alpha" , { 0xff, 0x0, 0xff, 0x0 }},
	{ "Red_Green_Full_Alpha", { 0xff, 0xff, 0x0, 0x0 }},
	{ "Green_Blue_Full_Alpha" , { 0x0, 0xff, 0xff, 0x0 }}
};

//block storage format: [R, G, B, W, Black, RB, RG, GB]. Alpha: 100%, 50%, 0%
enum ColorEnum {
	Red, Green, Blue, White, Black, Red_Blue, Red_Green, Green_Blue
};
enum AlphaEnum {
	Ignore_Alpha, Half_Alpha, Full_Alpha
};
enum CompEnum {
	BC1, BC2, BC3
};

std::string BlockKeyName(CompEnum compression, ColorEnum color, AlphaEnum alpha)
{
	std::string result = "";
	switch (compression) {
		case BC1:	result += "BC1"; break;
		case BC2:	result += "BC2"; break;
		case BC3:	result += "BC3"; break;
	}
	switch (color) {
		case Red:			result += "_Red_"; break;
		case Green:			result += "_Green_"; break;
		case Blue:			result += "_Blue_"; break;
		case White:			result += "_White_"; break;
		case Black:			result += "_Black_"; break;
		case Red_Blue:		result += "_Red_Blue_"; break;
		case Red_Green:		result += "_Red_Green_"; break;
		case Green_Blue:	result += "_Green_Blue_"; break;
	}
	switch (alpha) {
		case Ignore_Alpha:	result += "Ignore_Alpha"; break;
		case Half_Alpha:	result += "Half_Alpha"; break;
		case Full_Alpha:	result += "Full_Alpha"; break;
	}
	return result;
}

void AssignExpectedColorsToBlocks()
{
	ColorEnum color = Red;
	CompEnum comp = BC1;
	AlphaEnum alpha = Ignore_Alpha;
	for (int i = 0; i < blocks.size(); ++i) {
		if (i % 24 == 0 && i > 0) {
			comp = static_cast<CompEnum>(comp + 1);
		}
		if (i % 8 == 0 && i > 0) {
			alpha = static_cast<AlphaEnum>((alpha + 1) % 3);
		}
		const std::string keyBlocks = BlockKeyName(comp, color, alpha);
		std::string keyColor = keyBlocks;
		// string keyColor is in format BCn_color_alpha. To use it as key to access colorValues, delete the BCn_ part.
		keyColor.erase(0, 4);
		((blocks.find(keyBlocks))->second).color = ((colorValues.find(keyColor))->second).data();
		color = static_cast<ColorEnum>((color + 1) % 8);
	}
}

bool ColorMatches(unsigned char* buffer, const unsigned char* expectedColor, bool ignoreAlpha)
{
	unsigned char expectedColorBuffer[64];
	// handle formats that do not support alpha.
	if (ignoreAlpha) {
		// if alpha is ignored, BC should set all values to 0. Except the alpha value which can be 0 or 0xff only.
		if (buffer[3] != 0 && buffer[3] != 255) {
			return false;
		}
		unsigned char expColorWithoutAlpha[4] = { 0 };
		// Only when the alpha value is 0xff colors are stored. Otherwise the RGB colors were set to 0 by during compression.
		if (expectedColor[3] == 0xff) {
			memcpy(expColorWithoutAlpha, expectedColor, 4);
		}
		// Set alpha value to the alpha value in the first pixel of the decompressed buffer.
		// The buffer contains only one color, so all pixels should have the same values.
		expColorWithoutAlpha[3] = buffer[3];
		
		for (int idx = 0; idx < DECOMPRESSED_BLOCK_SIZE / 4; ++idx) {
			memcpy(expectedColorBuffer + (idx * 4), expColorWithoutAlpha, 4);
		}
		return memcmp(&expectedColorBuffer, buffer, DECOMPRESSED_BLOCK_SIZE) == 0;
	}
	
	for (int idx = 0; idx < DECOMPRESSED_BLOCK_SIZE / 4; ++idx) {
		memcpy(expectedColorBuffer + (idx * 4), expectedColor, 4);
	}
	return memcmp(&expectedColorBuffer, buffer, DECOMPRESSED_BLOCK_SIZE) == 0;
}

//***************************************************************************************

TEST_CASE("BC1_Red_Ignore_Alpha", "[BC1_Red_Ignore_Alpha]")
{
	const auto block = blocks.find("BC1_Red_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock [64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,true));
}
TEST_CASE("BC1_Blue_Half_Alpha", "[BC1_Blue_Half_Alpha]")
{
	const auto block = blocks.find("BC1_Blue_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock [64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,true));
}
TEST_CASE("BC1_White_Half_Alpha", "[BC1_White_Half_Alpha]")
{
	const auto block = blocks.find("BC1_White_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock [64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,true));
}
TEST_CASE("BC1_Black_Half_Alpha", "[BC1_Black_Half_Alpha]")
{
	const auto block = blocks.find("BC1_Black_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock [64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,true));
}
TEST_CASE("BC1_Red_Blue_Half_Alpha", "[BC1_Red_Blue_Half_Alpha]")
{
	const auto block = blocks.find("BC1_Red_Blue_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock [64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,true));
}
TEST_CASE("BC1_Red_Green_Half_Alpha", "[BC1_Red_Green_Half_Alpha]")
{
	const auto block = blocks.find("BC1_Red_Green_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock [64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,true));
}
TEST_CASE("BC1_Green_Blue_Half_Alpha", "[BC1_Green_Blue_Half_Alpha]")
{
	const auto block = blocks.find("BC1_Green_Blue_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock [64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,true));
}
TEST_CASE("BC1_Red_Full_Alpha", "[BC1_Red_Full_Alpha]")
{
	const auto block = blocks.find("BC1_Red_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock [64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,true));
}
TEST_CASE("BC1_Green_Full_Alpha", "[BC1_Green_Full_Alpha]")
{
	const auto block = blocks.find("BC1_Green_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock [64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,true));
}
TEST_CASE("BC1_Blue_Full_Alpha", "[BC1_Blue_Full_Alpha]")
{
	const auto block = blocks.find("BC1_Blue_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock [64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,true));
}
TEST_CASE("BC1_White_Full_Alpha", "[BC1_White_Full_Alpha]")
{
	const auto block = blocks.find("BC1_White_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock [64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,true));
}
TEST_CASE("BC1_Green_Ignore_Alpha", "[BC1_Green_Ignore_Alpha]")
{
	const auto block = blocks.find("BC1_Green_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock [64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,true));
}
TEST_CASE("BC1_Black_Full_Alpha", "[BC1_Black_Full_Alpha]")
{
	const auto block = blocks.find("BC1_Black_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock [64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,true));
}
TEST_CASE("BC1_Red_Blue_Full_Alpha", "[BC1_Red_Blue_Full_Alpha]")
{
	const auto block = blocks.find("BC1_Red_Blue_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock [64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,true));
}
TEST_CASE("BC1_Red_Green_Full_Alpha", "[BC1_Red_Green_Full_Alpha]")
{
	const auto block = blocks.find("BC1_Red_Green_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock [64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,true));
}
TEST_CASE("BC1_Green_Blue_Full_Alpha", "[BC1_Green_Blue_Full_Alpha]")
{
	const auto block = blocks.find("BC1_Green_Blue_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock [64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,true));
}
TEST_CASE("BC1_Blue_Ignore_Alpha", "[BC1_Blue_Ignore_Alpha]")
{
	const auto block = blocks.find("BC1_Blue_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock [64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,true));
}
TEST_CASE("BC1_White_Ignore_Alpha", "[BC1_White_Ignore_Alpha]")
{
	const auto block = blocks.find("BC1_White_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock [64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,true));
}
TEST_CASE("BC1_Black_Ignore_Alpha", "[BC1_Black_Ignore_Alpha]")
{
	const auto block = blocks.find("BC1_Black_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock [64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,true));
}
TEST_CASE("BC1_Red_Blue_Ignore_Alpha", "[BC1_Red_Blue_Ignore_Alpha]")
{
	const auto block = blocks.find("BC1_Red_Blue_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock [64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,true));
}
TEST_CASE("BC1_Red_Green_Ignore_Alpha", "[BC1_Red_Green_Ignore_Alpha]")
{
	const auto block = blocks.find("BC1_Red_Green_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock [64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,true));
}
TEST_CASE("BC1_Green_Blue_Ignore_Alpha", "[BC1_Green_Blue_Ignore_Alpha]")
{
	const auto block = blocks.find("BC1_Green_Blue_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock [64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,true));
}
TEST_CASE("BC1_Red_Half_Alpha", "[BC1_Red_Half_Alpha]")
{
	const auto block = blocks.find("BC1_Red_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock [64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,true));
}
TEST_CASE("BC1_Green_Half_Alpha", "[BC1_Green_Half_Alpha]")
{
	const auto block = blocks.find("BC1_Green_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock [64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,true));
}
TEST_CASE("BC2_Red_Ignore_Alpha", "[BC2_Red_Ignore_Alpha]")
{
	const auto block = blocks.find("BC2_Red_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC2_Blue_Half_Alpha", "[BC2_Blue_Half_Alpha]")
{
	const auto block = blocks.find("BC2_Blue_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC2_White_Half_Alpha", "[BC2_White_Half_Alpha]")
{
	const auto block = blocks.find("BC2_White_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC2_Black_Half_Alpha", "[BC2_Black_Half_Alpha]")
{
	const auto block = blocks.find("BC2_Black_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC2_Red_Blue_Half_Alpha", "[BC2_Red_Blue_Half_Alpha]")
{
	const auto block = blocks.find("BC2_Red_Blue_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC2_Red_Green_Half_Alpha", "[BC2_Red_Green_Half_Alpha]")
{
	const auto block = blocks.find("BC2_Red_Green_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC2_Green_Blue_Half_Alpha", "[BC2_Green_Blue_Half_Alpha]")
{
	const auto block = blocks.find("BC2_Green_Blue_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC2_Red_Full_Alpha", "[BC2_Red_Full_Alpha]")
{
	const auto block = blocks.find("BC2_Red_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC2_Green_Full_Alpha", "[BC2_Green_Full_Alpha]")
{
	const auto block = blocks.find("BC2_Green_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC2_Blue_Full_Alpha", "[BC2_Blue_Full_Alpha]")
{
	const auto block = blocks.find("BC2_Blue_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC2_White_Full_Alpha", "[BC2_White_Full_Alpha]")
{
	const auto block = blocks.find("BC2_White_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC2_Green_Ignore_Alpha", "[BC2_Green_Ignore_Alpha]")
{
	const auto block = blocks.find("BC2_Green_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC2_Black_Full_Alpha", "[BC2_Black_Full_Alpha]")
{
	const auto block = blocks.find("BC2_Black_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC2_Red_Blue_Full_Alpha", "[BC2_Red_Blue_Full_Alpha]")
{
	const auto block = blocks.find("BC2_Red_Blue_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC2_Red_Green_Full_Alpha", "[BC2_Red_Green_Full_Alpha]")
{
	const auto block = blocks.find("BC2_Red_Green_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC2_Green_Blue_Full_Alpha", "[BC2_Green_Blue_Full_Alpha]")
{
	const auto block = blocks.find("BC2_Green_Blue_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC2_Blue_Ignore_Alpha", "[BC2_Blue_Ignore_Alpha]")
{
	const auto block = blocks.find("BC2_Blue_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC2_White_Ignore_Alpha", "[BC2_White_Ignore_Alpha]")
{
	const auto block = blocks.find("BC2_White_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC2_Black_Ignore_Alpha", "[BC2_Black_Ignore_Alpha]")
{
	const auto block = blocks.find("BC2_Black_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC2_Red_Blue_Ignore_Alpha", "[BC2_Red_Blue_Ignore_Alpha]")
{
	const auto block = blocks.find("BC2_Red_Blue_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC2_Red_Green_Ignore_Alpha", "[BC2_Red_Green_Ignore_Alpha]")
{
	const auto block = blocks.find("BC2_Red_Green_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC2_Green_Blue_Ignore_Alpha", "[BC2_Green_Blue_Ignore_Alpha]")
{
	const auto block = blocks.find("BC2_Green_Blue_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC2_Red_Half_Alpha", "[BC2_Red_Half_Alpha]")
{
	const auto block = blocks.find("BC2_Red_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC2_Green_Half_Alpha", "[BC2_Green_Half_Alpha]")
{
	const auto block = blocks.find("BC2_Green_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC3_Red_Ignore_Alpha", "[BC3_Red_Ignore_Alpha]")
{
	const auto block = blocks.find("BC3_Red_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC3_Blue_Half_Alpha", "[BC3_Blue_Half_Alpha]")
{
	const auto block = blocks.find("BC3_Blue_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC3_White_Half_Alpha", "[BC3_White_Half_Alpha]")
{
	const auto block = blocks.find("BC3_White_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC3_Black_Half_Alpha", "[BC3_Black_Half_Alpha]")
{
	const auto block = blocks.find("BC3_Black_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC3_Red_Blue_Half_Alpha", "[BC3_Red_Blue_Half_Alpha]")
{
	const auto block = blocks.find("BC3_Red_Blue_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC3_Red_Green_Half_Alpha", "[BC3_Red_Green_Half_Alpha]")
{
	const auto block = blocks.find("BC3_Red_Green_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC3_Green_Blue_Half_Alpha", "[BC3_Green_Blue_Half_Alpha]")
{
	const auto block = blocks.find("BC3_Green_Blue_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC3_Red_Full_Alpha", "[BC3_Red_Full_Alpha]")
{
	const auto block = blocks.find("BC3_Red_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC3_Green_Full_Alpha", "[BC3_Green_Full_Alpha]")
{
	const auto block = blocks.find("BC3_Green_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC3_Blue_Full_Alpha", "[BC3_Blue_Full_Alpha]")
{
	const auto block = blocks.find("BC3_Blue_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC3_White_Full_Alpha", "[BC3_White_Full_Alpha]")
{
	const auto block = blocks.find("BC3_White_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC3_Green_Ignore_Alpha", "[BC3_Green_Ignore_Alpha]")
{
	const auto block = blocks.find("BC3_Green_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC3_Black_Full_Alpha", "[BC3_Black_Full_Alpha]")
{
	const auto block = blocks.find("BC3_Black_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC3_Red_Blue_Full_Alpha", "[BC3_Red_Blue_Full_Alpha]")
{
	const auto block = blocks.find("BC3_Red_Blue_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC3_Red_Green_Full_Alpha", "[BC3_Red_Green_Full_Alpha]")
{
	const auto block = blocks.find("BC3_Red_Green_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC3_Green_Blue_Full_Alpha", "[BC3_Green_Blue_Full_Alpha]")
{
	const auto block = blocks.find("BC3_Green_Blue_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC3_Blue_Ignore_Alpha", "[BC3_Blue_Ignore_Alpha]")
{
	const auto block = blocks.find("BC3_Blue_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC3_White_Ignore_Alpha", "[BC3_White_Ignore_Alpha]")
{
	const auto block = blocks.find("BC3_White_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC3_Black_Ignore_Alpha", "[BC3_Black_Ignore_Alpha]")
{
	const auto block = blocks.find("BC3_Black_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC3_Red_Blue_Ignore_Alpha", "[BC3_Red_Blue_Ignore_Alpha]")
{
	const auto block = blocks.find("BC3_Red_Blue_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC3_Red_Green_Ignore_Alpha", "[BC3_Red_Green_Ignore_Alpha]")
{
	const auto block = blocks.find("BC3_Red_Green_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC3_Green_Blue_Ignore_Alpha", "[BC3_Green_Blue_Ignore_Alpha]")
{
	const auto block = blocks.find("BC3_Green_Blue_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC3_Red_Half_Alpha", "[BC3_Red_Half_Alpha]")
{
	const auto block = blocks.find("BC3_Red_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}
TEST_CASE("BC3_Green_Half_Alpha", "[BC3_Green_Half_Alpha]")
{
	const auto block = blocks.find("BC3_Green_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock [64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor,false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock [64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor,false));
}

//***************************************************************************************