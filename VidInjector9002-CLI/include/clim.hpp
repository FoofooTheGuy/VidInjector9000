#pragma once

#include <cstdint>

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

struct chunk {
    char imag[4];
    uint32_t chunksize;
    uint16_t width;
    uint16_t height;
    uint8_t format;
    uint8_t reserved = 0;
    uint8_t reserved1 = 0;
    uint8_t reserved2 = 0;
    uint32_t footeroffset;
};

struct footer {
    char clim[4];
    uint8_t endianess[2];
    uint32_t footersize;
    uint16_t version;
    uint32_t filesize;
    uint32_t blockcount;
    chunk imag;
};
