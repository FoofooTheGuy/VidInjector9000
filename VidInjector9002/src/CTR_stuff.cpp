#include "CTR_stuff.hpp"

void Generate_Files(std::string dir, bool Multi) {
    if(!std::filesystem::exists(dir)) std::filesystem::create_directories(dir);
    miniz_cpp::zip_file file;
    file.load(Multi ? Multivid : Singlevid);
    std::vector<std::string> list = file.namelist();
    for (auto& member : list) {//plant seeds
        if (member.find_last_of("/") == member.size() - 1)
            std::filesystem::create_directory(dir + "/" + member);
    }
    file.extractall(dir, list);//grow fruit (don't you mean grow tree?)
}

bool TIDisValid(unsigned long TID) {
    unsigned long min = 0xC0000;
    unsigned long max = 0xEFFFF;
    switch (TID)
    {
    case 0xc0d00://Side-Scrolling Example
    case 0xce1cc://CHMM
    case 0xd921e://homebrew launcher loader
    case 0xda001://Smash Bros Dummy Application
    case 0xda002://3ds quick shutdown
    case 0xda003://Wifi Toggle
    case 0xe7a5a://NASA ALL
    case 0xec100://PKSM
    case 0xed990://NotifyMii
    case 0xeffec://FileKong
    case 0xeffed://TriaAl
    case 0:
    {
        return false;
    }
    break;
    default:
        if (TID > max || TID < min) return false;
    }
    return true;
}

unsigned long RandomTID() {
    unsigned long min = 0xC0000;
    unsigned long max = 0xEFFFF;
    unsigned long TID = 0;
    static std::mt19937 rng;

    while (!TIDisValid(TID)) {//loop until we get a good value
        rng.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        std::uniform_int_distribution<unsigned long> uniform(min, max);
        TID = uniform(rng);
    }
    return TID;
}

void image_data_to_tiles(void* out, void* img, uint32_t width, uint32_t height) {//from bannertool, edited for rgb565 only
	for (uint32_t y = 0; y < height; y++) {
		for (uint32_t x = 0; x < width; x++) {
			uint32_t index = (((y >> 3) * (width >> 3) + (x >> 3)) << 6) + ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3));

			uint8_t* pixel = &((uint8_t*)img)[(y * width + x) * 3];
			uint16_t color = 0;
			color = (uint16_t)((((uint8_t)(pixel[0]) & ~0x7) << 8) | (((uint8_t)(pixel[1]) & ~0x3) << 3) | ((uint8_t)(pixel[2]) >> 3));

			((uint16_t*)out)[index] = color;
		}
	}
}

const unsigned char bimgheader[32]{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x80, 0x00, 0x01, 0x02, 0x00, 0x00,
	0x10, 0x00, 0x00, 0x00, 0x12, 0xDC, 0xA6, 0x51,
};

//based on https://raw.githubusercontent.com/nothings/stb/master/tests/resample_test.cpp
bool convertToBimg(std::string input, unsigned char* outBuffer, bool writeHeader)// true for write header, false for dont write header
{
	unsigned char* input_pixels;
	unsigned char* output_pixels;
	unsigned char* output_3c;
	unsigned char* output_fin;
	int w, h, ch, comp;
	const int new_w = 256;
	const int new_h = 128;
	const int out_w = 200;
	const int out_h = 120;
	const uint8_t FF = 0xFF;
	if (!stbi_info(input.c_str(), &w, &h, &comp)) {
		return false;
	}
	input_pixels = stbi_load(input.c_str(), &w, &h, &ch, 0);
	output_pixels = (unsigned char*)malloc(out_w * out_h * ch);
	if (w == out_w && h == out_h) memcpy(output_pixels, input_pixels, w * h * ch);
	else stbir_resize_uint8(input_pixels, w, h, 0, output_pixels, out_w, out_h, 0, ch);//scale to 200x120 if needed
	stbi_image_free(input_pixels);

	output_3c = (unsigned char*)malloc(out_w * out_h * 3);
	if (ch == 4) {//if png?
		for (int i = 3; i < out_w * out_h * ch; i += 4) {//make background all white
			//https://stackoverflow.com/a/64655571
			uint8_t alpha_out = output_pixels[i] + (FF * (FF - output_pixels[i]) / FF);
			output_pixels[i - 1] = ((output_pixels[i - 1] * output_pixels[i] + FF * FF * (FF - output_pixels[i]) / FF) / alpha_out) & FF;
			output_pixels[i - 2] = ((output_pixels[i - 2] * output_pixels[i] + FF * FF * (FF - output_pixels[i]) / FF) / alpha_out) & FF;
			output_pixels[i - 3] = ((output_pixels[i - 3] * output_pixels[i] + FF * FF * (FF - output_pixels[i]) / FF) / alpha_out) & FF;
			output_pixels[i] = alpha_out;
		}
		int newi = 3;
		for (int i = 3; i < out_w * out_h * 4; i += 4) {//remove alpha channel
			output_3c[newi - 3] = output_pixels[i - 3];
			output_3c[newi - 2] = output_pixels[i - 2];
			output_3c[newi - 1] = output_pixels[i - 1];
			newi += 3;
		}
		free(output_pixels);
	}
	if (ch == 3) {
		memcpy(output_3c, output_pixels, out_w * out_h * 3);
		free(output_pixels);
	}

	//layer 200x120 image on a 256x128 image
	output_fin = (unsigned char*)malloc(new_w * new_h * 3);
	memset(output_fin, 0, new_w * new_h * 3);
	for (int y = 0; y < out_h; y++)
		for (int x = 0; x < out_w; x++) {
			output_fin[(y * (new_w)+x) * 3] = output_3c[(y * (out_w)+x) * 3];
			output_fin[(y * (new_w)+x) * 3 + 1] = output_3c[(y * (out_w)+x) * 3 + 1];
			output_fin[(y * (new_w)+x) * 3 + 2] = output_3c[(y * (out_w)+x) * 3 + 2];
		}
	free(output_3c);

	unsigned char* tiledbanner = new unsigned char[65536];
	image_data_to_tiles(tiledbanner, output_fin, new_w, new_h);
	if (writeHeader) {
		memcpy(outBuffer, bimgheader, sizeof(bimgheader));
		memcpy(outBuffer + sizeof(bimgheader), tiledbanner, sizeof(tiledbanner));
	}
	else {
		memcpy(outBuffer, tiledbanner, sizeof(tiledbanner));
	}

	//stbi_write_png("imag.png", new_w, new_h, 3, output_fin, 0);
	delete[] tiledbanner;
	free(output_fin);
	return true;
}

