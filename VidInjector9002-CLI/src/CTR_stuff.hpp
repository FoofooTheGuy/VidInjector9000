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
#include "image.hpp"
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

/*
gets the short name, long name, and publisher from an smdh.
returns: false for fail (not an smdh?) true for complete
*/
int setIcon(std::string inpath, std::string &Sname, std::string &Lname, std::string &Publisher);

/*
this is here because this file already includes vi9p.hpp
inpath: in vi9p
Newicon: path to smdh
outpath: out vi9p
return:
0 good
2 failed to load (loadParameters)
3 failed to read param (loadParameters)
4 Bad version (loadParameters)
5 failed to find StrVerParam (loadParameters)
6 failed to load something from parameters (loadParameters)
7 failed to open file
8 not an smdh
*/
int SetSMDH(std::string inpath, std::string Newicon, std::string outpath);
/*
return:
0 good
2 failed to load (loadParameters)
3 failed to read param (loadParameters)
4 Bad version (loadParameters)
5 failed to find StrVerParam (loadParameters)
6 failed to load something from parameters (loadParameters)
7 temppath error
8, 17, 23 failed to create movie dir
9 failed to create movie_title.csv
10, 12, 24 failed to create settings dir
11 failed to create settingsTL.csv
13 failed to create information_buttons.csv
14 failed to create copyright.txt
15 failed to find moflex to copy
16 input for moflex is not moflex
18, 19, 20, 21 failed to copy moflex
22 failed to create bimg
25 failed to create movie_bnrname.csv
26 failed to create icon
27, 28 failed to copy icon
29 failed to copy banner
30 failed to convert banner image
31 failed to create banner
NNC errors ?
*/
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
