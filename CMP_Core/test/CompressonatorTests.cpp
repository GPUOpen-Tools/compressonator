#include <map>
#include <array>
#include "../../../Common/Lib/Ext/Catch2/catch.hpp"
#include "../source/CMP_Core.h"
#include "../../Applications/_Plugins/Common/UtilFuncs.h"
// incudes all compressed 4x4 blocks
#include "BlockConstants.h"
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

const std::map<std::string, std::array<float, 3>> colorValuesBC6{
	{ "Red_Ignore_Alpha", { 1.0f, 0.0f, 0.0f}},
	{ "Green_Ignore_Alpha" , { 0.0f, 01.0f, 0.0f}},
	{ "Blue_Ignore_Alpha" , { 0.0f, 0.0f, 1.0f}},
	{ "White_Ignore_Alpha" , { 1.0f, 1.0f, 1.0f}},
	{ "Black_Ignore_Alpha" , { 0.0f, 0.0f, 0.0f}},
	{ "Red_Blue_Ignore_Alpha" , { 1.0f, 0.0f, 1.0f}},
	{ "Red_Green_Ignore_Alpha" , { 1.0f, 1.0f, 0.0f}},
	{ "Green_Blue_Ignore_Alpha", { 0.0f, 1.0f, 1.0f }},
};

//block storage format: [R, G, B, W, Black, RB, RG, GB]. Alpha: 100%, 50%, 0%
enum ColorEnum {
	Red, Green, Blue, White, Black, Red_Blue, Red_Green, Green_Blue
};
enum AlphaEnum {
	Ignore_Alpha, Half_Alpha, Full_Alpha
};
enum CompEnum {
	BC1, BC2, BC3, BC4, BC5, BC7, BC6
};

