#pragma once

// creation/extraction of cia and tar

#include <filesystem>
#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>

#include "formatting.hpp"
#include "microtar.hpp"
#include "strings.hpp"
#include "files.hpp"
#include "vi9p.hpp"

#include <nnc/swizzle.h>
#include <nnc/romfs.h>
#include <nnc/cia.h>

typedef std::string TARpath;
typedef std::string CIApath;

int writeCIA(const std::string& romfsPath, const std::string& tempPath, const std::string& productCode, const std::string& outCIA);

void writeTAR(const std::string& tempPath, const std::string& outTAR);

int extractCIA(const std::string& inArc, const std::string& outDir, const std::string& seedPath);

int extractTAR(const std::string& inArc, const std::string& outDir, VI9Pparameters* parameters);


// helpers for extracting cia
// output: string of error or empty string if it completed
std::string extract_dir(nnc_romfs_ctx* ctx, nnc_romfs_info* info, const char* path, int baselen);

/*
infile: path to seeddb file
TID: title ID to search for
outseed: array big enough to recieve the seed (0x10 pls)
return 0 if succeeded or other numbers if fail
*/
uint8_t getSeedFromTID(std::string infile, uint64_t TID, uint8_t* outseed);
