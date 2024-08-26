#pragma once

#include <string>
#include <vector>
#include <cstring>
#include <fstream>
#include "formatting.hpp"
#include "CTR_stuff.hpp"
#include "vi9p.hpp"

#include <nnc/swizzle.h>

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
void layer_pixels(uint8_t* out, const uint8_t* foreground, const uint8_t* background, int forewidth, int foreheight, int forechannels, int backwidth, int backheight, int backchannels, int x_offset, int y_offset);

/*this only exists for generating a preview of what the banner or icon could look like
parampath: vi9p file
number: index of which thing to generate (do -pp to see list)
outpath: output .png file.
return values:
17: custom banner, (cant make a preview)
28, 29, 16, 44, 45, 18, 46, 47, 24: failed to read input (banner)
30, 31, 32: failed to read input (icon)
ect.*/
int generate_preview(std::string parampath, int number, std::string outpath);