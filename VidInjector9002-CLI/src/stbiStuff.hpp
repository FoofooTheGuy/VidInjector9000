#pragma once

// all things stbi, since that can only be included once

#include <filesystem>
#include <iostream>

#include "formatting.hpp"
#include "strings.hpp"
#include "arrays.hpp"
#include "banner.hpp"
#include "image.hpp"

#include <nnc/swizzle.h>
#include <nnc/smdh.h>

void resize_crop(const uint8_t* input_pixels, int input_w, int input_h, uint8_t* output_pixels, int output_w, int output_h, int num_channels);

int generateBlankBanner(std::string &outfile);

// use multibanner true if you are doing multibanner
int generateBannerPreview(std::string infile, std::string outfile, bool multibanner = false);
int generateIconPreview(std::string infile, int borderMode, std::string outfile);

// return 0 if it completed otherwise return some greater number
uint8_t convertToBimg(const std::string input, uint8_t* outBuffer, bool writeHeader);

/*note: text has to be utf16
input: path to input image (size doesnt really matter)
output: icon.bin out
borderMode: 0 for no border, 1 for overlap, 2 for fit within it
*/
uint8_t convertToIcon(const std::string input, std::string output, std::string shortname, std::string longname, std::string publisher, int borderMode);
