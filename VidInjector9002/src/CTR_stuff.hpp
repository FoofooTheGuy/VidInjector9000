//this will have stuff that is for the 3ds like the romfs/exefs creation and everything in between

#pragma once
#include <filesystem>
#include <fstream>
#include <chrono>
#include <random>

#include "files.hpp"
#include "zip_file.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_STATIC
#include "stb_image_resize.h"

//only put the name of the dir (not / at the end)
void Generate_Files(std::string dir, bool Multi);

bool TIDisValid(unsigned long TID);

unsigned long RandomTID();

bool convertToBimg(std::string input, unsigned char* outBuffer, bool writeHeader);
//note: text has to be utf16
//input: path to input image (size doesnt really matter)
//output: icon.bin out
bool convertToIcon(std::string input, std::string output, std::string shortname, std::string longname, std::string publisher);

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