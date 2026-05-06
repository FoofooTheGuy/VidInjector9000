#pragma once

#include <cstdint>
#include <cstring>
#include <sstream>

#define MIN(a,b) (((a)<(b))?(a):(b))

// from bannertool
uint32_t lz11_get_occurence_length(uint8_t* newPtr, uint32_t newLength, uint8_t* oldPtr, uint32_t oldLength, uint32_t* disp);
void* lz11_compress(uint32_t* size, void* input, uint32_t inputSize);

uint32_t Get_Decompressed_size(const uint8_t* in);
uint32_t DecompressLZ11(const uint8_t* in, uint8_t* out);
