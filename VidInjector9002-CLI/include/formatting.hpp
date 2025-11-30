#pragma once

#include <string>
#include <vector>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <nnc/utf.h>

#include "microtar.hpp"

size_t chrcount(const std::string& str);

std::string UTF8toUTF16(const std::string input);
std::string UTF16toUTF8(const std::string& input);
std::string to_UTF8(const nnc_u16* UTF16, const size_t UTF16size);

std::string tolowerstr(std::string str);
std::string toupperstr(std::string str);

bool stol_s(long& output, std::string input, bool isHex = false);
bool stoul_s(uint8_t& output, std::string input, bool isHex = false);
bool stoul_s(uint32_t& output, std::string input, bool isHex = false);
bool stoul_s(unsigned long& output, std::string input, bool isHex = false);

uint32_t CRC32(void* pData, size_t iLen);

std::error_code copyfile(std::string inpath, std::string outpath);

void encode_bigend_u64(uint64_t value, void* dest);

std::string fixSlashes(std::string instr);

/*
memory efficient tar file adding
tar: mtar tar thing. be sure to call mtar_open before doing this
filename: input file path to add
arcname: filename to write to the tar archive. must only be a file name.
buffersize: size of chunk to read from the file at a time.
return value: std::error_code probably from std::filesystem
*/
std::error_code add_file(mtar_t* tar, std::string filename, std::string arcname, size_t buffersize);

/*
add all of a directory to a tar using
tar: mtar tar thing. be sure to call mtar_open before doing this
dirname: directory to put in the tar, must be an absolute path.
buffersize: size of chunk to read from the file at a time.
return value: std::error_code probably from std::filesystem
*/
std::error_code add_directory(mtar_t* tar, std::string dirname, size_t buffersize);

/*
extract all records from a tar
tar: mtar tar thing. be sure to call mtar_open before doing this
h: mtar tar header. so it knows which file to get
inputfile: file path of the tar
outputdir: directory to extract to
buffersize: size of chunk to read from the file at a time.
return value: int from mtar, use mtar_strerror to see what it means
*/
int read_record_data(mtar_t* tar, mtar_header_t* h, std::string inputfile, std::string outputdir, size_t buffersize);

/*
extract all records from a tar
tar: mtar tar thing. be sure to call mtar_open before doing this
inputfile: file path of the tar
outputdir: directory to extract to
buffersize: size of chunk to read from the file at a time.
return value: int from mtar, use mtar_strerror to see what it means
*/
int extract_content(mtar_t* tar, std::string inputfile, std::string outputdir, size_t buffersize);
