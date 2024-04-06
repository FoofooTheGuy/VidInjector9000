#pragma once

//this will have stuff that is for the 3ds like the romfs/exefs creation and everything in between

#include <filesystem>
#include <fstream>
#include <chrono>
#include <random>

#include "microtar.hpp"
#include "zip_file.hpp"
#include "strings.hpp"
#include "arrays.hpp"
#include "files.hpp"
#include "vi9p.hpp"

#include <nnc/swizzle.h>
#include <nnc/romfs.h>
#include <nnc/smdh.h>
#include <nnc/cia.h>

//only put the name of the dir (not / at the end)
std::error_code Generate_Files(std::string dir, bool Multi);

bool TIDisValid(uint32_t TID);

uint32_t RandomTID();

//return 0 if it completed otherwise return some greater number
uint8_t convertToBimg(const std::string input, uint8_t* outBuffer, bool writeHeader);
/*note: text has to be utf16
input: path to input image (size doesnt really matter)
output: icon.bin out
borderMode: 0 for no border, 1 for overlap, 2 for fit within it
*/
uint8_t convertToIcon(const std::string input, std::string output, std::string shortname, std::string longname, std::string publisher, int borderMode);

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

int build_archive(std::string inVi9p, std::string outCIA, std::string outTAR = "", uint32_t uniqueID = RandomTID(), std::string ApplicationName = "video", std::string ProductCode = "VDIJ");

int extract_archive(std::string inArc, std::string outDir, bool dopatch = false, std::string seedpath = "");

/*path: input path to text file containing utf16 text
outVec: output vector containing each line per element
trim: true for remove lines that start with '#' and empty lines, false for write all lines to outVec
return 0 if succeeded or other numbers for fail*/
uint8_t UTF16fileToUTF8str(const std::string path, std::vector<std::string>* outVec);

//output: string of error or empty string if it completed
std::string extract_dir(nnc_romfs_ctx* ctx, nnc_romfs_info* info, const char* path, int baselen);

/*infile: path to seeddb file
TID: title ID to search for
outseed: array big enough to recieve the seed (0x10 pls)
return 0 if succeeded or other numbers if fail*/
uint8_t getSeedFromTID(std::string infile, uint64_t TID, uint8_t* outseed);