bool convertToIcon(std::string input, std::string output, std::string shortname, std::string longname, std::string publisher) {//bare bones SMDH creation. thanks 3dbrew
	unsigned char* input_pixels;
	unsigned char* output_pixels;
	unsigned char* large_3c;
	unsigned char* small_3c;
	int w, h, ch, comp;
	const int largeLW = 48;
	const int smallLW = 24;
	const uint8_t FF = 0xFF;
	if (!stbi_info(input.c_str(), &w, &h, &comp)) {
		return false;
	}
	input_pixels = stbi_load(input.c_str(), &w, &h, &ch, 0);
	output_pixels = (unsigned char*)malloc(largeLW * largeLW * ch);
	if (w == largeLW && h == largeLW) memcpy(output_pixels, input_pixels, w * h * ch);
	else stbir_resize_uint8(input_pixels, w, h, 0, output_pixels, largeLW, largeLW, 0, ch);//scale to 48x48 if needed

	large_3c = (unsigned char*)malloc(largeLW * largeLW * 3);
	if (ch == 4) {//if png?
		for (int i = 3; i < largeLW * largeLW * ch; i += 4) {//make background all white
			//https://stackoverflow.com/a/64655571
			uint8_t alpha_out = output_pixels[i] + (FF * (FF - output_pixels[i]) / FF);
			output_pixels[i - 1] = ((output_pixels[i - 1] * output_pixels[i] + FF * FF * (FF - output_pixels[i]) / FF) / alpha_out) & FF;//do & FF so that the compiler isnt like "uh what if there is a number larger than unsigned char euhh"
			output_pixels[i - 2] = ((output_pixels[i - 2] * output_pixels[i] + FF * FF * (FF - output_pixels[i]) / FF) / alpha_out) & FF;
			output_pixels[i - 3] = ((output_pixels[i - 3] * output_pixels[i] + FF * FF * (FF - output_pixels[i]) / FF) / alpha_out) & FF;
			output_pixels[i] = alpha_out;
		}
		int newi = 3;
		for (int i = 3; i < largeLW * largeLW * 4; i += 4) {
			large_3c[newi - 3] = output_pixels[i - 3];
			large_3c[newi - 2] = output_pixels[i - 2];
			large_3c[newi - 1] = output_pixels[i - 1];
			newi += 3;
		}
		free(output_pixels);
	}
	if (ch == 3) {
		for (int i = 0; i < largeLW * largeLW * 3; i++) large_3c[i] = output_pixels[i];
		free(output_pixels);
	}

	small_3c = (unsigned char*)malloc(smallLW * smallLW * 3);
	stbir_resize_uint8(large_3c, largeLW, largeLW, 0, small_3c, smallLW, smallLW, 0, 3);//make the small icon
	unsigned char tiledsmall[0x480];
	unsigned char tiledlarge[0x1200];
	image_data_to_tiles(tiledsmall, small_3c, smallLW, smallLW);
	image_data_to_tiles(tiledlarge, large_3c, largeLW, largeLW);
	std::ofstream smdh(output, std::ios_base::out | std::ios_base::binary);
	smdh << "SMDH";//make smdh!
	for (int i = 0; i < 4; i++)
		smdh << '\0';
	for (int i = 0; i < 16; i++) {
		smdh << shortname;
		for (size_t j = 0; j < 0x80 - shortname.size(); j++)
			smdh << '\0';
		smdh << longname;
		for (size_t j = 0; j < 0x100 - longname.size(); j++)
			smdh << '\0';
		smdh << publisher;
		for (size_t j = 0; j < 0x80 - publisher.size(); j++)
			smdh << '\0';
	}
	for (int i = 0; i < 0x10; i++)
		smdh << '\0';
	smdh << "\xFF\xFF\xFF\x7F";//region free
	for (int i = 0; i < 0xC; i++)
		smdh << '\0';
	smdh << "\x01\x04";//visible, no save backups
	for (int i = 0; i < 0x16; i++)
		smdh << '\0';
	smdh.write(reinterpret_cast<const char*>(tiledsmall), sizeof(tiledsmall));
	smdh.write(reinterpret_cast<const char*>(tiledlarge), sizeof(tiledlarge));

	//stbi_write_png(output.c_str(), largeLW, largeLW, 3, large_3c, 0);
	stbi_image_free(input_pixels);
	free(small_3c);
	free(large_3c);
	return true;
}


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