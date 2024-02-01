#include "formatting.hpp"

size_t chrcount(const std::string& str) {
	size_t length = 0;
	for (char c : str) {
		if ((c & 0xC0) != 0x80) {
			++length;
		}
	}
	return length;
}

std::string UTF8toUTF16(const std::string input) {
	std::vector<uint8_t> utf8 = std::vector<uint8_t>(input.size() + 1);
	std::vector<uint16_t> utf16 = std::vector<uint16_t>(chrcount(input) * 2);
	memcpy(utf8.data(), input.c_str(), input.size());
	utf8[input.size()] = '\0';
	nnc_utf8_to_utf16(utf16.data(), chrcount(input) * 2 + 1, utf8.data(), input.size());

	std::string output(reinterpret_cast<char*>(utf16.data()), chrcount(input) * 2);

	return output;
}

std::string UTF16toUTF8(const std::string& input) {
	size_t outLen = 0;
	size_t utf16length = input.size() / 2;//divide by 2 because it's a u8 size going into a u16 array
	std::vector<uint16_t> utf16 = std::vector<uint16_t>(utf16length);
	memcpy(utf16.data(), &input[0], input.size());
	size_t utf8length = nnc_utf16_to_utf8(NULL, 0, utf16.data(), utf16length) + 1;
	std::vector<uint8_t> utf8 = std::vector<uint8_t>(utf8length);
	memset(utf8.data(), 0, utf8length);

	outLen = nnc_utf16_to_utf8(utf8.data(), utf8length, utf16.data(), utf16length);
	std::string output(reinterpret_cast<char*>(utf8.data()), outLen);

	return output;
}

void crop_pixels(const uint8_t* input, int width, int height, int channels, uint8_t* output, int x_offset, int y_offset, int out_w, int out_h) {
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++) {
			if (x >= x_offset && x < width - x_offset && y >= y_offset && y < height - y_offset && (x - x_offset) < out_w && (y - y_offset) < out_h) {
				for (int ch = 0; ch < channels; ch++)
					output[((y - y_offset) * out_w + (x - x_offset)) * channels + ch] = input[(y * width + x) * channels + ch];
			}
		}
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

	std::vector<uint8_t> scaled = std::vector<uint8_t>(width * height * num_channels);
	stbir_resize_uint8(input_pixels, input_w, input_h, 0, scaled.data(), width, height, 0, num_channels);//scale it down
	crop_pixels(scaled.data(), width, height, num_channels, output_pixels, (width - output_w) / 2, (height - output_h) / 2, output_w, output_h);
}

void ToRGBA(const uint8_t* input, uint8_t* output, int width, int height, int channels) {
	if (channels == 1) {//grayscale
		int j = 0;
		for (int i = 0; i < width * height; i++) {
			for (int ch = 0; ch < 3; ch++)
				output[j + ch] = input[i];
			output[j + 3] = 0xFF;
			j += 4;
		}
	}
	else if (channels == 2) {//grayscale alpha
		int j = 0;
		for (int i = 0; i < width * height * channels; i += channels) {
			for (int ch = 0; ch < 3; ch++)
				output[j + ch] = input[i];
			output[j + 3] = input[i + 1];
			j += 4;
		}
	}
	else if (channels == 3) {//rgb
		int j = 0;
		for (int i = 0; i < width * height * channels; i += channels) {
			for (int ch = 0; ch < 3; ch++)
				output[j + ch] = input[i + ch];
			output[j + 3] = 0xFF;
			j += 4;
		}
	}
	else if (channels == 4) {//rgba
		memcpy(output, input, width * height * channels);
	}
}

