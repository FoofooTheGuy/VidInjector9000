#pragma once

#include <string>
#include <cstring>
#include <vector>
#include <filesystem>
#include <nnc/utf.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_STATIC
#include "stb_image_resize.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

size_t chrcount(const std::string& str);

std::string UTF8toUTF16(const std::string input);
std::string UTF16toUTF8(const std::string& input);

std::string tolowerstr(std::string str);
std::string toupperstr(std::string str);

void crop_pixels(const uint8_t* input, int width, int height, int channels, uint8_t* output, int x_offset, int y_offset, int out_w, int out_h);
void resize_crop(const uint8_t* input_pixels, int input_w, int input_h, uint8_t* output_pixels, int output_w, int output_h, int num_channels);
void ToRGBA(const uint8_t* input, uint8_t* output, int width, int height, int channels);
void layer_pixels(uint8_t* out, uint8_t* foreground, uint8_t* background, int forewidth, int foreheight, int forechannels, int backwidth, int backheight, int backchannels, int x_offset, int y_offset);

bool stol_s(long& output, std::string input, bool isHex = false);
bool stoul_s(uint8_t& output, std::string input, bool isHex = false);
bool stoul_s(uint32_t& output, std::string input, bool isHex = false);
bool stoul_s(unsigned long& output, std::string input, bool isHex = false);

std::error_code copyfile(std::string inpath, std::string outpath);

void encode_bigend_u64(uint64_t value, void* dest);