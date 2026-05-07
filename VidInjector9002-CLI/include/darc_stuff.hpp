#pragma once

#include <filesystem>
#include <iostream>

#include <darctool.hpp>
#include <darc.hpp>
#include <utf.hpp>

#include "strings.hpp"

/*extract a darc file (.arc)
inpath: path to darc file
outdir: path of new directory to create and extract the files
*/
int extract_darc(const std::string inpath, const std::string outdir);