std::string BlockKeyName(CompEnum compression, ColorEnum color, AlphaEnum alpha) {
	std::string result = "";
	switch (compression) {
	case BC1:	result += "BC1"; break;
	case BC2:	result += "BC2"; break;
	case BC3:	result += "BC3"; break;
	case BC4:	result += "BC4"; break;
	case BC5:	result += "BC5"; break;
	case BC6:	result += "BC6"; break;
	case BC7:	result += "BC7"; break;
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

void AssignExpectedColorsToBlocks() {
	ColorEnum color = Red;
	CompEnum comp = BC1;
	AlphaEnum alpha = Ignore_Alpha;
	for (int i = 0; i < blocks.size(); ++i) {
		if (i % 24 == 0 && i > 0) {
			comp = static_cast<CompEnum>(comp + 1);
		}
		if (comp == CompEnum::BC6)		//Bc6 blocks are stored in its own blocks map.
			continue;

		if (i % 8 == 0 && i > 0) {
			alpha = static_cast<AlphaEnum>((alpha + 1) % 3);
		}
		const std::string keyBlocks = BlockKeyName(comp, color, alpha);
		std::string keyColor = keyBlocks;
		keyColor.erase(0, 4);
		auto it = (blocks.find(keyBlocks));
		it->second.color = ((colorValues.find(keyColor))->second).data();
		color = static_cast<ColorEnum>((color + 1) % 8);
	}
	// BC6 list
	comp = CompEnum::BC6;
	for (int i = 0; i < blocksBC6.size(); ++i){
		if (i % 8 == 0 && i > 0) {
			alpha = static_cast<AlphaEnum>((alpha + 1) % 3);
		}
		const std::string keyBlocks = BlockKeyName(comp, color, alpha);
		std::string keyColor = BlockKeyName(comp, color, AlphaEnum::Ignore_Alpha);
		// string keyColor is in format BCn_color_alpha. To use it as key to access colorValues, delete the BCn_ part.
		keyColor.erase(0, 4);
		((blocksBC6.find(keyBlocks))->second).color = ((colorValuesBC6.find(keyColor))->second).data();
		color = static_cast<ColorEnum>((color + 1) % 8);
	}
}

bool ColorMatches(unsigned char* buffer, const unsigned char* expectedColor, bool ignoreAlpha)
{
	unsigned char expectedColorBuffer[64];
	// handle formats that do not support alpha.
	if (ignoreAlpha) {
		// if alpha is ignored, BC should set all values to 0. exept the alpha value which can be 0 or 0xff only.
		// Since all blocks have the same color, there should always be the same alpha.
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


bool ColorMatchesBC4(unsigned char* buffer, const unsigned char* expectedColor) {
	unsigned char expectedColorBuffer[16];
	for (int i = 0; i < 16; ++i) {
		expectedColorBuffer[i] = expectedColor[0];		//Bc4 supports red channel only.
	}
	return memcmp(&expectedColorBuffer, buffer, sizeof(expectedColorBuffer)) == 0;
}

bool ColorMatchesBC5(unsigned char* bufferR, unsigned char* bufferG, const unsigned char* expectedColor) {
	unsigned char expectedColorR[16];
	unsigned char expectedColorG[16];
	for (int i = 0; i < 16; ++i) {
		expectedColorR[i] = expectedColor[0];			//Bc5 supports red channel and green channel only.
		expectedColorG[i] = expectedColor[1];
	}
	return memcmp(&expectedColorR, bufferR, 16) == 0 && memcmp(&expectedColorG, bufferG, 16) == 0;
}

bool ColorMatchesBC6(unsigned short* buffer, const float* expectedColor)
{
	float bufferInFloat[48];
	float expectedColorBuffer[48];
	for (int i = 0; i < 16; ++i) {
		// SF16: 1:5:10	: 1bit signed, 5bit exponent, 10bit mantissa
		// DecompressBC6 stores decompressed color as SF16
		// BC6 stores RGB channels only
		for (int channel = 0; channel < 3; ++channel) {
			// convert expcolor float to half-float with intrinsic
			//__m128 val = _mm_load_ps1(&expColor);
			//__m128i half = _mm_cvtps_ph(val, 0);
			//unsigned short expColorSh = _mm_extract_epi32(half, 0);
			unsigned short color = buffer[i * 3 + channel];
			bufferInFloat[i * 3 + channel] = HalfToFloat(color);

			expectedColorBuffer[i * 3 + channel] = expectedColor[channel];
		}
	}
	return memcmp(&expectedColorBuffer, bufferInFloat, sizeof(expectedColorBuffer)) == 0;
}

//***************************************************************************************

TEST_CASE("BC1_Red_Ignore_Alpha", "[BC1_Red_Ignore_Alpha]")
{
	const auto block = blocks.find("BC1_Red_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock[64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, true));
}
TEST_CASE("BC1_Blue_Half_Alpha", "[BC1_Blue_Half_Alpha]")
{
	const auto block = blocks.find("BC1_Blue_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock[64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, true));
}
TEST_CASE("BC1_White_Half_Alpha", "[BC1_White_Half_Alpha]")
{
	const auto block = blocks.find("BC1_White_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock[64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, true));
}
TEST_CASE("BC1_Black_Half_Alpha", "[BC1_Black_Half_Alpha]")
{
	const auto block = blocks.find("BC1_Black_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock[64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, true));
}
TEST_CASE("BC1_Red_Blue_Half_Alpha", "[BC1_Red_Blue_Half_Alpha]")
{
	const auto block = blocks.find("BC1_Red_Blue_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock[64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, true));
}
TEST_CASE("BC1_Red_Green_Half_Alpha", "[BC1_Red_Green_Half_Alpha]")
{
	const auto block = blocks.find("BC1_Red_Green_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock[64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, true));
}
TEST_CASE("BC1_Green_Blue_Half_Alpha", "[BC1_Green_Blue_Half_Alpha]")
{
	const auto block = blocks.find("BC1_Green_Blue_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock[64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, true));
}
TEST_CASE("BC1_Red_Full_Alpha", "[BC1_Red_Full_Alpha]")
{
	const auto block = blocks.find("BC1_Red_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock[64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, true));
}
TEST_CASE("BC1_Green_Full_Alpha", "[BC1_Green_Full_Alpha]")
{
	const auto block = blocks.find("BC1_Green_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock[64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, true));
}
TEST_CASE("BC1_Blue_Full_Alpha", "[BC1_Blue_Full_Alpha]")
{
	const auto block = blocks.find("BC1_Blue_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock[64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, true));
}
TEST_CASE("BC1_White_Full_Alpha", "[BC1_White_Full_Alpha]")
{
	const auto block = blocks.find("BC1_White_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock[64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, true));
}
TEST_CASE("BC1_Green_Ignore_Alpha", "[BC1_Green_Ignore_Alpha]")
{
	const auto block = blocks.find("BC1_Green_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock[64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, true));
}
TEST_CASE("BC1_Black_Full_Alpha", "[BC1_Black_Full_Alpha]")
{
	const auto block = blocks.find("BC1_Black_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock[64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, true));
}
TEST_CASE("BC1_Red_Blue_Full_Alpha", "[BC1_Red_Blue_Full_Alpha]")
{
	const auto block = blocks.find("BC1_Red_Blue_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock[64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, true));
}
TEST_CASE("BC1_Red_Green_Full_Alpha", "[BC1_Red_Green_Full_Alpha]")
{
	const auto block = blocks.find("BC1_Red_Green_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock[64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, true));
}
TEST_CASE("BC1_Green_Blue_Full_Alpha", "[BC1_Green_Blue_Full_Alpha]")
{
	const auto block = blocks.find("BC1_Green_Blue_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock[64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, true));
}
TEST_CASE("BC1_Blue_Ignore_Alpha", "[BC1_Blue_Ignore_Alpha]")
{
	const auto block = blocks.find("BC1_Blue_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock[64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, true));
}
TEST_CASE("BC1_White_Ignore_Alpha", "[BC1_White_Ignore_Alpha]")
{
	const auto block = blocks.find("BC1_White_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock[64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, true));
}
TEST_CASE("BC1_Black_Ignore_Alpha", "[BC1_Black_Ignore_Alpha]")
{
	const auto block = blocks.find("BC1_Black_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock[64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, true));
}
TEST_CASE("BC1_Red_Blue_Ignore_Alpha", "[BC1_Red_Blue_Ignore_Alpha]")
{
	const auto block = blocks.find("BC1_Red_Blue_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock[64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, true));
}
TEST_CASE("BC1_Red_Green_Ignore_Alpha", "[BC1_Red_Green_Ignore_Alpha]")
{
	const auto block = blocks.find("BC1_Red_Green_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock[64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, true));
}
TEST_CASE("BC1_Green_Blue_Ignore_Alpha", "[BC1_Green_Blue_Ignore_Alpha]")
{
	const auto block = blocks.find("BC1_Green_Blue_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock[64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, true));
}
TEST_CASE("BC1_Red_Half_Alpha", "[BC1_Red_Half_Alpha]")
{
	const auto block = blocks.find("BC1_Red_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock[64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, true));
}
TEST_CASE("BC1_Green_Half_Alpha", "[BC1_Green_Half_Alpha]")
{
	const auto block = blocks.find("BC1_Green_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC1(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, true));
	unsigned char compBlock[8];
	unsigned char decompCompBlock[64];
	CompressBlockBC1(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC1(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, true));
}
TEST_CASE("BC2_Red_Ignore_Alpha", "[BC2_Red_Ignore_Alpha]")
{
	const auto block = blocks.find("BC2_Red_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC2_Blue_Half_Alpha", "[BC2_Blue_Half_Alpha]")
{
	const auto block = blocks.find("BC2_Blue_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC2_White_Half_Alpha", "[BC2_White_Half_Alpha]")
{
	const auto block = blocks.find("BC2_White_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC2_Black_Half_Alpha", "[BC2_Black_Half_Alpha]")
{
	const auto block = blocks.find("BC2_Black_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC2_Red_Blue_Half_Alpha", "[BC2_Red_Blue_Half_Alpha]")
{
	const auto block = blocks.find("BC2_Red_Blue_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC2_Red_Green_Half_Alpha", "[BC2_Red_Green_Half_Alpha]")
{
	const auto block = blocks.find("BC2_Red_Green_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC2_Green_Blue_Half_Alpha", "[BC2_Green_Blue_Half_Alpha]")
{
	const auto block = blocks.find("BC2_Green_Blue_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC2_Red_Full_Alpha", "[BC2_Red_Full_Alpha]")
{
	const auto block = blocks.find("BC2_Red_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC2_Green_Full_Alpha", "[BC2_Green_Full_Alpha]")
{
	const auto block = blocks.find("BC2_Green_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC2_Blue_Full_Alpha", "[BC2_Blue_Full_Alpha]")
{
	const auto block = blocks.find("BC2_Blue_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC2_White_Full_Alpha", "[BC2_White_Full_Alpha]")
{
	const auto block = blocks.find("BC2_White_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC2_Green_Ignore_Alpha", "[BC2_Green_Ignore_Alpha]")
{
	const auto block = blocks.find("BC2_Green_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC2_Black_Full_Alpha", "[BC2_Black_Full_Alpha]")
{
	const auto block = blocks.find("BC2_Black_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC2_Red_Blue_Full_Alpha", "[BC2_Red_Blue_Full_Alpha]")
{
	const auto block = blocks.find("BC2_Red_Blue_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC2_Red_Green_Full_Alpha", "[BC2_Red_Green_Full_Alpha]")
{
	const auto block = blocks.find("BC2_Red_Green_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC2_Green_Blue_Full_Alpha", "[BC2_Green_Blue_Full_Alpha]")
{
	const auto block = blocks.find("BC2_Green_Blue_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC2_Blue_Ignore_Alpha", "[BC2_Blue_Ignore_Alpha]")
{
	const auto block = blocks.find("BC2_Blue_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC2_White_Ignore_Alpha", "[BC2_White_Ignore_Alpha]")
{
	const auto block = blocks.find("BC2_White_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC2_Black_Ignore_Alpha", "[BC2_Black_Ignore_Alpha]")
{
	const auto block = blocks.find("BC2_Black_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC2_Red_Blue_Ignore_Alpha", "[BC2_Red_Blue_Ignore_Alpha]")
{
	const auto block = blocks.find("BC2_Red_Blue_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC2_Red_Green_Ignore_Alpha", "[BC2_Red_Green_Ignore_Alpha]")
{
	const auto block = blocks.find("BC2_Red_Green_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC2_Green_Blue_Ignore_Alpha", "[BC2_Green_Blue_Ignore_Alpha]")
{
	const auto block = blocks.find("BC2_Green_Blue_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC2_Red_Half_Alpha", "[BC2_Red_Half_Alpha]")
{
	const auto block = blocks.find("BC2_Red_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC2_Green_Half_Alpha", "[BC2_Green_Half_Alpha]")
{
	const auto block = blocks.find("BC2_Green_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC2(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC2(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC2(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC3_Red_Ignore_Alpha", "[BC3_Red_Ignore_Alpha]")
{
	const auto block = blocks.find("BC3_Red_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC3_Blue_Half_Alpha", "[BC3_Blue_Half_Alpha]")
{
	const auto block = blocks.find("BC3_Blue_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC3_White_Half_Alpha", "[BC3_White_Half_Alpha]")
{
	const auto block = blocks.find("BC3_White_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC3_Black_Half_Alpha", "[BC3_Black_Half_Alpha]")
{
	const auto block = blocks.find("BC3_Black_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC3_Red_Blue_Half_Alpha", "[BC3_Red_Blue_Half_Alpha]")
{
	const auto block = blocks.find("BC3_Red_Blue_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC3_Red_Green_Half_Alpha", "[BC3_Red_Green_Half_Alpha]")
{
	const auto block = blocks.find("BC3_Red_Green_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC3_Green_Blue_Half_Alpha", "[BC3_Green_Blue_Half_Alpha]")
{
	const auto block = blocks.find("BC3_Green_Blue_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC3_Red_Full_Alpha", "[BC3_Red_Full_Alpha]")
{
	const auto block = blocks.find("BC3_Red_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC3_Green_Full_Alpha", "[BC3_Green_Full_Alpha]")
{
	const auto block = blocks.find("BC3_Green_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC3_Blue_Full_Alpha", "[BC3_Blue_Full_Alpha]")
{
	const auto block = blocks.find("BC3_Blue_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC3_White_Full_Alpha", "[BC3_White_Full_Alpha]")
{
	const auto block = blocks.find("BC3_White_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC3_Green_Ignore_Alpha", "[BC3_Green_Ignore_Alpha]")
{
	const auto block = blocks.find("BC3_Green_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC3_Black_Full_Alpha", "[BC3_Black_Full_Alpha]")
{
	const auto block = blocks.find("BC3_Black_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC3_Red_Blue_Full_Alpha", "[BC3_Red_Blue_Full_Alpha]")
{
	const auto block = blocks.find("BC3_Red_Blue_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC3_Red_Green_Full_Alpha", "[BC3_Red_Green_Full_Alpha]")
{
	const auto block = blocks.find("BC3_Red_Green_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC3_Green_Blue_Full_Alpha", "[BC3_Green_Blue_Full_Alpha]")
{
	const auto block = blocks.find("BC3_Green_Blue_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC3_Blue_Ignore_Alpha", "[BC3_Blue_Ignore_Alpha]")
{
	const auto block = blocks.find("BC3_Blue_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC3_White_Ignore_Alpha", "[BC3_White_Ignore_Alpha]")
{
	const auto block = blocks.find("BC3_White_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC3_Black_Ignore_Alpha", "[BC3_Black_Ignore_Alpha]")
{
	const auto block = blocks.find("BC3_Black_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC3_Red_Blue_Ignore_Alpha", "[BC3_Red_Blue_Ignore_Alpha]")
{
	const auto block = blocks.find("BC3_Red_Blue_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC3_Red_Green_Ignore_Alpha", "[BC3_Red_Green_Ignore_Alpha]")
{
	const auto block = blocks.find("BC3_Red_Green_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC3_Green_Blue_Ignore_Alpha", "[BC3_Green_Blue_Ignore_Alpha]")
{
	const auto block = blocks.find("BC3_Green_Blue_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC3_Red_Half_Alpha", "[BC3_Red_Half_Alpha]")
{
	const auto block = blocks.find("BC3_Red_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC3_Green_Half_Alpha", "[BC3_Green_Half_Alpha]")
{
	const auto block = blocks.find("BC3_Green_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC3(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC3(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC3(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC4_Red_Ignore_Alpha", "[BC4_Red_Ignore_Alpha]")
{
	const auto block = blocks.find("BC4_Red_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[16];
	DecompressBlockBC4(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[16];
	CompressBlockBC4(decompBlock, 4, compBlock, nullptr);
	DecompressBlockBC4(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompCompBlock, blockColor));
}
TEST_CASE("BC4_Blue_Half_Alpha", "[BC4_Blue_Half_Alpha]")
{
	const auto block = blocks.find("BC4_Blue_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[16];
	DecompressBlockBC4(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[16];
	CompressBlockBC4(decompBlock, 4, compBlock, nullptr);
	DecompressBlockBC4(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompCompBlock, blockColor));
}
TEST_CASE("BC4_White_Half_Alpha", "[BC4_White_Half_Alpha]")
{
	const auto block = blocks.find("BC4_White_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[16];
	DecompressBlockBC4(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[16];
	CompressBlockBC4(decompBlock, 4, compBlock, nullptr);
	DecompressBlockBC4(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompCompBlock, blockColor));
}
TEST_CASE("BC4_Black_Half_Alpha", "[BC4_Black_Half_Alpha]")
{
	const auto block = blocks.find("BC4_Black_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[16];
	DecompressBlockBC4(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[16];
	CompressBlockBC4(decompBlock, 4, compBlock, nullptr);
	DecompressBlockBC4(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompCompBlock, blockColor));
}
TEST_CASE("BC4_Red_Blue_Half_Alpha", "[BC4_Red_Blue_Half_Alpha]")
{
	const auto block = blocks.find("BC4_Red_Blue_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[16];
	DecompressBlockBC4(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[16];
	CompressBlockBC4(decompBlock, 4, compBlock, nullptr);
	DecompressBlockBC4(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompCompBlock, blockColor));
}
TEST_CASE("BC4_Red_Green_Half_Alpha", "[BC4_Red_Green_Half_Alpha]")
{
	const auto block = blocks.find("BC4_Red_Green_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[16];
	DecompressBlockBC4(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[16];
	CompressBlockBC4(decompBlock, 4, compBlock, nullptr);
	DecompressBlockBC4(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompCompBlock, blockColor));
}
TEST_CASE("BC4_Green_Blue_Half_Alpha", "[BC4_Green_Blue_Half_Alpha]")
{
	const auto block = blocks.find("BC4_Green_Blue_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[16];
	DecompressBlockBC4(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[16];
	CompressBlockBC4(decompBlock, 4, compBlock, nullptr);
	DecompressBlockBC4(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompCompBlock, blockColor));
}
TEST_CASE("BC4_Red_Full_Alpha", "[BC4_Red_Full_Alpha]")
{
	const auto block = blocks.find("BC4_Red_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[16];
	DecompressBlockBC4(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[16];
	CompressBlockBC4(decompBlock, 4, compBlock, nullptr);
	DecompressBlockBC4(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompCompBlock, blockColor));
}
TEST_CASE("BC4_Green_Full_Alpha", "[BC4_Green_Full_Alpha]")
{
	const auto block = blocks.find("BC4_Green_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[16];
	DecompressBlockBC4(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[16];
	CompressBlockBC4(decompBlock, 4, compBlock, nullptr);
	DecompressBlockBC4(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompCompBlock, blockColor));
}
TEST_CASE("BC4_Blue_Full_Alpha", "[BC4_Blue_Full_Alpha]")
{
	const auto block = blocks.find("BC4_Blue_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[16];
	DecompressBlockBC4(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[16];
	CompressBlockBC4(decompBlock, 4, compBlock, nullptr);
	DecompressBlockBC4(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompCompBlock, blockColor));
}
TEST_CASE("BC4_White_Full_Alpha", "[BC4_White_Full_Alpha]")
{
	const auto block = blocks.find("BC4_White_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[16];
	DecompressBlockBC4(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[16];
	CompressBlockBC4(decompBlock, 4, compBlock, nullptr);
	DecompressBlockBC4(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompCompBlock, blockColor));
}
TEST_CASE("BC4_Green_Ignore_Alpha", "[BC4_Green_Ignore_Alpha]")
{
	const auto block = blocks.find("BC4_Green_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[16];
	DecompressBlockBC4(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[16];
	CompressBlockBC4(decompBlock, 4, compBlock, nullptr);
	DecompressBlockBC4(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompCompBlock, blockColor));
}
TEST_CASE("BC4_Black_Full_Alpha", "[BC4_Black_Full_Alpha]")
{
	const auto block = blocks.find("BC4_Black_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[16];
	DecompressBlockBC4(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[16];
	CompressBlockBC4(decompBlock, 4, compBlock, nullptr);
	DecompressBlockBC4(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompCompBlock, blockColor));
}
TEST_CASE("BC4_Red_Blue_Full_Alpha", "[BC4_Red_Blue_Full_Alpha]")
{
	const auto block = blocks.find("BC4_Red_Blue_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[16];
	DecompressBlockBC4(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[16];
	CompressBlockBC4(decompBlock, 4, compBlock, nullptr);
	DecompressBlockBC4(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompCompBlock, blockColor));
}
TEST_CASE("BC4_Red_Green_Full_Alpha", "[BC4_Red_Green_Full_Alpha]")
{
	const auto block = blocks.find("BC4_Red_Green_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[16];
	DecompressBlockBC4(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[16];
	CompressBlockBC4(decompBlock, 4, compBlock, nullptr);
	DecompressBlockBC4(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompCompBlock, blockColor));
}
TEST_CASE("BC4_Green_Blue_Full_Alpha", "[BC4_Green_Blue_Full_Alpha]")
{
	const auto block = blocks.find("BC4_Green_Blue_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[16];
	DecompressBlockBC4(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[16];
	CompressBlockBC4(decompBlock, 4, compBlock, nullptr);
	DecompressBlockBC4(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompCompBlock, blockColor));
}
TEST_CASE("BC4_Blue_Ignore_Alpha", "[BC4_Blue_Ignore_Alpha]")
{
	const auto block = blocks.find("BC4_Blue_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[16];
	DecompressBlockBC4(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[16];
	CompressBlockBC4(decompBlock, 4, compBlock, nullptr);
	DecompressBlockBC4(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompCompBlock, blockColor));
}
TEST_CASE("BC4_White_Ignore_Alpha", "[BC4_White_Ignore_Alpha]")
{
	const auto block = blocks.find("BC4_White_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[16];
	DecompressBlockBC4(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[16];
	CompressBlockBC4(decompBlock, 4, compBlock, nullptr);
	DecompressBlockBC4(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompCompBlock, blockColor));
}
TEST_CASE("BC4_Black_Ignore_Alpha", "[BC4_Black_Ignore_Alpha]")
{
	const auto block = blocks.find("BC4_Black_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[16];
	DecompressBlockBC4(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[16];
	CompressBlockBC4(decompBlock, 4, compBlock, nullptr);
	DecompressBlockBC4(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompCompBlock, blockColor));
}
TEST_CASE("BC4_Red_Blue_Ignore_Alpha", "[BC4_Red_Blue_Ignore_Alpha]")
{
	const auto block = blocks.find("BC4_Red_Blue_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[16];
	DecompressBlockBC4(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[16];
	CompressBlockBC4(decompBlock, 4, compBlock, nullptr);
	DecompressBlockBC4(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompCompBlock, blockColor));
}
TEST_CASE("BC4_Red_Green_Ignore_Alpha", "[BC4_Red_Green_Ignore_Alpha]")
{
	const auto block = blocks.find("BC4_Red_Green_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[16];
	DecompressBlockBC4(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[16];
	CompressBlockBC4(decompBlock, 4, compBlock, nullptr);
	DecompressBlockBC4(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompCompBlock, blockColor));
}
TEST_CASE("BC4_Green_Blue_Ignore_Alpha", "[BC4_Green_Blue_Ignore_Alpha]")
{
	const auto block = blocks.find("BC4_Green_Blue_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[16];
	DecompressBlockBC4(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[16];
	CompressBlockBC4(decompBlock, 4, compBlock, nullptr);
	DecompressBlockBC4(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompCompBlock, blockColor));
}
TEST_CASE("BC4_Red_Half_Alpha", "[BC4_Red_Half_Alpha]")
{
	const auto block = blocks.find("BC4_Red_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[16];
	DecompressBlockBC4(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[16];
	CompressBlockBC4(decompBlock, 4, compBlock, nullptr);
	DecompressBlockBC4(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompCompBlock, blockColor));
}
TEST_CASE("BC4_Green_Half_Alpha", "[BC4_Green_Half_Alpha]")
{
	const auto block = blocks.find("BC4_Green_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[16];
	DecompressBlockBC4(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[16];
	CompressBlockBC4(decompBlock, 4, compBlock, nullptr);
	DecompressBlockBC4(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC4(decompCompBlock, blockColor));
}
TEST_CASE("BC5_Red_Ignore_Alpha", "[BC5_Red_Ignore_Alpha]")
{
	const auto block = blocks.find("BC5_Red_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlockR[16];
	unsigned char decompBlockG[16];
	DecompressBlockBC5(blockData, decompBlockR, decompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompBlockR, decompBlockG, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlockR[16];
	unsigned char decompCompBlockG[16];
	CompressBlockBC5(decompBlockR, 4, decompBlockG, 4, compBlock, nullptr);
	DecompressBlockBC5(compBlock, decompCompBlockR, decompCompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompCompBlockR, decompCompBlockG, blockColor));
}
TEST_CASE("BC5_Blue_Half_Alpha", "[BC5_Blue_Half_Alpha]")
{
	const auto block = blocks.find("BC5_Blue_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlockR[16];
	unsigned char decompBlockG[16];
	DecompressBlockBC5(blockData, decompBlockR, decompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompBlockR, decompBlockG, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlockR[16];
	unsigned char decompCompBlockG[16];
	CompressBlockBC5(decompBlockR, 4, decompBlockG, 4, compBlock, nullptr);
	DecompressBlockBC5(compBlock, decompCompBlockR, decompCompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompCompBlockR, decompCompBlockG, blockColor));
}
TEST_CASE("BC5_White_Half_Alpha", "[BC5_White_Half_Alpha]")
{
	const auto block = blocks.find("BC5_White_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlockR[16];
	unsigned char decompBlockG[16];
	DecompressBlockBC5(blockData, decompBlockR, decompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompBlockR, decompBlockG, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlockR[16];
	unsigned char decompCompBlockG[16];
	CompressBlockBC5(decompBlockR, 4, decompBlockG, 4, compBlock, nullptr);
	DecompressBlockBC5(compBlock, decompCompBlockR, decompCompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompCompBlockR, decompCompBlockG, blockColor));
}
TEST_CASE("BC5_Black_Half_Alpha", "[BC5_Black_Half_Alpha]")
{
	const auto block = blocks.find("BC5_Black_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlockR[16];
	unsigned char decompBlockG[16];
	DecompressBlockBC5(blockData, decompBlockR, decompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompBlockR, decompBlockG, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlockR[16];
	unsigned char decompCompBlockG[16];
	CompressBlockBC5(decompBlockR, 4, decompBlockG, 4, compBlock, nullptr);
	DecompressBlockBC5(compBlock, decompCompBlockR, decompCompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompCompBlockR, decompCompBlockG, blockColor));
}
TEST_CASE("BC5_Red_Blue_Half_Alpha", "[BC5_Red_Blue_Half_Alpha]")
{
	const auto block = blocks.find("BC5_Red_Blue_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlockR[16];
	unsigned char decompBlockG[16];
	DecompressBlockBC5(blockData, decompBlockR, decompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompBlockR, decompBlockG, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlockR[16];
	unsigned char decompCompBlockG[16];
	CompressBlockBC5(decompBlockR, 4, decompBlockG, 4, compBlock, nullptr);
	DecompressBlockBC5(compBlock, decompCompBlockR, decompCompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompCompBlockR, decompCompBlockG, blockColor));
}
TEST_CASE("BC5_Red_Green_Half_Alpha", "[BC5_Red_Green_Half_Alpha]")
{
	const auto block = blocks.find("BC5_Red_Green_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlockR[16];
	unsigned char decompBlockG[16];
	DecompressBlockBC5(blockData, decompBlockR, decompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompBlockR, decompBlockG, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlockR[16];
	unsigned char decompCompBlockG[16];
	CompressBlockBC5(decompBlockR, 4, decompBlockG, 4, compBlock, nullptr);
	DecompressBlockBC5(compBlock, decompCompBlockR, decompCompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompCompBlockR, decompCompBlockG, blockColor));
}
TEST_CASE("BC5_Green_Blue_Half_Alpha", "[BC5_Green_Blue_Half_Alpha]")
{
	const auto block = blocks.find("BC5_Green_Blue_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlockR[16];
	unsigned char decompBlockG[16];
	DecompressBlockBC5(blockData, decompBlockR, decompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompBlockR, decompBlockG, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlockR[16];
	unsigned char decompCompBlockG[16];
	CompressBlockBC5(decompBlockR, 4, decompBlockG, 4, compBlock, nullptr);
	DecompressBlockBC5(compBlock, decompCompBlockR, decompCompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompCompBlockR, decompCompBlockG, blockColor));
}
TEST_CASE("BC5_Red_Full_Alpha", "[BC5_Red_Full_Alpha]")
{
	const auto block = blocks.find("BC5_Red_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlockR[16];
	unsigned char decompBlockG[16];
	DecompressBlockBC5(blockData, decompBlockR, decompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompBlockR, decompBlockG, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlockR[16];
	unsigned char decompCompBlockG[16];
	CompressBlockBC5(decompBlockR, 4, decompBlockG, 4, compBlock, nullptr);
	DecompressBlockBC5(compBlock, decompCompBlockR, decompCompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompCompBlockR, decompCompBlockG, blockColor));
}
TEST_CASE("BC5_Green_Full_Alpha", "[BC5_Green_Full_Alpha]")
{
	const auto block = blocks.find("BC5_Green_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlockR[16];
	unsigned char decompBlockG[16];
	DecompressBlockBC5(blockData, decompBlockR, decompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompBlockR, decompBlockG, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlockR[16];
	unsigned char decompCompBlockG[16];
	CompressBlockBC5(decompBlockR, 4, decompBlockG, 4, compBlock, nullptr);
	DecompressBlockBC5(compBlock, decompCompBlockR, decompCompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompCompBlockR, decompCompBlockG, blockColor));
}
TEST_CASE("BC5_Blue_Full_Alpha", "[BC5_Blue_Full_Alpha]")
{
	const auto block = blocks.find("BC5_Blue_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlockR[16];
	unsigned char decompBlockG[16];
	DecompressBlockBC5(blockData, decompBlockR, decompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompBlockR, decompBlockG, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlockR[16];
	unsigned char decompCompBlockG[16];
	CompressBlockBC5(decompBlockR, 4, decompBlockG, 4, compBlock, nullptr);
	DecompressBlockBC5(compBlock, decompCompBlockR, decompCompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompCompBlockR, decompCompBlockG, blockColor));
}
TEST_CASE("BC5_White_Full_Alpha", "[BC5_White_Full_Alpha]")
{
	const auto block = blocks.find("BC5_White_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlockR[16];
	unsigned char decompBlockG[16];
	DecompressBlockBC5(blockData, decompBlockR, decompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompBlockR, decompBlockG, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlockR[16];
	unsigned char decompCompBlockG[16];
	CompressBlockBC5(decompBlockR, 4, decompBlockG, 4, compBlock, nullptr);
	DecompressBlockBC5(compBlock, decompCompBlockR, decompCompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompCompBlockR, decompCompBlockG, blockColor));
}
TEST_CASE("BC5_Green_Ignore_Alpha", "[BC5_Green_Ignore_Alpha]")
{
	const auto block = blocks.find("BC5_Green_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlockR[16];
	unsigned char decompBlockG[16];
	DecompressBlockBC5(blockData, decompBlockR, decompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompBlockR, decompBlockG, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlockR[16];
	unsigned char decompCompBlockG[16];
	CompressBlockBC5(decompBlockR, 4, decompBlockG, 4, compBlock, nullptr);
	DecompressBlockBC5(compBlock, decompCompBlockR, decompCompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompCompBlockR, decompCompBlockG, blockColor));
}
TEST_CASE("BC5_Black_Full_Alpha", "[BC5_Black_Full_Alpha]")
{
	const auto block = blocks.find("BC5_Black_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlockR[16];
	unsigned char decompBlockG[16];
	DecompressBlockBC5(blockData, decompBlockR, decompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompBlockR, decompBlockG, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlockR[16];
	unsigned char decompCompBlockG[16];
	CompressBlockBC5(decompBlockR, 4, decompBlockG, 4, compBlock, nullptr);
	DecompressBlockBC5(compBlock, decompCompBlockR, decompCompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompCompBlockR, decompCompBlockG, blockColor));
}
TEST_CASE("BC5_Red_Blue_Full_Alpha", "[BC5_Red_Blue_Full_Alpha]")
{
	const auto block = blocks.find("BC5_Red_Blue_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlockR[16];
	unsigned char decompBlockG[16];
	DecompressBlockBC5(blockData, decompBlockR, decompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompBlockR, decompBlockG, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlockR[16];
	unsigned char decompCompBlockG[16];
	CompressBlockBC5(decompBlockR, 4, decompBlockG, 4, compBlock, nullptr);
	DecompressBlockBC5(compBlock, decompCompBlockR, decompCompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompCompBlockR, decompCompBlockG, blockColor));
}
TEST_CASE("BC5_Red_Green_Full_Alpha", "[BC5_Red_Green_Full_Alpha]")
{
	const auto block = blocks.find("BC5_Red_Green_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlockR[16];
	unsigned char decompBlockG[16];
	DecompressBlockBC5(blockData, decompBlockR, decompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompBlockR, decompBlockG, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlockR[16];
	unsigned char decompCompBlockG[16];
	CompressBlockBC5(decompBlockR, 4, decompBlockG, 4, compBlock, nullptr);
	DecompressBlockBC5(compBlock, decompCompBlockR, decompCompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompCompBlockR, decompCompBlockG, blockColor));
}
TEST_CASE("BC5_Green_Blue_Full_Alpha", "[BC5_Green_Blue_Full_Alpha]")
{
	const auto block = blocks.find("BC5_Green_Blue_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlockR[16];
	unsigned char decompBlockG[16];
	DecompressBlockBC5(blockData, decompBlockR, decompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompBlockR, decompBlockG, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlockR[16];
	unsigned char decompCompBlockG[16];
	CompressBlockBC5(decompBlockR, 4, decompBlockG, 4, compBlock, nullptr);
	DecompressBlockBC5(compBlock, decompCompBlockR, decompCompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompCompBlockR, decompCompBlockG, blockColor));
}
TEST_CASE("BC5_Blue_Ignore_Alpha", "[BC5_Blue_Ignore_Alpha]")
{
	const auto block = blocks.find("BC5_Blue_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlockR[16];
	unsigned char decompBlockG[16];
	DecompressBlockBC5(blockData, decompBlockR, decompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompBlockR, decompBlockG, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlockR[16];
	unsigned char decompCompBlockG[16];
	CompressBlockBC5(decompBlockR, 4, decompBlockG, 4, compBlock, nullptr);
	DecompressBlockBC5(compBlock, decompCompBlockR, decompCompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompCompBlockR, decompCompBlockG, blockColor));
}
TEST_CASE("BC5_White_Ignore_Alpha", "[BC5_White_Ignore_Alpha]")
{
	const auto block = blocks.find("BC5_White_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlockR[16];
	unsigned char decompBlockG[16];
	DecompressBlockBC5(blockData, decompBlockR, decompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompBlockR, decompBlockG, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlockR[16];
	unsigned char decompCompBlockG[16];
	CompressBlockBC5(decompBlockR, 4, decompBlockG, 4, compBlock, nullptr);
	DecompressBlockBC5(compBlock, decompCompBlockR, decompCompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompCompBlockR, decompCompBlockG, blockColor));
}
TEST_CASE("BC5_Black_Ignore_Alpha", "[BC5_Black_Ignore_Alpha]")
{
	const auto block = blocks.find("BC5_Black_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlockR[16];
	unsigned char decompBlockG[16];
	DecompressBlockBC5(blockData, decompBlockR, decompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompBlockR, decompBlockG, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlockR[16];
	unsigned char decompCompBlockG[16];
	CompressBlockBC5(decompBlockR, 4, decompBlockG, 4, compBlock, nullptr);
	DecompressBlockBC5(compBlock, decompCompBlockR, decompCompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompCompBlockR, decompCompBlockG, blockColor));
}
TEST_CASE("BC5_Red_Blue_Ignore_Alpha", "[BC5_Red_Blue_Ignore_Alpha]")
{
	const auto block = blocks.find("BC5_Red_Blue_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlockR[16];
	unsigned char decompBlockG[16];
	DecompressBlockBC5(blockData, decompBlockR, decompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompBlockR, decompBlockG, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlockR[16];
	unsigned char decompCompBlockG[16];
	CompressBlockBC5(decompBlockR, 4, decompBlockG, 4, compBlock, nullptr);
	DecompressBlockBC5(compBlock, decompCompBlockR, decompCompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompCompBlockR, decompCompBlockG, blockColor));
}
TEST_CASE("BC5_Red_Green_Ignore_Alpha", "[BC5_Red_Green_Ignore_Alpha]")
{
	const auto block = blocks.find("BC5_Red_Green_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlockR[16];
	unsigned char decompBlockG[16];
	DecompressBlockBC5(blockData, decompBlockR, decompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompBlockR, decompBlockG, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlockR[16];
	unsigned char decompCompBlockG[16];
	CompressBlockBC5(decompBlockR, 4, decompBlockG, 4, compBlock, nullptr);
	DecompressBlockBC5(compBlock, decompCompBlockR, decompCompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompCompBlockR, decompCompBlockG, blockColor));
}
TEST_CASE("BC5_Green_Blue_Ignore_Alpha", "[BC5_Green_Blue_Ignore_Alpha]")
{
	const auto block = blocks.find("BC5_Green_Blue_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlockR[16];
	unsigned char decompBlockG[16];
	DecompressBlockBC5(blockData, decompBlockR, decompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompBlockR, decompBlockG, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlockR[16];
	unsigned char decompCompBlockG[16];
	CompressBlockBC5(decompBlockR, 4, decompBlockG, 4, compBlock, nullptr);
	DecompressBlockBC5(compBlock, decompCompBlockR, decompCompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompCompBlockR, decompCompBlockG, blockColor));
}
TEST_CASE("BC5_Red_Half_Alpha", "[BC5_Red_Half_Alpha]")
{
	const auto block = blocks.find("BC5_Red_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlockR[16];
	unsigned char decompBlockG[16];
	DecompressBlockBC5(blockData, decompBlockR, decompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompBlockR, decompBlockG, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlockR[16];
	unsigned char decompCompBlockG[16];
	CompressBlockBC5(decompBlockR, 4, decompBlockG, 4, compBlock, nullptr);
	DecompressBlockBC5(compBlock, decompCompBlockR, decompCompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompCompBlockR, decompCompBlockG, blockColor));
}
TEST_CASE("BC5_Green_Half_Alpha", "[BC5_Green_Half_Alpha]")
{
	const auto block = blocks.find("BC5_Green_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlockR[16];
	unsigned char decompBlockG[16];
	DecompressBlockBC5(blockData, decompBlockR, decompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompBlockR, decompBlockG, blockColor));
	unsigned char compBlock[16];
	unsigned char decompCompBlockR[16];
	unsigned char decompCompBlockG[16];
	CompressBlockBC5(decompBlockR, 4, decompBlockG, 4, compBlock, nullptr);
	DecompressBlockBC5(compBlock, decompCompBlockR, decompCompBlockG, nullptr);
	CHECK(ColorMatchesBC5(decompCompBlockR, decompCompBlockG, blockColor));
}
TEST_CASE("BC6_Red_Ignore_Alpha", "[BC6_Red_Ignore_Alpha]")
{
	const auto block = blocksBC6.find("BC6_Red_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned short decompBlock[64];
	DecompressBlockBC6(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned short decompCompBlock[48];
	CompressBlockBC6(decompBlock, 12, compBlock, nullptr);
	DecompressBlockBC6(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompCompBlock, blockColor));
}
TEST_CASE("BC6_Blue_Half_Alpha", "[BC6_Blue_Half_Alpha]")
{
	const auto block = blocksBC6.find("BC6_Blue_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned short decompBlock[64];
	DecompressBlockBC6(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned short decompCompBlock[48];
	CompressBlockBC6(decompBlock, 12, compBlock, nullptr);
	DecompressBlockBC6(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompCompBlock, blockColor));
}
TEST_CASE("BC6_White_Half_Alpha", "[BC6_White_Half_Alpha]")
{
	const auto block = blocksBC6.find("BC6_White_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned short decompBlock[64];
	DecompressBlockBC6(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned short decompCompBlock[48];
	CompressBlockBC6(decompBlock, 12, compBlock, nullptr);
	DecompressBlockBC6(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompCompBlock, blockColor));
}
TEST_CASE("BC6_Black_Half_Alpha", "[BC6_Black_Half_Alpha]")
{
	const auto block = blocksBC6.find("BC6_Black_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned short decompBlock[64];
	DecompressBlockBC6(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned short decompCompBlock[48];
	CompressBlockBC6(decompBlock, 12, compBlock, nullptr);
	DecompressBlockBC6(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompCompBlock, blockColor));
}
TEST_CASE("BC6_Red_Blue_Half_Alpha", "[BC6_Red_Blue_Half_Alpha]")
{
	const auto block = blocksBC6.find("BC6_Red_Blue_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned short decompBlock[64];
	DecompressBlockBC6(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned short decompCompBlock[48];
	CompressBlockBC6(decompBlock, 12, compBlock, nullptr);
	DecompressBlockBC6(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompCompBlock, blockColor));
}
TEST_CASE("BC6_Red_Green_Half_Alpha", "[BC6_Red_Green_Half_Alpha]")
{
	const auto block = blocksBC6.find("BC6_Red_Green_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned short decompBlock[64];
	DecompressBlockBC6(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned short decompCompBlock[48];
	CompressBlockBC6(decompBlock, 12, compBlock, nullptr);
	DecompressBlockBC6(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompCompBlock, blockColor));
}
TEST_CASE("BC6_Green_Blue_Half_Alpha", "[BC6_Green_Blue_Half_Alpha]")
{
	const auto block = blocksBC6.find("BC6_Green_Blue_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned short decompBlock[64];
	DecompressBlockBC6(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned short decompCompBlock[48];
	CompressBlockBC6(decompBlock, 12, compBlock, nullptr);
	DecompressBlockBC6(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompCompBlock, blockColor));
}
TEST_CASE("BC6_Red_Full_Alpha", "[BC6_Red_Full_Alpha]")
{
	const auto block = blocksBC6.find("BC6_Red_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned short decompBlock[64];
	DecompressBlockBC6(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned short decompCompBlock[48];
	CompressBlockBC6(decompBlock, 12, compBlock, nullptr);
	DecompressBlockBC6(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompCompBlock, blockColor));
}
TEST_CASE("BC6_Green_Full_Alpha", "[BC6_Green_Full_Alpha]")
{
	const auto block = blocksBC6.find("BC6_Green_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned short decompBlock[64];
	DecompressBlockBC6(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned short decompCompBlock[48];
	CompressBlockBC6(decompBlock, 12, compBlock, nullptr);
	DecompressBlockBC6(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompCompBlock, blockColor));
}
TEST_CASE("BC6_Blue_Full_Alpha", "[BC6_Blue_Full_Alpha]")
{
	const auto block = blocksBC6.find("BC6_Blue_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned short decompBlock[64];
	DecompressBlockBC6(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned short decompCompBlock[48];
	CompressBlockBC6(decompBlock, 12, compBlock, nullptr);
	DecompressBlockBC6(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompCompBlock, blockColor));
}
TEST_CASE("BC6_White_Full_Alpha", "[BC6_White_Full_Alpha]")
{
	const auto block = blocksBC6.find("BC6_White_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned short decompBlock[64];
	DecompressBlockBC6(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned short decompCompBlock[48];
	CompressBlockBC6(decompBlock, 12, compBlock, nullptr);
	DecompressBlockBC6(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompCompBlock, blockColor));
}
TEST_CASE("BC6_Green_Ignore_Alpha", "[BC6_Green_Ignore_Alpha]")
{
	const auto block = blocksBC6.find("BC6_Green_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned short decompBlock[64];
	DecompressBlockBC6(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned short decompCompBlock[48];
	CompressBlockBC6(decompBlock, 12, compBlock, nullptr);
	DecompressBlockBC6(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompCompBlock, blockColor));
}
TEST_CASE("BC6_Black_Full_Alpha", "[BC6_Black_Full_Alpha]")
{
	const auto block = blocksBC6.find("BC6_Black_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned short decompBlock[64];
	DecompressBlockBC6(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned short decompCompBlock[48];
	CompressBlockBC6(decompBlock, 12, compBlock, nullptr);
	DecompressBlockBC6(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompCompBlock, blockColor));
}
TEST_CASE("BC6_Red_Blue_Full_Alpha", "[BC6_Red_Blue_Full_Alpha]")
{
	const auto block = blocksBC6.find("BC6_Red_Blue_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned short decompBlock[64];
	DecompressBlockBC6(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned short decompCompBlock[48];
	CompressBlockBC6(decompBlock, 12, compBlock, nullptr);
	DecompressBlockBC6(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompCompBlock, blockColor));
}
TEST_CASE("BC6_Red_Green_Full_Alpha", "[BC6_Red_Green_Full_Alpha]")
{
	const auto block = blocksBC6.find("BC6_Red_Green_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned short decompBlock[64];
	DecompressBlockBC6(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned short decompCompBlock[48];
	CompressBlockBC6(decompBlock, 12, compBlock, nullptr);
	DecompressBlockBC6(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompCompBlock, blockColor));
}
TEST_CASE("BC6_Green_Blue_Full_Alpha", "[BC6_Green_Blue_Full_Alpha]")
{
	const auto block = blocksBC6.find("BC6_Green_Blue_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned short decompBlock[64];
	DecompressBlockBC6(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned short decompCompBlock[48];
	CompressBlockBC6(decompBlock, 12, compBlock, nullptr);
	DecompressBlockBC6(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompCompBlock, blockColor));
}
TEST_CASE("BC6_Blue_Ignore_Alpha", "[BC6_Blue_Ignore_Alpha]")
{
	const auto block = blocksBC6.find("BC6_Blue_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned short decompBlock[64];
	DecompressBlockBC6(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned short decompCompBlock[48];
	CompressBlockBC6(decompBlock, 12, compBlock, nullptr);
	DecompressBlockBC6(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompCompBlock, blockColor));
}
TEST_CASE("BC6_White_Ignore_Alpha", "[BC6_White_Ignore_Alpha]")
{
	const auto block = blocksBC6.find("BC6_White_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned short decompBlock[64];
	DecompressBlockBC6(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned short decompCompBlock[48];
	CompressBlockBC6(decompBlock, 12, compBlock, nullptr);
	DecompressBlockBC6(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompCompBlock, blockColor));
}
TEST_CASE("BC6_Black_Ignore_Alpha", "[BC6_Black_Ignore_Alpha]")
{
	const auto block = blocksBC6.find("BC6_Black_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned short decompBlock[64];
	DecompressBlockBC6(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned short decompCompBlock[48];
	CompressBlockBC6(decompBlock, 12, compBlock, nullptr);
	DecompressBlockBC6(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompCompBlock, blockColor));
}
TEST_CASE("BC6_Red_Blue_Ignore_Alpha", "[BC6_Red_Blue_Ignore_Alpha]")
{
	const auto block = blocksBC6.find("BC6_Red_Blue_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned short decompBlock[64];
	DecompressBlockBC6(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned short decompCompBlock[48];
	CompressBlockBC6(decompBlock, 12, compBlock, nullptr);
	DecompressBlockBC6(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompCompBlock, blockColor));
}
TEST_CASE("BC6_Red_Green_Ignore_Alpha", "[BC6_Red_Green_Ignore_Alpha]")
{
	const auto block = blocksBC6.find("BC6_Red_Green_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned short decompBlock[64];
	DecompressBlockBC6(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned short decompCompBlock[48];
	CompressBlockBC6(decompBlock, 12, compBlock, nullptr);
	DecompressBlockBC6(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompCompBlock, blockColor));
}
TEST_CASE("BC6_Green_Blue_Ignore_Alpha", "[BC6_Green_Blue_Ignore_Alpha]")
{
	const auto block = blocksBC6.find("BC6_Green_Blue_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned short decompBlock[64];
	DecompressBlockBC6(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned short decompCompBlock[48];
	CompressBlockBC6(decompBlock, 12, compBlock, nullptr);
	DecompressBlockBC6(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompCompBlock, blockColor));
}
TEST_CASE("BC6_Red_Half_Alpha", "[BC6_Red_Half_Alpha]")
{
	const auto block = blocksBC6.find("BC6_Red_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned short decompBlock[64];
	DecompressBlockBC6(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned short decompCompBlock[48];
	CompressBlockBC6(decompBlock, 12, compBlock, nullptr);
	DecompressBlockBC6(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompCompBlock, blockColor));
}
TEST_CASE("BC6_Green_Half_Alpha", "[BC6_Green_Half_Alpha]")
{
	const auto block = blocksBC6.find("BC6_Green_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned short decompBlock[64];
	DecompressBlockBC6(blockData, decompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompBlock, blockColor));
	unsigned char compBlock[16];
	unsigned short decompCompBlock[48];
	CompressBlockBC6(decompBlock, 12, compBlock, nullptr);
	DecompressBlockBC6(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatchesBC6(decompCompBlock, blockColor));
}
TEST_CASE("BC7_Red_Ignore_Alpha", "[BC7_Red_Ignore_Alpha]")
{
	const auto block = blocks.find("BC7_Red_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC7(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC7(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC7(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC7_Blue_Half_Alpha", "[BC7_Blue_Half_Alpha]")
{
	const auto block = blocks.find("BC7_Blue_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC7(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC7(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC7(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC7_White_Half_Alpha", "[BC7_White_Half_Alpha]")
{
	const auto block = blocks.find("BC7_White_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC7(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC7(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC7(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC7_Black_Half_Alpha", "[BC7_Black_Half_Alpha]")
{
	const auto block = blocks.find("BC7_Black_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC7(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC7(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC7(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC7_Red_Blue_Half_Alpha", "[BC7_Red_Blue_Half_Alpha]")
{
	const auto block = blocks.find("BC7_Red_Blue_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC7(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC7(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC7(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC7_Red_Green_Half_Alpha", "[BC7_Red_Green_Half_Alpha]")
{
	const auto block = blocks.find("BC7_Red_Green_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC7(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC7(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC7(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC7_Green_Blue_Half_Alpha", "[BC7_Green_Blue_Half_Alpha]")
{
	const auto block = blocks.find("BC7_Green_Blue_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC7(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC7(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC7(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC7_Red_Full_Alpha", "[BC7_Red_Full_Alpha]")
{
	const auto block = blocks.find("BC7_Red_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC7(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC7(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC7(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC7_Green_Full_Alpha", "[BC7_Green_Full_Alpha]")
{
	const auto block = blocks.find("BC7_Green_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC7(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC7(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC7(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC7_Blue_Full_Alpha", "[BC7_Blue_Full_Alpha]")
{
	const auto block = blocks.find("BC7_Blue_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC7(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC7(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC7(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC7_White_Full_Alpha", "[BC7_White_Full_Alpha]")
{
	const auto block = blocks.find("BC7_White_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC7(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC7(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC7(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC7_Green_Ignore_Alpha", "[BC7_Green_Ignore_Alpha]")
{
	const auto block = blocks.find("BC7_Green_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC7(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC7(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC7(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC7_Black_Full_Alpha", "[BC7_Black_Full_Alpha]")
{
	const auto block = blocks.find("BC7_Black_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC7(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC7(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC7(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC7_Red_Blue_Full_Alpha", "[BC7_Red_Blue_Full_Alpha]")
{
	const auto block = blocks.find("BC7_Red_Blue_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC7(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC7(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC7(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC7_Red_Green_Full_Alpha", "[BC7_Red_Green_Full_Alpha]")
{
	const auto block = blocks.find("BC7_Red_Green_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC7(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC7(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC7(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC7_Green_Blue_Full_Alpha", "[BC7_Green_Blue_Full_Alpha]")
{
	const auto block = blocks.find("BC7_Green_Blue_Full_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC7(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC7(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC7(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC7_Blue_Ignore_Alpha", "[BC7_Blue_Ignore_Alpha]")
{
	const auto block = blocks.find("BC7_Blue_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC7(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC7(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC7(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC7_White_Ignore_Alpha", "[BC7_White_Ignore_Alpha]")
{
	const auto block = blocks.find("BC7_White_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC7(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC7(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC7(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC7_Black_Ignore_Alpha", "[BC7_Black_Ignore_Alpha]")
{
	const auto block = blocks.find("BC7_Black_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC7(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC7(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC7(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC7_Red_Blue_Ignore_Alpha", "[BC7_Red_Blue_Ignore_Alpha]")
{
	const auto block = blocks.find("BC7_Red_Blue_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC7(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC7(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC7(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC7_Red_Green_Ignore_Alpha", "[BC7_Red_Green_Ignore_Alpha]")
{
	const auto block = blocks.find("BC7_Red_Green_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC7(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC7(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC7(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC7_Green_Blue_Ignore_Alpha", "[BC7_Green_Blue_Ignore_Alpha]")
{
	const auto block = blocks.find("BC7_Green_Blue_Ignore_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC7(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC7(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC7(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC7_Red_Half_Alpha", "[BC7_Red_Half_Alpha]")
{
	const auto block = blocks.find("BC7_Red_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC7(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC7(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC7(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}
TEST_CASE("BC7_Green_Half_Alpha", "[BC7_Green_Half_Alpha]")
{
	const auto block = blocks.find("BC7_Green_Half_Alpha")->second;
	const auto blockData = block.data;
	const auto blockColor = block.color;
	unsigned char decompBlock[64];
	DecompressBlockBC7(blockData, decompBlock, nullptr);
	CHECK(ColorMatches(decompBlock, blockColor, false));
	unsigned char compBlock[16];
	unsigned char decompCompBlock[64];
	CompressBlockBC7(decompBlock, 16, compBlock, nullptr);
	DecompressBlockBC7(compBlock, decompCompBlock, nullptr);
	CHECK(ColorMatches(decompCompBlock, blockColor, false));
}


//***************************************************************************************