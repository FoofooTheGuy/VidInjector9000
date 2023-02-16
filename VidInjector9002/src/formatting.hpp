#pragma once

#include <string>
#include <cstring>

#ifdef SIZE_MAX
#define _SIZE_MAX ((SIZE_MAX) >> 1)
#endif

std::string UTF8toUTF16(std::string input);

/*layers pixel array onto another
it wont resize if you put a larger image onto a smaller one
larger or equal size background than forground recommended
out: 4 channel pixels the same size as the largest one*/
void layer_pixels(unsigned char* out, unsigned char* foreground, unsigned char* background, int forewidth, int foreheight, int forechannels, int backwidth, int backheight, int backchannels, int x_offset, int y_offset);
unsigned char* invert_pixels(unsigned char* input, int width, int height, int channels);

std::string tolowerstr(std::string str);
std::string toupperstr(std::string str);
bool stoul_s(unsigned long& output, std::string input, bool isHex = false);
void removeQuotes(std::string& str);
