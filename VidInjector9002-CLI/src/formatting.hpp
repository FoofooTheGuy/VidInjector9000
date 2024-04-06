#pragma once

#include <string>
#include <vector>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <nnc/utf.h>

#include "microtar.hpp"
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
std::string to_UTF8(const nnc_u16* UTF16, const size_t UTF16size);

std::string tolowerstr(std::string str);
std::string toupperstr(std::string str);

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
void resize_crop(const uint8_t* input_pixels, int input_w, int input_h, uint8_t* output_pixels, int output_w, int output_h, int num_channels);
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

bool stol_s(long& output, std::string input, bool isHex = false);
bool stoul_s(uint8_t& output, std::string input, bool isHex = false);
bool stoul_s(uint32_t& output, std::string input, bool isHex = false);
bool stoul_s(unsigned long& output, std::string input, bool isHex = false);

std::error_code copyfile(std::string inpath, std::string outpath);

void encode_bigend_u64(uint64_t value, void* dest);

std::string fixSlashes(std::string instr);

/*memory efficient tar file adding
tar: mtar tar thing. be sure to call mtar_open before doing this
filename: input file path to add
arcname: filename to write to the tar archive. must only be a file name.
buffersize: size of chunk to read from the file at a time.
return value: std::error_code probably from std::filesystem
*/
std::error_code add_file(mtar_t* tar, std::string filename, std::string arcname, size_t buffersize);

/*add all of a directory to a tar using
tar: mtar tar thing. be sure to call mtar_open before doing this
dirname: directory to put in the tar, must be an absolute path.
buffersize: size of chunk to read from the file at a time.
return value: std::error_code probably from std::filesystem
*/
std::error_code add_directory(mtar_t* tar, std::string dirname, size_t buffersize);

/*extract all records from a tar
tar: mtar tar thing. be sure to call mtar_open before doing this
h: mtar tar header. so it knows which file to get
inputfile: file path of the tar
outputdir: directory to extract to
buffersize: size of chunk to read from the file at a time.
return value: int from mtar, use mtar_strerror to see what it means
*/
int read_record_data(mtar_t* tar, mtar_header_t* h, std::string inputfile, std::string outputdir, size_t buffersize);

/*extract all records from a tar
tar: mtar tar thing. be sure to call mtar_open before doing this
inputfile: file path of the tar
outputdir: directory to extract to
buffersize: size of chunk to read from the file at a time.
return value: int from mtar, use mtar_strerror to see what it means
*/
int extract_content(mtar_t* tar, std::string inputfile, std::string outputdir, size_t buffersize);