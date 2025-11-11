#pragma once

// this will have stuff that is for the 3ds like the romfs/exefs creation and everything in between

#include <filesystem>
#include <fstream>
#include <chrono>
#include <random>

#include "stbiStuff.hpp"
#include "microtar.hpp"
#include "zip_file.hpp"
#include "files.hpp"
#include "vi9p.hpp"

#include <nnc/swizzle.h>
#include <nnc/romfs.h>
#include <nnc/cia.h>

// formerly in image.hpp

/*this only exists for generating a preview of what the banner or icon could look like
parampath: vi9p file
number: index of which thing to generate (do -pp to see list)
outpath: output .png file.
return values:
17: custom banner, (cant make a preview)
28, 29, 16, 44, 45, 18, 46, 47, 24: failed to read input (banner)
30, 31, 32: failed to read input (icon)
ect.*/
int generate_preview(std::string parampath, uint8_t number, std::string outpath);


// only put the name of the dir (not / at the end)
std::error_code Generate_Files(std::string dir, bool Multi);

bool TIDisValid(uint32_t TID);

uint32_t RandomTID();

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
