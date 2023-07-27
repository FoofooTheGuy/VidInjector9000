#pragma once

#include <string>
#include <cstring>
#include <vector>

#ifdef SIZE_MAX
#define _SIZE_MAX ((SIZE_MAX) >> 1)
#endif

std::string UTF8toUTF16(std::string input);
/*convert any format to rgba
(1 channel, 2 channel, 3 channel, 4 channel)
if you input 4 channel it will just memcpy it so never do that
make sure output is initialized to width * height * 4
*/
void ToRGBA(const uint8_t* input, uint8_t* output, int width, int height, int channels);
/*layers pixel array onto another
it wont resize if you put a larger image onto a smaller one, (only put a smaller image on a larger image)
larger or equal size background than forground recommended
out: 4 channel pixels the same size as the largest one*/
void layer_pixels(uint8_t* out, uint8_t* foreground, uint8_t* background, int forewidth, int foreheight, int forechannels, int backwidth, int backheight, int backchannels, int x_offset, int y_offset);
uint8_t* invert_pixels(uint8_t* input, int width, int height, int channels);

std::string tolowerstr(std::string str);
std::string toupperstr(std::string str);
bool stoul_s(uint32_t& output, std::string input, bool isHex = false);
void removeQuotes(std::string& str);
/*crops pixel array to a smaller pixel array
input: input image
width: width of input
height: height of input
channels: channels of input
output: cropped output image (pre-allocate it with out size before calling this)
x_offset: x position of the input image to start cropping from
y_offset: y position of the input image to start cropping from
out_w: width of cropped image
out_h: height of cropped image*/
void crop_pixels(const uint8_t* input, int width, int height, int channels, uint8_t* output, int x_offset, int y_offset, int out_w, int out_h);
//input a vector output an int
int getLargestNumber(std::vector<int> &nums);

uint32_t CRC32(void* pData, size_t iLen);