//only works if the forwground image is larger than the background image
void layer_pixels(uint8_t* out, uint8_t* foreground, uint8_t* background, int forewidth, int foreheight, int forechannels, int backwidth, int backheight, int backchannels, int x_offset, int y_offset) {
	std::vector<uint8_t> foreground_4c = std::vector<uint8_t>(forewidth * foreheight * 4);
	std::vector<uint8_t> background_4c = std::vector<uint8_t>(backwidth * backheight * 4);
	const uint8_t FF = 0xFF;

	ToRGBA(foreground, foreground_4c.data(), forewidth, foreheight, forechannels);
	ToRGBA(background, background_4c.data(), backwidth, backheight, backchannels);

	for (int y = 0; y < foreheight; y++)
		for (int x = 0; x < forewidth; x++) {
			if (x >= x_offset && x < forewidth - x_offset && y >= y_offset && y < backheight + y_offset) {
				//https://stackoverflow.com/a/64655571
				uint8_t alpha_out = foreground_4c.at((y * foreheight + x) * 4 + 3) + (background_4c.at(((y - y_offset) * backwidth + (x - x_offset)) * 4 + 3) * (FF - foreground_4c.at((y * foreheight + x) * 4 + 3)) / FF);
				for (int ch = 0; ch < 3; ch++)
					out[(y * forewidth + x) * 4 + ch] = (foreground_4c.at((y * forewidth + x) * 4 + ch) * foreground_4c.at((y * forewidth + x) * 4 + 3) + background_4c.at(((y - y_offset) * backwidth + (x - x_offset)) * 4 + ch) * FF * (FF - foreground_4c.at((y * forewidth + x) * 4 + 3)) / FF) / alpha_out;
				out[(y * forewidth + x) * 4 + 3] = alpha_out;
			}
			else {
				for (int ch = 0; ch < 4; ch++)
					out[(y * forewidth + x) * 4 + ch] = foreground_4c.at((y * forewidth + x) * 4 + ch);
			}
		}
}

std::string tolowerstr(std::string str) {
	for (char& i : str)
		i = tolower(i) & 0xFF;
	return str;
}

std::string toupperstr(std::string str) {
	for (char& i : str)
		i = toupper(i) & 0xFF;
	return str;
}

bool stol_s(long& output, std::string input, bool isHex) {
	std::string lowinput(tolowerstr(input));
	if (isHex) {
		if (lowinput.find_first_of("0123456789abcdef") != 0) return false;
		else output = std::stol(lowinput, nullptr, 16);
	}
	else {
		if (lowinput.find_first_of("0123456789") != 0) return false;
		else output = std::stol(lowinput);
	}
	return true;
}

bool stoul_s(unsigned long& output, std::string input, bool isHex) {
	std::string lowinput(tolowerstr(input));
	if (isHex) {
		if (lowinput.find_first_of("0123456789abcdef") != 0) return false;
		else output = std::stoul(lowinput, nullptr, 16);
	}
	else {
		if (lowinput.find_first_of("0123456789") != 0) return false;
		else output = std::stoul(lowinput);
	}
	return true;
}

bool stoul_s(uint8_t& output, std::string input, bool isHex) {
	std::string lowinput(tolowerstr(input));
	if (isHex) {
		if (lowinput.find_first_of("0123456789abcdef") != 0) return false;
		else output = std::stoul(&lowinput[0], nullptr, 16) & 0xFF;
	}
	else {
		if (lowinput.find_first_of("0123456789") != 0) return false;
		else output = std::stoul(&lowinput[0]) & 0xFF;
	}
	return true;
}

bool stoul_s(uint32_t& output, std::string input, bool isHex) {
	std::string lowinput(tolowerstr(input));
	if (isHex) {
		if (lowinput.find_first_of("0123456789abcdef") != 0) return false;
		else output = std::stoul(lowinput, nullptr, 16);
	}
	else {
		if (lowinput.find_first_of("0123456789") != 0) return false;
		else output = std::stoul(lowinput);
	}
	return true;
}

std::error_code copyfile(std::string inpath, std::string outpath) {//also works with directories
	std::error_code error;
	std::filesystem::remove_all(std::filesystem::path((const char8_t*)&*outpath.c_str()), error);
	if (error) return error;
	std::filesystem::copy(std::filesystem::path((const char8_t*)&*inpath.c_str()), std::filesystem::path((const char8_t*)&*outpath.c_str()), std::filesystem::copy_options::recursive, error);
	return error;
}

void encode_bigend_u64(uint64_t value, void* dest)//https://stackoverflow.com/a/36552262
{
	value =
		((value & 0xFF00000000000000u) >> 56u) |
		((value & 0x00FF000000000000u) >> 40u) |
		((value & 0x0000FF0000000000u) >> 24u) |
		((value & 0x000000FF00000000u) >> 8u) |
		((value & 0x00000000FF000000u) << 8u) |
		((value & 0x0000000000FF0000u) << 24u) |
		((value & 0x000000000000FF00u) << 40u) |
		((value & 0x00000000000000FFu) << 56u);
	memcpy(dest, &value, sizeof(uint64_t));
}
