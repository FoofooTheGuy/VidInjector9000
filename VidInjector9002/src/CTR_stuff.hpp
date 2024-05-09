//this will have stuff that is for the 3ds like the romfs/exefs creation and everything in between

#pragma once
#include <filesystem>
#include <fstream>
#include <chrono>
#include <random>

#include "images.hpp"
#include "arrays.h"
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
#include "nnc/nnc/romfs.h"
#include "nnc/nnc/smdh.h"

//only put the name of the dir (not / at the end)
std::error_code Generate_Files(std::string dir, bool Multi);

bool TIDisValid(uint32_t TID);

uint32_t RandomTID();

//resize image and maintain aspect ratio and also crop it
void resize_crop(const uint8_t* input_pixels, int input_w, int input_h, uint8_t* output_pixels, int output_w, int output_h, int num_channels);

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

/*//https://www.3dbrew.org/wiki/CGFX
struct CGFXHeader {
	char Magic[4];//Magic "CGFX"
	uint16_t BOM;//Byte order mark: FFFE (little endian) or FEFF (big endian)
	uint16_t Header_size;//CGFX header size
	uint32_t Revision;
	uint32_t File_size;//File size (bytes)
	uint32_t Entries;//Number of entries
};

struct DICTReference {
	uint32_t DICT_entry;//Number of entries in DICT N
	uint32_t DICT_offset;//Offset (self-relative) to DICT N
};

struct DATA {
	char Magic[4];//Magic "DATA"
	uint32_t DATA_size;//DATA Size (in bytes)
	DICTReference* DICTrefs[15];//DATA contains a list of DICT references.
};

struct DICTHeader {
	char Magic[4];//Magic "DICT"
	uint32_t DICT_size;//DICT size (in bytes)
	uint32_t Entries;//Number of entries
	uint32_t Unk1;//?
	uint16_t Unk2;//Unknown. Seems to be shifted left by 4 bits in the source.
	uint8_t* Unk3[0xA];//?
};

struct DICTEntry {
	uint32_t Unk1;//?
	uint16_t Unk2;//?
	uint16_t Unk3;//?
	uint32_t Symbol_offset;//Offset (self-relative) to symbol
	uint32_t Object_offset;//Offset (self-relative) to object
};

struct CMDL {
	uint32_t Flags;//Flags (bit 7: hasSkeletonSobj)
	char Magic[4];//Magic "CMDL"
	uint32_t Unk1;//?
	uint32_t Name_offset;//Offset (self-relative) to model name
	uint8_t* Unk2[0x18];//?
	uint32_t AnimTypesDICT_entries;//Number of entries in Animation Types DICT
	uint32_t AnimTypesDICT_offset;//Offset (self-relative) to Animation Types DICT
	float* Global_scale[3];//Global scale vector (3 floats : x, y, z)
	uint8_t* Unk3[0x18];//?
	uint8_t* Matrix1[0x30];//Matrix 1//this is probably 0xC floats but im not even gonna use this so meh
	uint8_t* Matrix2[0x30];//Matrix 2//^
	uint32_t VSOBJinfo_entries1;//Number of Vertex Info SOBJ entries
	uint32_t VSOBJinfo_offset1;//Offset (self-relative) to Vertex Info SOBJ list
	uint32_t MTOBDICT_entries;//Number of MTOB DICT entries
	uint32_t MTOBDICT_offset;//Offset (self-relative) to MTOB DICT
	uint32_t VSOBJinfo_entries2;//Number of Vertex Info SOBJ entries
	uint32_t VSOBJinfo_offset2;//Offset (self-relative) to Vertex Info SOBJ list
	uint32_t UnkDICT_entries;//Number of Unknown DICT entries
	uint32_t UnkDICT_offset;//Offset (self-relative) to Unknown DICT
	uint8_t* Unk4[0xC];//?
	uint32_t SSOBJinfo_offset;//Skeleton Info SOBJ offset (self-relative) [only present if flag bit 7 is set]
	//I stopped here because I dont know what they mean by "only present" like is it just 00 or is it not there at all? I need to find an example with bit 7 unset
};*/


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
uint8_t CBMDgetCommonCGFX(const std::string inpath, const uint32_t compressedSize, const uint32_t decompressedSize, const uint32_t CGFXoffset, uint8_t* outbuff);
/*
inpath: string path to file
symbol: name of the texture you want
outbuff: output buffer for image data, make sure it is the correct size for the image you want
return true if succeed, false if fail
*/
uint8_t getCBMDTexture(const std::string inpath, const std::string symbol, uint8_t* outbuff);

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