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

#include <nnc/swizzle.h>
#include <nnc/romfs.h>
#include <nnc/cia.h>

typedef std::string TARpath;
typedef std::string CIApath;

int writeCIA(std::string& romfsPath, std::string& tempPath, std::string& productCode, std::string& outCIA);

void writeTAR(std::string& tempPath, std::string& outTAR);
