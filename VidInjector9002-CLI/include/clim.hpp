#pragma once

#include <cstdint>
#include <vector>
#include <fstream>

// this should probably be a own class
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
    uint16_t width;
    uint16_t height;
    uint8_t format;
    uint8_t reserved = 0;
    uint8_t reserved1 = 0;
    uint8_t reserved2 = 0;
    uint32_t footeroffset;
};

struct footer {
    char magic[4] = {'C', 'L', 'I', 'M'};
    uint8_t endianess[2] = {0xFF, 0xFE};
    uint32_t footersize = 0x14;
    uint16_t version = 0x0202;
    uint32_t filesize;
    uint32_t blockcount = 1;
    block imag;
};

void write_footer(std::ofstream *out, const footer *clim);
