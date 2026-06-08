#pragma once

#include <filesystem>
#include <iostream>
#include <cstdint>
#include <fstream>
#include <vector>

// TODO: put all this stuff in a class to better organize things
// https://problemkaputt.de/gbatek-3ds-files-video-layout-images-clim-flim.htm
enum encoding {
    L8, // 0
    A8, // 1
    LA44, // 2
    LA88, // 3 
    HILO88, // 4
    RGB565, // 5
    RGB888, // 6
    RGBA5551, // 7
    RGBA4444, // 8
    RGBA8888, // 9
    ETC1, // A
    ETC1A4, // B
    L4, // C
    A4, // D
};

struct block {
    char magic[4] = {'i', 'm', 'a', 'g'};
    uint32_t chunksize = 0x10;
    uint16_t width = 0;
    uint16_t height = 0;
    uint8_t format = 0;
    uint8_t reserved = 0;
    uint8_t reserved1 = 0;
    uint8_t reserved2 = 0;
    uint32_t footeroffset = 0;
};

struct footer {
    char magic[4] = {'C', 'L', 'I', 'M'};
    uint8_t endianess[2] = {0xFF, 0xFE};
    uint32_t footersize = 0x14;
    uint16_t version = 0x0202;
    uint32_t filesize = 0;
    uint32_t blockcount = 1;
    block imag;
};

void write_footer(std::ofstream *out, const footer *clim);

/*
this will mess with the seek position
return:
(file size - footer size): read footer offset
0: bad, file is too small
*/
size_t read_footer(std::ifstream *file, footer *clim);

/*
read image data from clim file
this will only work with 512x256 rgb565 images because that's all I need lol
just make sure rgb565_pixels is allocated to that size (512*256*2)
return:
0: good
1: infile does not exist
2: std::filesystem error
3: extension is not .bclim
4: failed to read footer
5: footer magic is not CLIM
6: imag magic is not imag
8: image is not RGB565
9: footer offset does not match assumed offset
10: image size does not match assumed size
11: width is not 512 or height is not 256
*/
int read_clim_image(const std::string &inpath, uint8_t* rgb565_pixels);

