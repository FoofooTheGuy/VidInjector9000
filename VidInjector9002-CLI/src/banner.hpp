#pragma once

#include <filesystem>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>

//these are from bannertool with slight edits
#define CBMD_NUM_CGFXS 14

typedef struct {
	char magic[4];
	uint32_t zero;
	uint32_t cgfxOffsets[CBMD_NUM_CGFXS];
	uint8_t padding[0x44];
	uint32_t cwavOffset;
} CBMDHeader;

typedef struct {
	void* cgfxs[CBMD_NUM_CGFXS];
	uint32_t cgfxSizes[CBMD_NUM_CGFXS];
	void* cwav;
	uint32_t cwavSize;
} CBMD;

#define MIN(a,b) (((a)<(b))?(a):(b))

uint32_t lz11_get_occurence_length(uint8_t* newPtr, uint32_t newLength, uint8_t* oldPtr, uint32_t oldLength, uint32_t* disp);
void* lz11_compress(uint32_t* size, void* input, uint32_t inputSize);
void* cbmd_build_data(uint32_t* size, CBMD cbmd);

uint32_t Get_Decompressed_size(const uint8_t* in);

uint32_t DecompressLZ11(const uint8_t* in, uint8_t* out);

/*
CGFX: array containing the uncompressed CGFX
symbol: name of the texture you want to get info of
height, width, mipmap, formatID, size: information extracted from the cgfx
*/
uint8_t getCGFXtextureInfo(uint8_t* CGFX, const std::string symbol, uint32_t& dataOffset, uint32_t& height, uint32_t& width, uint32_t& mipmap, uint32_t& formatID, uint32_t& size);
/*
inpath: path to cbmd
*/
uint8_t getCBMDInfo(const std::string inpath, uint32_t* compressedSize, uint32_t* decompressedSize, uint32_t* CGFXoffset);
/*

*/
uint8_t CBMDgetCommonCGFX(const std::string inpath, const uint32_t compressedSize, const uint32_t CGFXoffset, uint8_t* outbuff);
/*
inpath: string path to file
symbol: name of the texture you want
outbuff: output buffer for image data, make sure it is the correct size for the image you want
return true if succeed, false if fail
*/
uint8_t getCBMDTexture(const std::string inpath, const std::string symbol, uint8_t* outbuff);
