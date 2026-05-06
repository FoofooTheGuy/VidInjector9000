#include "lz.hpp"

// Ported from: https://github.com/svn2github/3DS-Explorer/blob/master/3DSExplorer/DSDecmp/Formats/Nitro/LZ11.cs

uint32_t lz11_get_occurence_length(uint8_t* newPtr, uint32_t newLength, uint8_t* oldPtr, uint32_t oldLength, uint32_t* disp) {
	if (disp != NULL) {
		*disp = 0;
	}

	if (newLength == 0) {
		return 0;
	}

	uint32_t maxLength = 0;
	if (oldLength > 0) {
		for (uint32_t i = 0; i < oldLength - 1; i++) {
			uint8_t* currentOldStart = oldPtr + i;
			uint32_t currentLength = 0;
			for (uint32_t j = 0; j < newLength; j++) {
				if (*(currentOldStart + j) != *(newPtr + j)) {
					break;
				}

				currentLength++;
			}

			if (currentLength > maxLength) {
				maxLength = currentLength;
				if (disp != NULL) {
					*disp = oldLength - i;
				}

				if (maxLength == newLength) {
					break;
				}
			}
		}
	}

	return maxLength;
}

void* lz11_compress(uint32_t* size, void* input, uint32_t inputSize) {
	if (inputSize > 0xFFFFFF) {
		return NULL;
	}

	std::stringstream ss;

	uint8_t header[4] = { 0x11, (uint8_t)(inputSize & 0xFF), (uint8_t)((inputSize >> 8) & 0xFF), (uint8_t)((inputSize >> 16) & 0xFF) };
	ss.write((char*)header, 4);

	uint32_t compressedLength = 4;
	uint8_t outbuffer[8 * 4 + 1];
	outbuffer[0] = 0;
	uint32_t bufferlength = 1;
	uint32_t bufferedBlocks = 0;
	uint32_t readBytes = 0;
	while (readBytes < inputSize) {
		if (bufferedBlocks == 8) {
			ss.write((char*)outbuffer, bufferlength);
			compressedLength += bufferlength;
			outbuffer[0] = 0;
			bufferlength = 1;
			bufferedBlocks = 0;
		}

		uint32_t disp = 0;
		uint32_t oldLength = MIN(readBytes, 0x1000);
		uint32_t length = lz11_get_occurence_length((uint8_t*)input + readBytes, MIN(inputSize - readBytes, 0x10110), (uint8_t*)input + readBytes - oldLength, oldLength, &disp);
		if (length < 3) {
			outbuffer[bufferlength++] = *((uint8_t*)input + (readBytes++));
		}
		else {
			readBytes += length;
			outbuffer[0] |= (uint8_t)(1 << (7 - bufferedBlocks));
			if (length > 0x110) {
				outbuffer[bufferlength] = 0x10;
				outbuffer[bufferlength] |= (uint8_t)(((length - 0x111) >> 12) & 0x0F);
				bufferlength++;
				outbuffer[bufferlength] = (uint8_t)(((length - 0x111) >> 4) & 0xFF);
				bufferlength++;
				outbuffer[bufferlength] = (uint8_t)(((length - 0x111) << 4) & 0xF0);
			}
			else if (length > 0x10) {
				outbuffer[bufferlength] = 0x00;
				outbuffer[bufferlength] |= (uint8_t)(((length - 0x111) >> 4) & 0x0F);
				bufferlength++;
				outbuffer[bufferlength] = (uint8_t)(((length - 0x111) << 4) & 0xF0);
			}
			else {
				outbuffer[bufferlength] = (uint8_t)(((length - 1) << 4) & 0xF0);
			}

			outbuffer[bufferlength] |= (uint8_t)(((disp - 1) >> 8) & 0x0F);
			bufferlength++;
			outbuffer[bufferlength] = (uint8_t)((disp - 1) & 0xFF);
			bufferlength++;
		}

		bufferedBlocks++;
	}

	if (bufferedBlocks > 0) {
		ss.write((char*)outbuffer, bufferlength);
		compressedLength += bufferlength;
	}

	if (compressedLength % 4 != 0) {
		uint32_t padLength = 4 - (compressedLength % 4);
		uint8_t* pad = new uint8_t[padLength];
		memset(pad, 0, (size_t)padLength);

		ss.write((char*)pad, padLength);
		compressedLength += padLength;
		delete[] pad;
	}

	void* buf = malloc((size_t)compressedLength);
	ss.read((char*)buf, compressedLength);

	if (size != NULL) {
		*size = (uint32_t)compressedLength;
	}

	return buf;
}

uint32_t Get_Decompressed_size(const uint8_t* in) {
	uint32_t decompressed_size;
	memcpy(&decompressed_size, in, sizeof(uint32_t));
	in += 4;

	uint8_t type = decompressed_size & 0xFF;
	if (type != 0x11) return 0xFFFFFFFF;
	decompressed_size >>= 8;
	return decompressed_size;
}

// https://github.com/citra-emu/citra/blob/bfb6a5b5de1e7a89ceebabe33e792d5c03a2cf46/src/core/hle/service/apt/apt.cpp#L135
uint32_t DecompressLZ11(const uint8_t* in, uint8_t* out) {
	uint32_t decompressed_size = Get_Decompressed_size(in);

	uint32_t current_out_size = 0;
	uint8_t flags = 0, mask = 1;
	while (current_out_size < decompressed_size) {
		if (mask == 1) {
			flags = *(in++);
			mask = 0x80;
		}
		else {
			mask >>= 1;
		}

		if (flags & mask) {
			uint8_t byte1 = *(in++);
			uint32_t length = byte1 >> 4;
			uint32_t offset;
			if (length == 0) {
				uint8_t byte2 = *(in++);
				uint8_t byte3 = *(in++);
				length = (((byte1 & 0x0F) << 4) | (byte2 >> 4)) + 0x11;
				offset = (((byte2 & 0x0F) << 8) | byte3) + 0x1;
			}
			else if (length == 1) {
				uint8_t byte2 = *(in++);
				uint8_t byte3 = *(in++);
				uint8_t byte4 = *(in++);
				length = (((byte1 & 0x0F) << 12) | (byte2 << 4) | (byte3 >> 4)) + 0x111;
				offset = (((byte3 & 0x0F) << 8) | byte4) + 0x1;
			}
			else {
				uint8_t byte2 = *(in++);
				length = (byte1 >> 4) + 0x1;
				offset = (((byte1 & 0x0F) << 8) | byte2) + 0x1;
			}

			for (uint32_t i = 0; i < length; i++) {
				*out = *(out - offset);
				++out;
			}

			current_out_size += length;
		}
		else {
			*(out++) = *(in++);
			current_out_size++;
		}
	}
	return decompressed_size;
}
