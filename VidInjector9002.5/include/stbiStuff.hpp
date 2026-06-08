#pragma once

// all things stbi, since that can only be included once

#include <filesystem>
#include <iostream>

#include "formatting.hpp"
#include "strings.hpp"
#include "banner.hpp"
#include "image.hpp"
#include "clim.hpp"

#include <nnc/swizzle.h>
#include <nnc/smdh.h>

#include <icon_border24_bin.hpp>
#include <icon_border48_bin.hpp>
#include <film_overlay_bin.hpp>
#include <bimgheader_bin.hpp>
#include <top_image_png.hpp>


/*
basically the same as crop_pixels
just make sure every image has the same amount of channels (num_channels)
*/
void resize_crop(const uint8_t* input_pixels, int input_w, int input_h, uint8_t* output_pixels, int output_w, int output_h, int num_channels);

/*
read any supported image with stb and convert to rgb565
inset_w and inset_h are the sizes of smaller image inside of the actual image (for alignment). set these to 0 if you dont need it aligned i.e. for smdh
new_w and new_h can be whatever, just make sure the size of rgb565_pixels is width * height * sizeof(uint16_t)
force: This means don't return if the input is unreadable, just push forward with a blank white image as input
*/
uint8_t stbiToRGB565(const std::string &infile, uint8_t* rgb565_pixels, int inset_w, int inset_h, const int &new_w, const int &new_h, const bool &force);

int generateBlankBanner(const std::string &outfile);

// use multibanner true if you are doing multibanner
int generateBannerPreview(std::string infile, std::string outfile, bool multibanner = false);
int generateIconPreview(std::string infile, int borderMode, std::string outfile);

int generateBlankClim(const std::string &outfile);
int generateClimPreview(const std::string &infile, std::string outfile);

// return 0 if it completed otherwise return some greater number
uint8_t convertToBimg(const std::string input, uint8_t* outBuffer, bool writeHeader);

/*bare bones SMDH creation. thanks 3dbrew
(note: text has to be utf16)
input: path to input image (size doesnt really matter)
output: icon.bin out
borderMode: 0 for no border, 1 for overlap, 2 for fit within it
*/
uint8_t convertToIcon(const std::string input, std::string output, std::string shortname, std::string longname, std::string publisher, int borderMode);

/*make clim (for custom Title_rogo)
input: path of image file
output: Title_Rogo.bclim out
output image will be rgb565 and resized to 400x240
*/
uint8_t convertToClim(const std::string input, const std::string output);
