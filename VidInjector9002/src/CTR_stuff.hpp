//this will have stuff that is for the 3ds like the romfs/exefs creation and everything in between

#pragma once
#include <filesystem>
#include <fstream>
#include <chrono>
#include <random>

#include "images.h"
#include "files.hpp"
#include "formatting.hpp"
#include "zip_file.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_STATIC
#include "stb_image_resize.h"
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include "stb_image_write.h"

#include "nnc/nnc/swizzle.h"

//only put the name of the dir (not / at the end)
void Generate_Files(std::string dir, bool Multi);

bool TIDisValid(unsigned long TID);

unsigned long RandomTID();

//resize image and maintain aspect ratio and also crop it
void resize_crop(const unsigned char* input_pixels, int input_w, int input_h, unsigned char* output_pixels, int output_w, int output_h, int num_channels);

bool convertToBimg(std::string input, unsigned char* outBuffer, bool writeHeader);
/*note: text has to be utf16
input: path to input image (size doesnt really matter)
output: icon.bin out
borderMode: 0 for no border, 1 for overlap, 2 for fit within it
*/
bool convertToIcon(std::string input, std::string output, std::string shortname, std::string longname, std::string publisher, int borderMode);

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