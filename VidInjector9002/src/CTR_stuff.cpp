#include "CTR_stuff.hpp"

void Generate_Files(std::string dir, bool Multi) {
	if (!std::filesystem::exists(dir)) std::filesystem::create_directories(dir);
	miniz_cpp::zip_file file;
	file.load(Multi ? Multivid : Singlevid);
	std::vector<std::string> list = file.namelist();
	for (auto& member : list) {//plant seeds
		if (member.find_last_of("/") == member.size() - 1)
			std::filesystem::create_directory(dir + "/" + member);
	}
	file.extractall(dir, list);//grow fruit (don't you mean grow tree?)
}

bool TIDisValid(uint32_t TID) {
	uint32_t min = 0xC0000;
	uint32_t max = 0xEFFFF;
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

uint32_t RandomTID() {
	uint32_t min = 0xC0000;
	uint32_t max = 0xEFFFF;
	uint32_t TID = 0;
	static std::mt19937 rng;

	while (!TIDisValid(TID)) {//loop until we get a good value
		rng.seed(static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
		std::uniform_int_distribution<unsigned long> uniform(min, max);
		TID = uniform(rng);
	}
	return TID;
}

void resize_crop(const uint8_t* input_pixels, int input_w, int input_h, uint8_t* output_pixels, int output_w, int output_h, int num_channels) {//this has to be here because of stbir_resize_uint8 and i dont wann include that everywhere because it's all inline
	int width, height;

	//"inspired" by https://github.com/endlessm/chromium-browser/blob/aa8c819d5ad2fcb3854a688a0401975eca721f43/ui/gfx/favicon_size.cc#L13
	if (input_w > input_h) {//panoramic
		height = output_h;
		width = static_cast<int>(output_h * static_cast<float>(input_w) / static_cast<float>(input_h));
	}
	else if (input_w < input_h) {//portrait
		height = static_cast<int>(input_h / (static_cast<float>(input_w) / static_cast<float>(output_w)));
		width = output_w;
	}
	else {//square
		width = (output_w >= output_h) ? output_w : output_h;
		height = (output_w >= output_h) ? output_w : output_h;
	}
	if (width < output_w) {
		height = static_cast<int>(input_h / (static_cast<float>(input_w) / static_cast<float>(output_w)));
		width = output_w;
	}
	if (height < output_h) {
		height = output_h;
		width = static_cast<int>(output_h * static_cast<float>(input_w) / static_cast<float>(input_h));
	}

	uint8_t* scaled = (uint8_t*)malloc(width * height * num_channels);
	stbir_resize_uint8(input_pixels, input_w, input_h, 0, scaled, width, height, 0, num_channels);//scale it down
	crop_pixels(scaled, width, height, num_channels, output_pixels, (width - output_w) / 2, (height - output_h) / 2, output_w, output_h);
	free(scaled);
}

const uint8_t bimgheader[32]{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x80, 0x00, 0x01, 0x02, 0x00, 0x00,
	0x10, 0x00, 0x00, 0x00, 0x12, 0xDC, 0xA6, 0x51,
};

//based on https://raw.githubusercontent.com/nothings/stb/master/tests/resample_test.cpp
bool convertToBimg(std::string input, uint8_t* outBuffer, bool writeHeader)// true for write header, false for dont write header
{
	uint8_t* input_pixels;
	uint8_t* output_pixels;
	uint8_t* output_4c;
	uint8_t* white;
	uint8_t* output_fin;
	int w, h, ch, comp;
	const int new_w = 256;
	const int new_h = 128;
	const int out_w = 200;
	const int out_h = 120;
	const uint8_t FF = 0xFF;
	if (std::filesystem::exists(input)) {
		std::string extension = input;
		extension.erase(extension.begin(), extension.end() - 5);
		if (extension == ".bimg") {
			if (std::filesystem::file_size(input) == 0x10020) {
				w = 256;
				h = 128;
				int ich = sizeof(nnc_u16);
				std::ifstream infile;
				infile.open(input, std::ios_base::in | std::ios_base::binary);//input file
				uint8_t* input_data = (uint8_t*)malloc((w * h * ich) + 0x20);
				if (!input_data) {
					free(input_data);
					return false;
				}
				char Byte;
				int it = 0;
				infile.read(&Byte, 1);//grab first byte of data
				while (infile) {//continue until input stream fails
					input_data[it] = Byte;
					infile.read(&Byte, 1);//grab next byte of file
					it++;
				}
				infile.close();
				for (int i = 0; i < 0x1C; i++) {
					if (input_data[i] != bimgheader[i]) {
						free(input_data);
						return false;
					}
				}
				if (writeHeader) {
					memcpy(outBuffer, input_data, (w * h * ich) + 0x20);
				}
				else {
					memcpy(outBuffer, &input_data[0x20], (w * h * ich));
				}
				return true;
			}
			return false;
		}
	}
	if (!stbi_info(input.c_str(), &w, &h, &comp)) {
		w = out_w;
		h = out_h;
		ch = 4;
		input_pixels = (uint8_t*)malloc(out_w * out_h * ch);
		if (input_pixels == NULL) {
			free(input_pixels);
			return false;
		}
		memset(input_pixels, FF, out_w * out_h * ch);
	}
	else input_pixels = stbi_load(input.c_str(), &w, &h, &ch, 0);
	output_pixels = (uint8_t*)malloc(out_w * out_h * ch);
	if (output_pixels == NULL) {
		free(output_pixels);
		free(input_pixels);
		return false;
	}
	if (w == out_w && h == out_h) memcpy(output_pixels, input_pixels, w * h * ch);
	else resize_crop(input_pixels, w, h, output_pixels, out_w, out_h, ch);//scale to 200x120 if needed
	free(input_pixels);

	output_4c = (uint8_t*)malloc(out_w * out_h * 4);
	if (output_4c == NULL) {
		free(output_pixels);
		free(output_4c);
		return false;
	}
	ToRGBA(output_pixels, output_4c, out_w, out_h, ch);
	white = (uint8_t*)malloc(out_w * out_h * 4);
	if (white == NULL) {
		free(white);
		free(output_pixels);
		free(output_4c);
		return false;
	}
	//stbi_write_png("output_pixels.png", out_w, out_h, ch, output_pixels, 0);
	memset(white, 0xFF, out_w * out_h * 4);
	layer_pixels(output_4c, output_pixels, white, out_w, out_h, ch, out_w, out_h, 4, 0, 0);
	//stbi_write_png("output_4c.png", out_w, out_h, 4, output_pixels, 0);
	free(white);
	free(output_pixels);

	//layer 200x120 image on a 256x128 image
	output_fin = (uint8_t*)malloc(new_w * new_h * 4);
	if (output_fin == NULL) {
		free(output_fin);
		free(output_4c);
		return false;
	}

	memset(output_fin, 0, new_w * new_h * 4);
	for (int i = 3; i < new_w * new_h * 4; i += 4)
		output_fin[i] = 0xFF;//make alpha 0xFF
	//layer_pixels(output_fin, output_4c, black, out_w, out_h, 4, new_w, new_h, 3, 0, 0);//why doesnt this work for this????
	for (int y = 0; y < out_h; y++)
		for (int x = 0; x < out_w; x++) {
			for (int c = 0; c < 4; c++)
				output_fin[(y * (new_w)+x) * 4 + c] = output_4c[(y * (out_w)+x) * 4 + c];
		}
	//stbi_write_png("output_fin.png", new_w, new_h, 4, output_fin, 0);
	free(output_4c);

	uint8_t tiledbanner[new_w * new_h * sizeof(nnc_u16)];
	nnc_swizzle_zorder_be_rgba8_to_le_rgb565(reinterpret_cast<nnc_u32*>(output_fin), reinterpret_cast<nnc_u16*>(tiledbanner), new_w, new_h);
	free(output_fin);
	if (writeHeader) {
		memcpy(outBuffer, bimgheader, sizeof(bimgheader));
		memcpy(outBuffer + sizeof(bimgheader), tiledbanner, sizeof(tiledbanner));
	}
	else {
		memcpy(outBuffer, tiledbanner, sizeof(tiledbanner));
	}
	return true;
}

bool convertToIcon(std::string input, std::string output, std::string shortname, std::string longname, std::string publisher, int borderMode) {//bare bones SMDH creation. thanks 3dbrew
	uint8_t* input_pixels;
	uint8_t* output_pixels;
	uint8_t* large_4c;
	uint8_t* small_4c;
	int w, h, ch, comp;
	const int largeLW = 48;
	const int smallLW = 24;
	const uint8_t FF = 0xFF;
	if (!stbi_info(input.c_str(), &w, &h, &comp)) {
		w = largeLW;
		h = largeLW;
		ch = 4;
		input_pixels = (uint8_t*)malloc(largeLW * largeLW * ch);
		memset(input_pixels, FF, largeLW * largeLW * ch);
	}
	else input_pixels = stbi_load(input.c_str(), &w, &h, &ch, 0);
	output_pixels = (uint8_t*)malloc(largeLW * largeLW * ch);
	if (output_pixels == NULL) {
		free(output_pixels);
		free(input_pixels);
		return false;
	}
	if (w == largeLW && h == largeLW) memcpy(output_pixels, input_pixels, w * h * ch);
	else resize_crop(input_pixels, w, h, output_pixels, largeLW, largeLW, ch);//scale to 48x48 if needed
	stbi_image_free(input_pixels);

	if (borderMode == 1) {
		uint8_t* output_4c = (uint8_t*)malloc(largeLW * largeLW * 4);
		uint8_t* white_background = (uint8_t*)malloc(largeLW * largeLW * 4);//fix the bugs by not fixing the bugs! :D
		memset(white_background, FF, largeLW * largeLW * 4);
		layer_pixels(output_4c, output_pixels, white_background, largeLW, largeLW, ch, largeLW, largeLW, 4, 0, 0);
		free(white_background);
		layer_pixels(output_4c, icon_border, output_4c, largeLW, largeLW, 4, largeLW, largeLW, 4, 0, 0);
		ch = 4;
		free(output_pixels);
		output_pixels = (uint8_t*)malloc(largeLW * largeLW * ch);
		memcpy(output_pixels, output_4c, largeLW * largeLW * ch);
		free(output_4c);
	}
	else if (borderMode == 2) {
		uint8_t* output_4c = (uint8_t*)malloc(largeLW * largeLW * 4);
		uint8_t* white_background = (uint8_t*)malloc(largeLW * largeLW * 4);//fix the bugs by not fixing the bugs! :D
		memset(white_background, FF, largeLW * largeLW * 4);
		layer_pixels(output_4c, output_pixels, white_background, largeLW, largeLW, ch, largeLW, largeLW, 4, 0, 0);
		free(white_background);
		ch = 4;
		uint8_t* scaled = (uint8_t*)malloc(largeLW * largeLW * ch);
		stbir_resize_uint8(output_4c, largeLW, largeLW, 0, scaled, largeLW - 10, largeLW - 10, 0, ch);//scale it down
		layer_pixels(output_4c, icon_border, scaled, largeLW, largeLW, ch, largeLW - 10, largeLW - 10, ch, 5, 5);
		free(scaled);
		free(output_pixels);
		output_pixels = (uint8_t*)malloc(largeLW * largeLW * ch);
		memcpy(output_pixels, output_4c, largeLW * largeLW * ch);
		free(output_4c);
	}

	large_4c = (uint8_t*)malloc(largeLW * largeLW * 4);
	if (large_4c == NULL) {
		free(large_4c);
		return false;
	}
	ToRGBA(output_pixels, large_4c, largeLW, largeLW, ch);
	free(output_pixels);

	small_4c = (uint8_t*)malloc(smallLW * smallLW * 4);
	stbir_resize_uint8(large_4c, largeLW, largeLW, 0, small_4c, smallLW, smallLW, 0, 4);//make the small icon
	uint8_t tiledsmall[smallLW * smallLW * sizeof(nnc_u16)];
	uint8_t tiledlarge[largeLW * largeLW * sizeof(nnc_u16)];
	nnc_swizzle_zorder_be_rgba8_to_le_rgb565(reinterpret_cast<nnc_u32*>(small_4c), reinterpret_cast<nnc_u16*>(tiledsmall), smallLW, smallLW);
	nnc_swizzle_zorder_be_rgba8_to_le_rgb565(reinterpret_cast<nnc_u32*>(large_4c), reinterpret_cast<nnc_u16*>(tiledlarge), largeLW, largeLW);
	//stbi_write_png("imag.png", largeLW, largeLW, 4, large_4c, 0);
	if (shortname.size() > 0x80 || longname.size() > 0x100 || publisher.size() > 0x80) {
		free(small_4c);
		free(large_4c);
		return false;
	}
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
	free(small_4c);
	free(large_4c);
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

uint32_t Get_Decompressed_size(const uint8_t* in) {
	uint32_t decompressed_size;
	memcpy(&decompressed_size, in, sizeof(uint32_t));
	in += 4;

	uint8_t type = decompressed_size & 0xFF;
	if (type != 0x11) return -1;
	decompressed_size >>= 8;
	return decompressed_size;
}

uint32_t DecompressLZ11(const uint8_t* in, uint8_t* out) {//https://github.com/citra-emu/citra/blob/bfb6a5b5de1e7a89ceebabe33e792d5c03a2cf46/src/core/hle/service/apt/apt.cpp#L135
	uint32_t decompressed_size;
	memcpy(&decompressed_size, in, sizeof(uint32_t));
	in += 4;

	uint8_t type = decompressed_size & 0xFF;
	if (type != 0x11) return -1;
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

bool getCGFXtextureInfo(uint8_t* CGFX, const std::string symbol, uint32_t** dataOffset, uint32_t** height, uint32_t** width, uint32_t** mipmap, uint32_t** formatID, uint32_t** size) {
	uint32_t* CGFXmagic = reinterpret_cast<uint32_t*>(&CGFX[0]);
	if (*CGFXmagic != 0x58464743) {//CGFX
		return false;
	}

	uint16_t* CGFXheaderSize = reinterpret_cast<uint16_t*>(&CGFX[0x6]);
	for (uint32_t N = 0; N < *reinterpret_cast<uint32_t*>(&CGFX[0x10]); N++) {//loop through Number of Dicts
		uint32_t* DICToffset = reinterpret_cast<uint32_t*>(&CGFX[*CGFXheaderSize + 0xC + (N * 8)]);
		*DICToffset += (*CGFXheaderSize + 0xC + (N * 8));//since it's self-relative, do this
		for (uint32_t i = 0; i < *reinterpret_cast<uint32_t*>(&CGFX[*DICToffset + 0x8]); i++) {//loop through entries in DICT N
			uint32_t* symbolOffset = reinterpret_cast<uint32_t*>(&CGFX[*DICToffset + 0x1C + (0x10 * i) + 0x8]);
			*symbolOffset += (*DICToffset + 0x1C + (0x10 * i) + 0x8);//self-relative
			uint32_t* objectOffset = reinterpret_cast<uint32_t*>(&CGFX[*DICToffset + 0x1C + (0x10 * i) + 0xC]);
			*objectOffset += (*DICToffset + 0x1C + (0x10 * i) + 0xC);//self-relative
			std::string isymbol = "";
			const auto* ch = &CGFX[*symbolOffset];
			while (*ch)
				isymbol += *ch++;
			if (strcmp(isymbol.c_str(), symbol.c_str()) == 0) {
				*height = reinterpret_cast<uint32_t*>(&CGFX[*objectOffset + 0x18]);
				*width = reinterpret_cast<uint32_t*>(&CGFX[*objectOffset + 0x1C]);
				*mipmap = reinterpret_cast<uint32_t*>(&CGFX[*objectOffset + 0x28]);
				*formatID = reinterpret_cast<uint32_t*>(&CGFX[*objectOffset + 0x34]);
				*size = reinterpret_cast<uint32_t*>(&CGFX[*objectOffset + 0x44]);
				uint32_t* dataOffsetStandin = reinterpret_cast<uint32_t*>(&CGFX[*objectOffset + 0x48]);
				*dataOffsetStandin += (*objectOffset + 0x48);//self-relative
				*dataOffset = dataOffsetStandin;
				return true;
			}
		}
	}
	return false;
}

bool getCGFXInfo(const std::string inpath, uint32_t* compressedSize, uint32_t* decompressedSize, uint32_t* CGFXoffset) {
	std::ifstream CBMD;
	CBMD.open(inpath, std::ios_base::in | std::ios_base::binary);
	uint32_t CBMDmagic = 0;
	CBMD.seekg(0);
	CBMD.read(reinterpret_cast<char*>(&CBMDmagic), 0x4);
	if (CBMDmagic != 0x444D4243) {
		return false;
	}
	//Offset for common CGFX
	uint32_t _CGFXoffset = 0;
	CBMD.seekg(0x8);
	CBMD.read(reinterpret_cast<char*>(&_CGFXoffset), 0x4);
	//BCWAV offset
	uint32_t BCWAVoffset = 0;
	CBMD.seekg(0x84);
	CBMD.read(reinterpret_cast<char*>(&BCWAVoffset), 0x4);
	uint8_t* CGFX = new uint8_t[BCWAVoffset - _CGFXoffset];
	//get stuff and decompress that stuff
	CBMD.seekg(_CGFXoffset);
	char Byte;
	size_t it = 0;
	CBMD.read(&Byte, 1);//grab first byte of CGFXoffset
	while (it < BCWAVoffset - _CGFXoffset) {
		CGFX[it] = Byte;//append byte to array
		CBMD.read(&Byte, 1);//grab next byte of file
		it++;
	}

	uint32_t decompressedSize_ = Get_Decompressed_size(CGFX);
	if (decompressedSize_ > 0x80000) {
		delete[] CGFX;
		return false;
	}
	*decompressedSize = decompressedSize_;
	*compressedSize = BCWAVoffset - _CGFXoffset;
	*CGFXoffset = _CGFXoffset;
	delete[] CGFX;
	return true;
}

bool CBMDgetCommonCGFX(const std::string inpath, const uint32_t compressedSize, const uint32_t decompressedSize, const uint32_t CGFXoffset, uint8_t* outbuff) {
	std::ifstream CBMD;
	CBMD.open(inpath, std::ios_base::in | std::ios_base::binary);
	uint32_t CBMDmagic = 0;
	CBMD.seekg(0);
	CBMD.read(reinterpret_cast<char*>(&CBMDmagic), 0x4);
	if (CBMDmagic != 0x444D4243) {
		return false;
	}
	uint8_t* CGFX = new uint8_t[compressedSize];
	//get stuff and decompress that stuff
	CBMD.seekg(CGFXoffset);
	char Byte;
	size_t it = 0;
	CBMD.read(&Byte, 1);//grab first byte of CGFXoffset
	while (it < compressedSize) {
		CGFX[it] = Byte;//append byte to array
		CBMD.read(&Byte, 1);//grab next byte of file
		it++;
	}

	if (DecompressLZ11(CGFX, outbuff) == NULL) {
		delete[] CGFX;
		return false;
	}
	delete[] CGFX;

	uint32_t* CGFXmagic = reinterpret_cast<uint32_t*>(&outbuff[0]);
	if (*CGFXmagic != 0x58464743) {//CGFX
		return false;
	}
	return true;
}

bool getCBMDTexture(const std::string inpath, const std::string symbol, uint8_t* outbuff) {
	uint32_t decompressedSize = 0;
	uint32_t compressedSize = 0;
	uint32_t CGFXoffset = 0;
	if (!getCGFXInfo(inpath, &compressedSize, &decompressedSize, &CGFXoffset)) {
		return false;
	}
	uint8_t* CGFXdecomp = new uint8_t[decompressedSize];
	if (!CBMDgetCommonCGFX(inpath, compressedSize, decompressedSize, CGFXoffset, CGFXdecomp)) {
		delete[] CGFXdecomp;
		return false;
	}

	uint32_t* dataOffset;
	uint32_t* height;
	uint32_t* width;
	uint32_t* mipmap;
	uint32_t* formatID;
	uint32_t* size;
	if (getCGFXtextureInfo(CGFXdecomp, symbol, &dataOffset, &height, &width, &mipmap, &formatID, &size)) {
		memcpy(outbuff, &CGFXdecomp[*dataOffset], *size);
		delete[] CGFXdecomp;
		return true;
	}
	delete[] CGFXdecomp;
	return false;
}