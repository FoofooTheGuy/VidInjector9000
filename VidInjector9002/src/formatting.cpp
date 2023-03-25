#include "formatting.hpp"

//this UTF stuff is from libctru lol
inline std::size_t strlen(const std::string& str) {//http://www.cplusplus.com/forum/beginner/192031/#msg925794
	std::size_t length = 0;
	for (char c : str) {
		if ((c & 0xC0) != 0x80) {
			++length;
		}
	}
	return length;
}

//https://github.com/devkitPro/libctru/raw/4e25fb1d6c2ea124a9011c4b65f76f2968a9fb97/libctru/source/util/utf/encode_utf16.c
inline int
encode_utf16(uint16_t* out,
	uint32_t in)
{
	if (in < 0x10000)
	{
		if (out != NULL)
			*out++ = in;
		return 1;
	}
	else if (in < 0x110000)
	{
		if (out != NULL)
		{
			*out++ = (in >> 10) + 0xD7C0;
			*out++ = (in & 0x3FF) + 0xDC00;
		}
		return 2;
	}

	return -1;
}

//https://github.com/devkitPro/libctru/raw/4e25fb1d6c2ea124a9011c4b65f76f2968a9fb97/libctru/source/util/utf/decode_utf8.c
inline int
decode_utf8(uint32_t* out,
	const uint8_t* in)
{
	uint8_t code1, code2, code3, code4;

	code1 = *in++;
	if (code1 < 0x80)
	{
		/* 1-byte sequence */
		*out = code1;
		return 1;
	}
	else if (code1 < 0xC2)
	{
		return -1;
	}
	else if (code1 < 0xE0)
	{
		/* 2-byte sequence */
		code2 = *in++;
		if ((code2 & 0xC0) != 0x80)
		{
			return -1;
		}

		*out = (code1 << 6) + code2 - 0x3080;
		return 2;
	}
	else if (code1 < 0xF0)
	{
		/* 3-byte sequence */
		code2 = *in++;
		if ((code2 & 0xC0) != 0x80)
		{
			return -1;
		}
		if (code1 == 0xE0 && code2 < 0xA0)
		{
			return -1;
		}

		code3 = *in++;
		if ((code3 & 0xC0) != 0x80)
		{
			return -1;
		}

		*out = (code1 << 12) + (code2 << 6) + code3 - 0xE2080;
		return 3;
	}
	else if (code1 < 0xF5)
	{
		/* 4-byte sequence */
		code2 = *in++;
		if ((code2 & 0xC0) != 0x80)
		{
			return -1;
		}
		if (code1 == 0xF0 && code2 < 0x90)
		{
			return -1;
		}
		if (code1 == 0xF4 && code2 >= 0x90)
		{
			return -1;
		}

		code3 = *in++;
		if ((code3 & 0xC0) != 0x80)
		{
			return -1;
		}

		code4 = *in++;
		if ((code4 & 0xC0) != 0x80)
		{
			return -1;
		}

		*out = (code1 << 18) + (code2 << 12) + (code3 << 6) + code4 - 0x3C82080;
		return 4;
	}

	return -1;
}

//https://github.com/devkitPro/libctru/raw/4e25fb1d6c2ea124a9011c4b65f76f2968a9fb97/libctru/source/util/utf/utf8_to_utf16.c
inline int
utf8_to_utf16(uint16_t* out,
	const uint8_t* in,
	size_t        len)
{
	int  rc = 0;
	int  units;
	uint32_t code;
	uint16_t encoded[2];

	do
	{
		units = decode_utf8(&code, in);
		if (units == -1)
			return -1;

		if (code > 0)
		{
			in += units;

			units = encode_utf16(encoded, code);
			if (units == -1)
				return -1;

			if (out != NULL)
			{
				if (rc + units <= len)
				{
					*out++ = encoded[0];
					if (units > 1)
						*out++ = encoded[1];
				}
			}

			if (_SIZE_MAX - units >= rc)
				rc += units;
			else
				return -1;
		}
	} while (code > 0);

	return rc;
}

