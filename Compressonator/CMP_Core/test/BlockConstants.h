#ifndef BLOCKCONSTANTS_H
#define BLOCKCONSTANTS_H
#include <string>
#include <unordered_map>
struct Block { const unsigned char* data; const unsigned char* color; };

static const unsigned char BC1_Red_Ignore_Alpha [] {0x0 , 0xf8, 0x0 , 0xf8, 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC1_Blue_Half_Alpha [] {0x0 , 0x0 , 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const unsigned char BC1_White_Half_Alpha [] {0x0 , 0x0 , 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const unsigned char BC1_Black_Half_Alpha [] {0x0 , 0x0 , 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const unsigned char BC1_Red_Blue_Half_Alpha [] {0x0 , 0x0 , 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const unsigned char BC1_Red_Green_Half_Alpha [] {0x0 , 0x0 , 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const unsigned char BC1_Green_Blue_Half_Alpha [] {0x0 , 0x0 , 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const unsigned char BC1_Red_Full_Alpha [] {0x0 , 0x0 , 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const unsigned char BC1_Green_Full_Alpha [] {0x0 , 0x0 , 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const unsigned char BC1_Blue_Full_Alpha [] {0x0 , 0x0 , 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const unsigned char BC1_White_Full_Alpha [] {0x0 , 0x0 , 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const unsigned char BC1_Green_Ignore_Alpha [] {0xe0, 0x7 , 0xe0, 0x7 , 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC1_Black_Full_Alpha [] {0x0 , 0x0 , 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const unsigned char BC1_Red_Blue_Full_Alpha [] {0x0 , 0x0 , 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const unsigned char BC1_Red_Green_Full_Alpha [] {0x0 , 0x0 , 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const unsigned char BC1_Green_Blue_Full_Alpha [] {0x0 , 0x0 , 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const unsigned char BC1_Blue_Ignore_Alpha [] {0x1f, 0x0 , 0x1f, 0x0 , 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC1_White_Ignore_Alpha [] {0xff, 0xff, 0xff, 0xff, 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC1_Black_Ignore_Alpha [] {0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC1_Red_Blue_Ignore_Alpha [] {0x1f, 0xf8, 0x1f, 0xf8, 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC1_Red_Green_Ignore_Alpha [] {0xe0, 0xff, 0xe0, 0xff, 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC1_Green_Blue_Ignore_Alpha [] {0xff, 0x7 , 0xff, 0x7 , 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC1_Red_Half_Alpha [] {0x0 , 0x0 , 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const unsigned char BC1_Green_Half_Alpha [] {0x0 , 0x0 , 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const unsigned char BC2_Red_Ignore_Alpha [] {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0 , 0xf8, 0x0 , 0xf8, 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC2_Blue_Half_Alpha [] {0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x1f, 0x0 , 0x1f, 0x0 , 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC2_White_Half_Alpha [] {0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0xff, 0xff, 0xff, 0xff, 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC2_Black_Half_Alpha [] {0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC2_Red_Blue_Half_Alpha [] {0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x1f, 0xf8, 0x1f, 0xf8, 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC2_Red_Green_Half_Alpha [] {0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0xe0, 0xff, 0xe0, 0xff, 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC2_Green_Blue_Half_Alpha [] {0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0xff, 0x7 , 0xff, 0x7 , 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC2_Red_Full_Alpha [] {0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0xf8, 0x0 , 0xf8, 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC2_Green_Full_Alpha [] {0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0xe0, 0x7 , 0xe0, 0x7 , 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC2_Blue_Full_Alpha [] {0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x1f, 0x0 , 0x1f, 0x0 , 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC2_White_Full_Alpha [] {0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0xff, 0xff, 0xff, 0xff, 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC2_Green_Ignore_Alpha [] {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x7 , 0xe0, 0x7 , 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC2_Black_Full_Alpha [] {0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC2_Red_Blue_Full_Alpha [] {0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x1f, 0xf8, 0x1f, 0xf8, 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC2_Red_Green_Full_Alpha [] {0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0xe0, 0xff, 0xe0, 0xff, 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC2_Green_Blue_Full_Alpha [] {0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0xff, 0x7 , 0xff, 0x7 , 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC2_Blue_Ignore_Alpha [] {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x0 , 0x1f, 0x0 , 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC2_White_Ignore_Alpha [] {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC2_Black_Ignore_Alpha [] {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC2_Red_Blue_Ignore_Alpha [] {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0xf8, 0x1f, 0xf8, 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC2_Red_Green_Ignore_Alpha [] {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xff, 0xe0, 0xff, 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC2_Green_Blue_Ignore_Alpha [] {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7 , 0xff, 0x7 , 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC2_Red_Half_Alpha [] {0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x0 , 0xf8, 0x0 , 0xf8, 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC2_Green_Half_Alpha [] {0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0xe0, 0x7 , 0xe0, 0x7 , 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC3_Red_Ignore_Alpha [] {0xff, 0xff, 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0xf8, 0x0 , 0xf8, 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC3_Blue_Half_Alpha [] {0x7b, 0x7b, 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x1f, 0x0 , 0x1f, 0x0 , 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC3_White_Half_Alpha [] {0x7b, 0x7b, 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0xff, 0xff, 0xff, 0xff, 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC3_Black_Half_Alpha [] {0x7b, 0x7b, 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC3_Red_Blue_Half_Alpha [] {0x7b, 0x7b, 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x1f, 0xf8, 0x1f, 0xf8, 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC3_Red_Green_Half_Alpha [] {0x7b, 0x7b, 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0xe0, 0xff, 0xe0, 0xff, 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC3_Green_Blue_Half_Alpha [] {0x7b, 0x7b, 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0xff, 0x7 , 0xff, 0x7 , 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC3_Red_Full_Alpha [] {0xff, 0x0 , 0x49, 0x92, 0x24, 0x49, 0x92, 0x24, 0x0 , 0xf8, 0x0 , 0xf8, 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC3_Green_Full_Alpha [] {0xff, 0x0 , 0x49, 0x92, 0x24, 0x49, 0x92, 0x24, 0xe0, 0x7 , 0xe0, 0x7 , 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC3_Blue_Full_Alpha [] {0xff, 0x0 , 0x49, 0x92, 0x24, 0x49, 0x92, 0x24, 0x1f, 0x0 , 0x1f, 0x0 , 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC3_White_Full_Alpha [] {0xff, 0x0 , 0x49, 0x92, 0x24, 0x49, 0x92, 0x24, 0xff, 0xff, 0xff, 0xff, 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC3_Green_Ignore_Alpha [] {0xff, 0xff, 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0xe0, 0x7 , 0xe0, 0x7 , 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC3_Black_Full_Alpha [] {0xff, 0x0 , 0x49, 0x92, 0x24, 0x49, 0x92, 0x24, 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC3_Red_Blue_Full_Alpha [] {0xff, 0x0 , 0x49, 0x92, 0x24, 0x49, 0x92, 0x24, 0x1f, 0xf8, 0x1f, 0xf8, 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC3_Red_Green_Full_Alpha [] {0xff, 0x0 , 0x49, 0x92, 0x24, 0x49, 0x92, 0x24, 0xe0, 0xff, 0xe0, 0xff, 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC3_Green_Blue_Full_Alpha [] {0xff, 0x0 , 0x49, 0x92, 0x24, 0x49, 0x92, 0x24, 0xff, 0x7 , 0xff, 0x7 , 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC3_Blue_Ignore_Alpha [] {0xff, 0xff, 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x1f, 0x0 , 0x1f, 0x0 , 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC3_White_Ignore_Alpha [] {0xff, 0xff, 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0xff, 0xff, 0xff, 0xff, 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC3_Black_Ignore_Alpha [] {0xff, 0xff, 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC3_Red_Blue_Ignore_Alpha [] {0xff, 0xff, 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x1f, 0xf8, 0x1f, 0xf8, 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC3_Red_Green_Ignore_Alpha [] {0xff, 0xff, 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0xe0, 0xff, 0xe0, 0xff, 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC3_Green_Blue_Ignore_Alpha [] {0xff, 0xff, 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0xff, 0x7 , 0xff, 0x7 , 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC3_Red_Half_Alpha [] {0x7b, 0x7b, 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0xf8, 0x0 , 0xf8, 0x0 , 0x0 , 0x0 , 0x0 };
static const unsigned char BC3_Green_Half_Alpha [] {0x7b, 0x7b, 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0xe0, 0x7 , 0xe0, 0x7 , 0x0 , 0x0 , 0x0 , 0x0 };

Block BC1_Red_Ignore_Alpha_Block = {BC1_Red_Ignore_Alpha, nullptr};
Block BC1_Blue_Half_Alpha_Block = {BC1_Blue_Half_Alpha, nullptr};
Block BC1_White_Half_Alpha_Block = {BC1_White_Half_Alpha, nullptr};
Block BC1_Black_Half_Alpha_Block = {BC1_Black_Half_Alpha, nullptr};
Block BC1_Red_Blue_Half_Alpha_Block = {BC1_Red_Blue_Half_Alpha, nullptr};
Block BC1_Red_Green_Half_Alpha_Block = {BC1_Red_Green_Half_Alpha, nullptr};
Block BC1_Green_Blue_Half_Alpha_Block = {BC1_Green_Blue_Half_Alpha, nullptr};
Block BC1_Red_Full_Alpha_Block = {BC1_Red_Full_Alpha, nullptr};
Block BC1_Green_Full_Alpha_Block = {BC1_Green_Full_Alpha, nullptr};
Block BC1_Blue_Full_Alpha_Block = {BC1_Blue_Full_Alpha, nullptr};
Block BC1_White_Full_Alpha_Block = {BC1_White_Full_Alpha, nullptr};
Block BC1_Green_Ignore_Alpha_Block = {BC1_Green_Ignore_Alpha, nullptr};
Block BC1_Black_Full_Alpha_Block = {BC1_Black_Full_Alpha, nullptr};
Block BC1_Red_Blue_Full_Alpha_Block = {BC1_Red_Blue_Full_Alpha, nullptr};
Block BC1_Red_Green_Full_Alpha_Block = {BC1_Red_Green_Full_Alpha, nullptr};
Block BC1_Green_Blue_Full_Alpha_Block = {BC1_Green_Blue_Full_Alpha, nullptr};
Block BC1_Blue_Ignore_Alpha_Block = {BC1_Blue_Ignore_Alpha, nullptr};
Block BC1_White_Ignore_Alpha_Block = {BC1_White_Ignore_Alpha, nullptr};
Block BC1_Black_Ignore_Alpha_Block = {BC1_Black_Ignore_Alpha, nullptr};
Block BC1_Red_Blue_Ignore_Alpha_Block = {BC1_Red_Blue_Ignore_Alpha, nullptr};
Block BC1_Red_Green_Ignore_Alpha_Block = {BC1_Red_Green_Ignore_Alpha, nullptr};
Block BC1_Green_Blue_Ignore_Alpha_Block = {BC1_Green_Blue_Ignore_Alpha, nullptr};
Block BC1_Red_Half_Alpha_Block = {BC1_Red_Half_Alpha, nullptr};
Block BC1_Green_Half_Alpha_Block = {BC1_Green_Half_Alpha, nullptr};
Block BC2_Red_Ignore_Alpha_Block = {BC2_Red_Ignore_Alpha, nullptr};
Block BC2_Blue_Half_Alpha_Block = {BC2_Blue_Half_Alpha, nullptr};
Block BC2_White_Half_Alpha_Block = {BC2_White_Half_Alpha, nullptr};
Block BC2_Black_Half_Alpha_Block = {BC2_Black_Half_Alpha, nullptr};
Block BC2_Red_Blue_Half_Alpha_Block = {BC2_Red_Blue_Half_Alpha, nullptr};
Block BC2_Red_Green_Half_Alpha_Block = {BC2_Red_Green_Half_Alpha, nullptr};
Block BC2_Green_Blue_Half_Alpha_Block = {BC2_Green_Blue_Half_Alpha, nullptr};
Block BC2_Red_Full_Alpha_Block = {BC2_Red_Full_Alpha, nullptr};
Block BC2_Green_Full_Alpha_Block = {BC2_Green_Full_Alpha, nullptr};
Block BC2_Blue_Full_Alpha_Block = {BC2_Blue_Full_Alpha, nullptr};
Block BC2_White_Full_Alpha_Block = {BC2_White_Full_Alpha, nullptr};
Block BC2_Green_Ignore_Alpha_Block = {BC2_Green_Ignore_Alpha, nullptr};
Block BC2_Black_Full_Alpha_Block = {BC2_Black_Full_Alpha, nullptr};
Block BC2_Red_Blue_Full_Alpha_Block = {BC2_Red_Blue_Full_Alpha, nullptr};
Block BC2_Red_Green_Full_Alpha_Block = {BC2_Red_Green_Full_Alpha, nullptr};
Block BC2_Green_Blue_Full_Alpha_Block = {BC2_Green_Blue_Full_Alpha, nullptr};
Block BC2_Blue_Ignore_Alpha_Block = {BC2_Blue_Ignore_Alpha, nullptr};
Block BC2_White_Ignore_Alpha_Block = {BC2_White_Ignore_Alpha, nullptr};
Block BC2_Black_Ignore_Alpha_Block = {BC2_Black_Ignore_Alpha, nullptr};
Block BC2_Red_Blue_Ignore_Alpha_Block = {BC2_Red_Blue_Ignore_Alpha, nullptr};
Block BC2_Red_Green_Ignore_Alpha_Block = {BC2_Red_Green_Ignore_Alpha, nullptr};
Block BC2_Green_Blue_Ignore_Alpha_Block = {BC2_Green_Blue_Ignore_Alpha, nullptr};
Block BC2_Red_Half_Alpha_Block = {BC2_Red_Half_Alpha, nullptr};
Block BC2_Green_Half_Alpha_Block = {BC2_Green_Half_Alpha, nullptr};
Block BC3_Red_Ignore_Alpha_Block = {BC3_Red_Ignore_Alpha, nullptr};
Block BC3_Blue_Half_Alpha_Block = {BC3_Blue_Half_Alpha, nullptr};
Block BC3_White_Half_Alpha_Block = {BC3_White_Half_Alpha, nullptr};
Block BC3_Black_Half_Alpha_Block = {BC3_Black_Half_Alpha, nullptr};
Block BC3_Red_Blue_Half_Alpha_Block = {BC3_Red_Blue_Half_Alpha, nullptr};
Block BC3_Red_Green_Half_Alpha_Block = {BC3_Red_Green_Half_Alpha, nullptr};
Block BC3_Green_Blue_Half_Alpha_Block = {BC3_Green_Blue_Half_Alpha, nullptr};
Block BC3_Red_Full_Alpha_Block = {BC3_Red_Full_Alpha, nullptr};
Block BC3_Green_Full_Alpha_Block = {BC3_Green_Full_Alpha, nullptr};
Block BC3_Blue_Full_Alpha_Block = {BC3_Blue_Full_Alpha, nullptr};
Block BC3_White_Full_Alpha_Block = {BC3_White_Full_Alpha, nullptr};
Block BC3_Green_Ignore_Alpha_Block = {BC3_Green_Ignore_Alpha, nullptr};
Block BC3_Black_Full_Alpha_Block = {BC3_Black_Full_Alpha, nullptr};
Block BC3_Red_Blue_Full_Alpha_Block = {BC3_Red_Blue_Full_Alpha, nullptr};
Block BC3_Red_Green_Full_Alpha_Block = {BC3_Red_Green_Full_Alpha, nullptr};
Block BC3_Green_Blue_Full_Alpha_Block = {BC3_Green_Blue_Full_Alpha, nullptr};
Block BC3_Blue_Ignore_Alpha_Block = {BC3_Blue_Ignore_Alpha, nullptr};
Block BC3_White_Ignore_Alpha_Block = {BC3_White_Ignore_Alpha, nullptr};
Block BC3_Black_Ignore_Alpha_Block = {BC3_Black_Ignore_Alpha, nullptr};
Block BC3_Red_Blue_Ignore_Alpha_Block = {BC3_Red_Blue_Ignore_Alpha, nullptr};
Block BC3_Red_Green_Ignore_Alpha_Block = {BC3_Red_Green_Ignore_Alpha, nullptr};
Block BC3_Green_Blue_Ignore_Alpha_Block = {BC3_Green_Blue_Ignore_Alpha, nullptr};
Block BC3_Red_Half_Alpha_Block = {BC3_Red_Half_Alpha, nullptr};
Block BC3_Green_Half_Alpha_Block = {BC3_Green_Half_Alpha, nullptr};

static std::unordered_map<std::string, Block> blocks {
	{ "BC1_Red_Ignore_Alpha", BC1_Red_Ignore_Alpha_Block},
	{ "BC1_Blue_Half_Alpha", BC1_Blue_Half_Alpha_Block},
	{ "BC1_White_Half_Alpha", BC1_White_Half_Alpha_Block},
	{ "BC1_Black_Half_Alpha", BC1_Black_Half_Alpha_Block},
	{ "BC1_Red_Blue_Half_Alpha", BC1_Red_Blue_Half_Alpha_Block},
	{ "BC1_Red_Green_Half_Alpha", BC1_Red_Green_Half_Alpha_Block},
	{ "BC1_Green_Blue_Half_Alpha", BC1_Green_Blue_Half_Alpha_Block},
	{ "BC1_Red_Full_Alpha", BC1_Red_Full_Alpha_Block},
	{ "BC1_Green_Full_Alpha", BC1_Green_Full_Alpha_Block},
	{ "BC1_Blue_Full_Alpha", BC1_Blue_Full_Alpha_Block},
	{ "BC1_White_Full_Alpha", BC1_White_Full_Alpha_Block},
	{ "BC1_Green_Ignore_Alpha", BC1_Green_Ignore_Alpha_Block},
	{ "BC1_Black_Full_Alpha", BC1_Black_Full_Alpha_Block},
	{ "BC1_Red_Blue_Full_Alpha", BC1_Red_Blue_Full_Alpha_Block},
	{ "BC1_Red_Green_Full_Alpha", BC1_Red_Green_Full_Alpha_Block},
	{ "BC1_Green_Blue_Full_Alpha", BC1_Green_Blue_Full_Alpha_Block},
	{ "BC1_Blue_Ignore_Alpha", BC1_Blue_Ignore_Alpha_Block},
	{ "BC1_White_Ignore_Alpha", BC1_White_Ignore_Alpha_Block},
	{ "BC1_Black_Ignore_Alpha", BC1_Black_Ignore_Alpha_Block},
	{ "BC1_Red_Blue_Ignore_Alpha", BC1_Red_Blue_Ignore_Alpha_Block},
	{ "BC1_Red_Green_Ignore_Alpha", BC1_Red_Green_Ignore_Alpha_Block},
	{ "BC1_Green_Blue_Ignore_Alpha", BC1_Green_Blue_Ignore_Alpha_Block},
	{ "BC1_Red_Half_Alpha", BC1_Red_Half_Alpha_Block},
	{ "BC1_Green_Half_Alpha", BC1_Green_Half_Alpha_Block},
	{ "BC2_Red_Ignore_Alpha", BC2_Red_Ignore_Alpha_Block},
	{ "BC2_Blue_Half_Alpha", BC2_Blue_Half_Alpha_Block},
	{ "BC2_White_Half_Alpha", BC2_White_Half_Alpha_Block},
	{ "BC2_Black_Half_Alpha", BC2_Black_Half_Alpha_Block},
	{ "BC2_Red_Blue_Half_Alpha", BC2_Red_Blue_Half_Alpha_Block},
	{ "BC2_Red_Green_Half_Alpha", BC2_Red_Green_Half_Alpha_Block},
	{ "BC2_Green_Blue_Half_Alpha", BC2_Green_Blue_Half_Alpha_Block},
	{ "BC2_Red_Full_Alpha", BC2_Red_Full_Alpha_Block},
	{ "BC2_Green_Full_Alpha", BC2_Green_Full_Alpha_Block},
	{ "BC2_Blue_Full_Alpha", BC2_Blue_Full_Alpha_Block},
	{ "BC2_White_Full_Alpha", BC2_White_Full_Alpha_Block},
	{ "BC2_Green_Ignore_Alpha", BC2_Green_Ignore_Alpha_Block},
	{ "BC2_Black_Full_Alpha", BC2_Black_Full_Alpha_Block},
	{ "BC2_Red_Blue_Full_Alpha", BC2_Red_Blue_Full_Alpha_Block},
	{ "BC2_Red_Green_Full_Alpha", BC2_Red_Green_Full_Alpha_Block},
	{ "BC2_Green_Blue_Full_Alpha", BC2_Green_Blue_Full_Alpha_Block},
	{ "BC2_Blue_Ignore_Alpha", BC2_Blue_Ignore_Alpha_Block},
	{ "BC2_White_Ignore_Alpha", BC2_White_Ignore_Alpha_Block},
	{ "BC2_Black_Ignore_Alpha", BC2_Black_Ignore_Alpha_Block},
	{ "BC2_Red_Blue_Ignore_Alpha", BC2_Red_Blue_Ignore_Alpha_Block},
	{ "BC2_Red_Green_Ignore_Alpha", BC2_Red_Green_Ignore_Alpha_Block},
	{ "BC2_Green_Blue_Ignore_Alpha", BC2_Green_Blue_Ignore_Alpha_Block},
	{ "BC2_Red_Half_Alpha", BC2_Red_Half_Alpha_Block},
	{ "BC2_Green_Half_Alpha", BC2_Green_Half_Alpha_Block},
	{ "BC3_Red_Ignore_Alpha", BC3_Red_Ignore_Alpha_Block},
	{ "BC3_Blue_Half_Alpha", BC3_Blue_Half_Alpha_Block},
	{ "BC3_White_Half_Alpha", BC3_White_Half_Alpha_Block},
	{ "BC3_Black_Half_Alpha", BC3_Black_Half_Alpha_Block},
	{ "BC3_Red_Blue_Half_Alpha", BC3_Red_Blue_Half_Alpha_Block},
	{ "BC3_Red_Green_Half_Alpha", BC3_Red_Green_Half_Alpha_Block},
	{ "BC3_Green_Blue_Half_Alpha", BC3_Green_Blue_Half_Alpha_Block},
	{ "BC3_Red_Full_Alpha", BC3_Red_Full_Alpha_Block},
	{ "BC3_Green_Full_Alpha", BC3_Green_Full_Alpha_Block},
	{ "BC3_Blue_Full_Alpha", BC3_Blue_Full_Alpha_Block},
	{ "BC3_White_Full_Alpha", BC3_White_Full_Alpha_Block},
	{ "BC3_Green_Ignore_Alpha", BC3_Green_Ignore_Alpha_Block},
	{ "BC3_Black_Full_Alpha", BC3_Black_Full_Alpha_Block},
	{ "BC3_Red_Blue_Full_Alpha", BC3_Red_Blue_Full_Alpha_Block},
	{ "BC3_Red_Green_Full_Alpha", BC3_Red_Green_Full_Alpha_Block},
	{ "BC3_Green_Blue_Full_Alpha", BC3_Green_Blue_Full_Alpha_Block},
	{ "BC3_Blue_Ignore_Alpha", BC3_Blue_Ignore_Alpha_Block},
	{ "BC3_White_Ignore_Alpha", BC3_White_Ignore_Alpha_Block},
	{ "BC3_Black_Ignore_Alpha", BC3_Black_Ignore_Alpha_Block},
	{ "BC3_Red_Blue_Ignore_Alpha", BC3_Red_Blue_Ignore_Alpha_Block},
	{ "BC3_Red_Green_Ignore_Alpha", BC3_Red_Green_Ignore_Alpha_Block},
	{ "BC3_Green_Blue_Ignore_Alpha", BC3_Green_Blue_Ignore_Alpha_Block},
	{ "BC3_Red_Half_Alpha", BC3_Red_Half_Alpha_Block},
	{ "BC3_Green_Half_Alpha", BC3_Green_Half_Alpha_Block}
};

#endif