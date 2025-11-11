#include "banner.hpp"

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

void* cbmd_build_data(uint32_t* size, CBMD cbmd) {//this is here because it has to be static for some reason (it's from bannertool)
	CBMDHeader header;
	memset(&header, 0, sizeof(header));

	memcpy(header.magic, "CBMD", sizeof(header.magic));

	uint32_t outputSize = sizeof(CBMDHeader);

	void* compressedCGFXs[14] = { NULL };
	uint32_t compressedCGFXSizes[14] = { 0 };
	for (uint32_t i = 0; i < CBMD_NUM_CGFXS; i++) {
		if (cbmd.cgfxs[i] != NULL) {
			header.cgfxOffsets[i] = outputSize;

			compressedCGFXs[i] = lz11_compress(&compressedCGFXSizes[i], cbmd.cgfxs[i], cbmd.cgfxSizes[i]);
			outputSize += compressedCGFXSizes[i];
		}
	}

	outputSize = (outputSize + 0xF) & ~0xF;

	if (cbmd.cwav != NULL) {
		header.cwavOffset = outputSize;

		outputSize += cbmd.cwavSize;
	}

	void* output = calloc(outputSize, sizeof(uint8_t));
	if (output == NULL) {
		for (uint32_t i = 0; i < CBMD_NUM_CGFXS; i++) {
			if (cbmd.cgfxs[i] != NULL) {
				free(compressedCGFXs[i]);
			}
		}
		return NULL;
	}

	memcpy(output, &header, sizeof(header));

	for (uint32_t i = 0; i < CBMD_NUM_CGFXS; i++) {
		if (compressedCGFXs[i] != NULL) {
			memcpy(&((uint8_t*)output)[header.cgfxOffsets[i]], compressedCGFXs[i], compressedCGFXSizes[i]);
			free(compressedCGFXs[i]);
		}
	}

	if (cbmd.cwav != NULL) {
		memcpy(&((uint8_t*)output)[header.cwavOffset], cbmd.cwav, cbmd.cwavSize);
	}

	if (size != NULL) {
		*size = outputSize;
	}

	return output;
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

uint32_t DecompressLZ11(const uint8_t* in, uint8_t* out) {//https://github.com/citra-emu/citra/blob/bfb6a5b5de1e7a89ceebabe33e792d5c03a2cf46/src/core/hle/service/apt/apt.cpp#L135
	uint32_t decompressed_size;
	memcpy(&decompressed_size, in, sizeof(uint32_t));
	in += 4;

	uint8_t type = decompressed_size & 0xFF;
	if (type != 0x11) return 0xFFFFFFFF;
	decompressed_size >>= 8;

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

uint8_t getCGFXtextureInfo(uint8_t* CGFX, const std::string symbol, uint32_t& dataOffset, uint32_t& height, uint32_t& width, uint32_t& mipmap, uint32_t& formatID, uint32_t& size) {
	uint32_t* CGFXmagic = reinterpret_cast<uint32_t*>(&CGFX[0]);
	if (*CGFXmagic != 0x58464743) {//CGFX
		return 1;
	}

	uint16_t* CGFXheaderSize = reinterpret_cast<uint16_t*>(&CGFX[0x6]);
	for (uint32_t N = 0; N < *reinterpret_cast<uint32_t*>(&CGFX[0x10]); N++) {//loop through Number of Dicts
		uint32_t DICToffset = *reinterpret_cast<uint32_t*>(&CGFX[*CGFXheaderSize + 0xC + (N * 8)]);
		DICToffset += (*CGFXheaderSize + 0xC + (N * 8));//since it's self-relative, do this
		for (uint32_t i = 0; i < *reinterpret_cast<uint32_t*>(&CGFX[DICToffset + 0x8]); i++) {//loop through entries in DICT N
			uint32_t symbolOffset = *reinterpret_cast<uint32_t*>(&CGFX[DICToffset + 0x1C + (0x10 * i) + 0x8]);
			symbolOffset += (DICToffset + 0x1C + (0x10 * i) + 0x8);//self-relative
			uint32_t objectOffset = *reinterpret_cast<uint32_t*>(&CGFX[DICToffset + 0x1C + (0x10 * i) + 0xC]);
			objectOffset += (DICToffset + 0x1C + (0x10 * i) + 0xC);//self-relative
			std::string isymbol = "";
			const auto* ch = &CGFX[symbolOffset];
			while (*ch)
				isymbol += *ch++;
			if (strcmp(isymbol.c_str(), symbol.c_str()) == 0) {
				height = *reinterpret_cast<uint32_t*>(&CGFX[objectOffset + 0x18]);
				width = *reinterpret_cast<uint32_t*>(&CGFX[objectOffset + 0x1C]);
				mipmap = *reinterpret_cast<uint32_t*>(&CGFX[objectOffset + 0x28]);
				formatID = *reinterpret_cast<uint32_t*>(&CGFX[objectOffset + 0x34]);
				size = *reinterpret_cast<uint32_t*>(&CGFX[objectOffset + 0x44]);
				dataOffset = *reinterpret_cast<uint32_t*>(&CGFX[objectOffset + 0x48]);
				dataOffset += (objectOffset + 0x48);//self-relative
				return 0;
			}
		}
	}
	return 2;
}

uint8_t getCBMDInfo(const std::string inpath, uint32_t* compressedSize, uint32_t* decompressedSize, uint32_t* CGFXoffset) {
	std::ifstream CBMD;
	CBMD.open(std::filesystem::path((const char8_t*)&*inpath.c_str()), std::ios_base::in | std::ios_base::binary);
	uint32_t CBMDmagic = 0;
	CBMD.seekg(0);
	CBMD.read(reinterpret_cast<char*>(&CBMDmagic), 0x4);
	if (CBMDmagic != 0x444D4243) {
		return 1;
	}
	//Offset for common CGFX
	uint32_t _CGFXoffset = 0;
	CBMD.seekg(0x8);
	CBMD.read(reinterpret_cast<char*>(&_CGFXoffset), 0x4);
	//BCWAV offset
	uint32_t BCWAVoffset = 0;
	CBMD.seekg(0x84);
	CBMD.read(reinterpret_cast<char*>(&BCWAVoffset), 0x4);
	std::vector<uint8_t> CGFX(BCWAVoffset - _CGFXoffset);
	//get stuff and decompress that stuff
	CBMD.seekg(_CGFXoffset);
	CBMD.read(reinterpret_cast<char*>(CGFX.data()), BCWAVoffset - _CGFXoffset);

	uint32_t decompressedSize_ = Get_Decompressed_size(CGFX.data());
	if (decompressedSize_ > 0x80000) {
		//delete[] CGFX;
		return 2;
	}
	*decompressedSize = decompressedSize_;
	*compressedSize = BCWAVoffset - _CGFXoffset;
	*CGFXoffset = _CGFXoffset;
	//delete[] CGFX;
	return 0;
}

uint8_t CBMDgetCommonCGFX(const std::string inpath, const uint32_t compressedSize, const uint32_t CGFXoffset, uint8_t* outbuff) {
	std::ifstream CBMD;
	CBMD.open(std::filesystem::path((const char8_t*)&*inpath.c_str()), std::ios_base::in | std::ios_base::binary);
	uint32_t CBMDmagic = 0;
	CBMD.seekg(0);
	CBMD.read(reinterpret_cast<char*>(&CBMDmagic), 0x4);
	if (CBMDmagic != 0x444D4243) {
		return 3;
	}
	std::vector<uint8_t> CGFX(compressedSize);
	//get stuff and decompress that stuff
	CBMD.seekg(CGFXoffset);
	CBMD.read(reinterpret_cast<char*>(CGFX.data()), compressedSize);

	if (DecompressLZ11(CGFX.data(), outbuff) == 0) {
		//delete[] CGFX;
		return 4;
	}
	//delete[] CGFX;

	uint32_t* CGFXmagic = reinterpret_cast<uint32_t*>(&outbuff[0]);
	if (*CGFXmagic != 0x58464743) {//CGFX
		return 5;
	}
	return 0;
}

uint8_t getCBMDTexture(const std::string inpath, const std::string symbol, uint8_t* outbuff) {
	uint32_t decompressedSize = 0;
	uint32_t compressedSize = 0;
	uint32_t CGFXoffset = 0;
	uint8_t ret = getCBMDInfo(inpath, &compressedSize, &decompressedSize, &CGFXoffset);
	if (ret > 0) {
		return ret;
	}
	std::vector<uint8_t> CGFXdecomp(decompressedSize);
	ret = CBMDgetCommonCGFX(inpath, compressedSize, CGFXoffset, CGFXdecomp.data());
	if (ret > 0) {
		//delete[] CGFXdecomp;
		return ret;
	}

	uint32_t dataOffset;
	uint32_t height;
	uint32_t width;
	uint32_t mipmap;
	uint32_t formatID;
	uint32_t size;
	ret = getCGFXtextureInfo(CGFXdecomp.data(), symbol, dataOffset, height, width, mipmap, formatID, size);
	if (ret == 0) {
		memcpy(outbuff, &CGFXdecomp[dataOffset], size);
		//delete[] CGFXdecomp;
		return ret;
	}
	//delete[] CGFXdecomp;
	return ret;
}