std::string UTF8toUTF16(std::string input) {//not to be confused with utf8_to_utf16
	std::string output = "";
	uint8_t* utf8 = new uint8_t[input.size() + 1];
	uint16_t* utf16 = new uint16_t[strlen(input) * 2 + 1];
	memcpy(utf8, input.c_str(), input.size());
	utf8[input.size()] = '\0';
	utf8_to_utf16(utf16, utf8, input.size());
	char* utf16str = new char[strlen(input) * 2 + 1];
	memcpy(utf16str, utf16, strlen(input) * 2 + 1);

	for (size_t i = 0; i < strlen(input) * 2; i++)
		output += utf16str[i];

	delete[] utf8;
	delete[] utf16;
	delete[] utf16str;
	return output;
}

void layer_pixels(unsigned char* out, unsigned char* foreground, unsigned char* background, int forewidth, int foreheight, int forechannels, int backwidth, int backheight, int backchannels, int x_offset, int y_offset) {
	unsigned char* foreground_4c = (unsigned char*)malloc(forewidth * foreheight * 4);
	unsigned char* background_4c = (unsigned char*)malloc(backwidth * backheight * 4);
	const uint8_t FF = 0xFF;
	if (forechannels == 3) {//if it's 3 make it 4
		int j = 0;
		for (int i = 0; i < forewidth * foreheight * forechannels; i += forechannels) {
			for (int ch = 0; ch < 3; ch++)
				foreground_4c[j + ch] = foreground[i + ch];
			foreground_4c[j + 3] = 0xFF;
			j += 4;
		}
	}
	else if (forechannels == 4) {
		memcpy(foreground_4c, foreground, forewidth * foreheight * forechannels);
	}

	if (backchannels == 3) {//if it's 3 make it 4
		int j = 0;
		for (int i = 0; i < backwidth * backheight * backchannels; i += backchannels) {
			for (int ch = 0; ch < 3; ch++)
				background_4c[j + ch] = background[i + ch];
			background_4c[j + 3] = 0xFF;
			j += 4;
		}
	}
	else if (forechannels == 4) {
		memcpy(background_4c, background, backwidth * backheight * backchannels);
	}

	for (int y = 0; y < foreheight; y++)
		for (int x = 0; x < forewidth; x++) {
			if (x >= x_offset && x < forewidth - x_offset && y >= y_offset && y < backheight + y_offset) {
				//https://stackoverflow.com/a/64655571
				uint8_t alpha_out = foreground_4c[(y * foreheight + x) * 4 + 3] + (background_4c[((y - y_offset) * backwidth + (x - x_offset)) * 4 + 3] * (FF - foreground_4c[(y * foreheight + x) * 4 + 3]) / FF);
				for (int ch = 0; ch < 3; ch++)
					out[(y * forewidth + x) * 4 + ch] = (foreground_4c[(y * forewidth + x) * 4 + ch] * foreground_4c[(y * forewidth + x) * 4 + 3] + background_4c[((y - y_offset) * backwidth + (x - x_offset)) * 4 + ch] * FF * (FF - foreground_4c[(y * forewidth + x) * 4 + 3]) / FF) / alpha_out;
				out[(y * forewidth + x) * 4 + 3] = alpha_out;
			}
			else {
				for (int ch = 0; ch < 4; ch++)
					out[(y * forewidth + x) * 4 + ch] = foreground_4c[(y * forewidth + x) * 4 + ch];
			}
		}
	free(foreground_4c);
	free(background_4c);
}

unsigned char* invert_pixels(unsigned char* input, int width, int height, int channels) {
	for (int i = 0; i < width * height * channels; i += channels) {
		input[i] = 0xFF - input[i];//r
		input[i+1] = 0xFF - input[i+1];//g
		input[i+2] = 0xFF - input[i+2];//b
	}
	return input;
}

std::string tolowerstr(std::string str) {
	for (char& i : str)
		i = tolower(i);
	return str;
}

std::string toupperstr(std::string str) {
	for (char& i : str)
		i = toupper(i);
	return str;
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

void removeQuotes(std::string& str) {
	std::string out;
	for (const auto& c : str)
		if (c != '\"') out += c;//pass through without the " if it has it there
	str = out;
}

void crop_pixels(const unsigned char* input, int width, int height, int channels, unsigned char* output, int x_offset, int y_offset, int out_w, int out_h) {
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++) {
			if (x >= x_offset && x < width - x_offset && y >= y_offset && y < height - y_offset) {
				for (int ch = 0; ch < channels; ch++)
					output[((y - y_offset) * out_w + (x - x_offset)) * channels + ch] = input[(y * width + x) * channels + ch];
			}
		}
}