#include "VidInjector9001.hpp"

//globals
#define pause betterPause();
#define cls Clear();

unsigned long amount = 0;
std::string name = "";
std::string type;
std::string longname;//you need this global because i said so also because make cia needs it too
bool MultiVid;//false if single video, true if multi video
bool Debug;//false if off true if on
char completed[8] { ' ' };
char scompleted[6] { ' ' };

//small functions
std::size_t strlen(const std::string& str) {//http://www.cplusplus.com/forum/beginner/192031/#msg925794
	std::size_t length = 0;
	for (char c : str) {
		if ((c & 0xC0) != 0x80) {
			++length;
		}
	}
	return length;
}

void copyfile(std::string inpath, std::string outpath) {//also works with directories
	if(std::filesystem::exists(outpath))
		std::filesystem::remove_all(outpath);
	if(std::filesystem::exists(inpath))
		std::filesystem::copy(inpath, outpath, std::filesystem::copy_options::recursive); 
}

void betterPause() {
	puts("Press Enter to continue . . .");
	std::cin.ignore();
	std::cin.sync();//clears it for if you typed any characters
}

std::string tolowerstr(std::string str) {
	for (char &i : str)
		i = tolower(i);
	return str;
}

bool stoul_s(unsigned long &output, std::string input, bool isHex = false) {
    std::string lowinput(tolowerstr(input));
    if(isHex) {
        if(lowinput.find_first_of("0123456789abcdef") != 0) return false;
        else output = std::stoul(lowinput, nullptr, 16);
    } else {
        if(lowinput.find_first_of("0123456789") != 0) return false;
        else output = std::stoul(lowinput);
    }
    return true;
}

//this cp437 to utf8 stff (shamelessly) stolen from https://github.com/justinasci/CP437TOUTF8/blob/master/437toUtf8.cpp
inline std::string int2utf8 (int cp) {
	if(cp == 0) return " ";

	if(cp <= 0x7F && cp > 0x00) // 1 byte
	{
		std::string out ("_");
		out[0] = (char) cp;
		return out;
	}
	else if((cp >= 0x80) && (cp <= 0x07FF)) // 2 bytes
	{
		std::string out ("__");
		char a = (cp & 0x003F) + 0x80;
		char b = ((cp &0x07C0) >> 6) + 0xC0 ;
		out[0] = b;
		out[1] = a;
		return out;
	}
	else if( (cp >= 0x0800) && (cp<= 0xFFFF) ) // 3 bytes
	{
		std::string out("___");
		char a = ((cp & 0xF000) >> 12) + 0x00E0;
		char b = ((cp & 0xFC0) >> 6) + 0x80;
		char c = ( cp & 0x003F) + 0x80;

		out[0] = a;
		out[1] = b;
		out[2] = c;
		return out;
	}
	else  if ( cp >= 0x10000 && cp <= 0x10FFFF) // 4 bytes
	{
		std::string out("____");

		char a = ((cp & 0x3F000) >>  18) + 0xF0;
		char b = ((cp & 0x3F000) >>  12) + 0x80;
		char c = ((cp & 0xFC0) >> 6) + 0x80;
		char d = ( cp & 0x003F) + 0x80;

		out[0] = a;
		out[1] = b;
		out[2] = c;
		out[3] = d;

		return out;
	}
	return "~";
}

inline std::string CP437toUTF8(std::string source) {
	int table[128] = {
		0xC7, 0xFC, 0xE9, 0xE2, 0xE4, 0xE0, 0xE5, 0xE7, 0xEA, 0xEB, 0xE8, 0xEF, 0xEE, 0xEC, 0xC4, 0xC5, 0xC9, 0xE6, 0xC6, 0xF4, 0xF6, 0xF2, 0xFB, 0xF9, 0xFF, 0xD6, 0xDC, 0xA2, 0xA3, 0xA5, 0x20A7, 0x192, 0xE1, 0xED, 0xF3, 0xFA, 0xF1, 0xD1, 0xAA, 0xBA, 0xBF, 0x2310, 0xAC, 0xBD, 0xBC, 0xA1, 0xAB, 0xBB, 0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556, 0x2555, 0x2563, 0x2551, 0x2557, 0x255D, 0x255C, 0x255B, 0x2510, 0x2514, 0x2534, 0x252C, 0x251C, 0x2500, 0x253C, 0x255E, 0x255F, 0x255A, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256C, 0x2567, 0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256B, 0x256A, 0x2518, 0x250C, 0x2588, 0x2584, 0x258C, 0x2590, 0x2580, 0x3B1, 0xDF, 0x393, 0x3C0, 0x3A3, 0x3C3, 0xB5, 0x3C4, 0x3A6, 0x398, 0x3A9, 0x3B4, 0x221E, 0x3C6, 0x3B5, 0x2229, 0x2261, 0xB1, 0x2265, 0x2264, 0x2320, 0x2321, 0xF7, 0x2248, 0xB0, 0x2219, 0xB7, 0x221A, 0x207F, 0xB2, 0x25A0, 0xA0
	};
	std::string dest = "";
	for(auto &c : source)
	{
		unsigned char o = c;
		int t = (int)o;

		if(t < 127)
			dest += c;
		else
		{
			dest += int2utf8(table[t-128]);
		}
	}
	return dest;
}

//this UTF stuff is from libctru lol
#ifdef SIZE_MAX
#define _SIZE_MAX ((SIZE_MAX) >> 1)
#endif

//https://github.com/devkitPro/libctru/raw/4e25fb1d6c2ea124a9011c4b65f76f2968a9fb97/libctru/source/util/utf/encode_utf16.c
inline ssize_t
encode_utf16(uint16_t *out,
             uint32_t in)
{
  if(in < 0x10000)
  {
    if(out != NULL)
      *out++ = in;
    return 1;
  }
  else if(in < 0x110000)
  {
    if(out != NULL)
    {
      *out++ = (in >> 10) + 0xD7C0;
      *out++ = (in & 0x3FF) + 0xDC00;
    }
    return 2;
  }

  return -1;
}

//https://github.com/devkitPro/libctru/raw/4e25fb1d6c2ea124a9011c4b65f76f2968a9fb97/libctru/source/util/utf/decode_utf8.c
inline ssize_t
decode_utf8(uint32_t      *out,
            const uint8_t *in)
{
  uint8_t code1, code2, code3, code4;

  code1 = *in++;
  if(code1 < 0x80)
  {
    /* 1-byte sequence */
    *out = code1;
    return 1;
  }
  else if(code1 < 0xC2)
  {
    return -1;
  }
  else if(code1 < 0xE0)
  {
    /* 2-byte sequence */
    code2 = *in++;
    if((code2 & 0xC0) != 0x80)
    {
      return -1;
    }

    *out = (code1 << 6) + code2 - 0x3080;
    return 2;
  }
  else if(code1 < 0xF0)
  {
    /* 3-byte sequence */
    code2 = *in++;
    if((code2 & 0xC0) != 0x80)
    {
      return -1;
    }
    if(code1 == 0xE0 && code2 < 0xA0)
    {
      return -1;
    }

    code3 = *in++;
    if((code3 & 0xC0) != 0x80)
    {
      return -1;
    }

    *out = (code1 << 12) + (code2 << 6) + code3 - 0xE2080;
    return 3;
  }
  else if(code1 < 0xF5)
  {
    /* 4-byte sequence */
    code2 = *in++;
    if((code2 & 0xC0) != 0x80)
    {
      return -1;
    }
    if(code1 == 0xF0 && code2 < 0x90)
    {
      return -1;
    }
    if(code1 == 0xF4 && code2 >= 0x90)
    {
      return -1;
    }

    code3 = *in++;
    if((code3 & 0xC0) != 0x80)
    {
      return -1;
    }

    code4 = *in++;
    if((code4 & 0xC0) != 0x80)
    {
      return -1;
    }

    *out = (code1 << 18) + (code2 << 12) + (code3 << 6) + code4 - 0x3C82080;
    return 4;
  }

  return -1;
}

//https://github.com/devkitPro/libctru/raw/4e25fb1d6c2ea124a9011c4b65f76f2968a9fb97/libctru/source/util/utf/utf8_to_utf16.c
inline ssize_t
utf8_to_utf16(uint16_t      *out,
              const uint8_t *in,
              size_t        len)
{
  ssize_t  rc = 0;
  ssize_t  units;
  uint32_t code;
  uint16_t encoded[2];

  do
  {
    units = decode_utf8(&code, in);
    if(units == -1)
      return -1;

    if(code > 0)
    {
      in += units;

      units = encode_utf16(encoded, code);
      if(units == -1)
        return -1;

      if(out != NULL)
      {
        if(rc + units <= len)
        {
          *out++ = encoded[0];
          if(units > 1)
            *out++ = encoded[1];
        }
      }

      if(_SIZE_MAX - units >= rc)
        rc += units;
      else
        return -1;
    }
  } while(code > 0);

  return rc;
}

std::string UTF8toUTF16(std::string input) {//not to be confused with utf8_to_utf16
	std::string output = "";
	uint8_t *utf8 = new uint8_t[input.size()];
	uint16_t *utf16 = new uint16_t[strlen(input) * 2];
	memcpy(utf8, input.c_str(), input.size());
	if(utf8_to_utf16(utf16, utf8, input.size()) == -1) {//it failed. go max mode
		std::string something = UTF8toUTF16(CP437toUTF8(input));//i cant wait for this to cause more problems than it solves
		delete[] utf8;
		delete[] utf16;
		//puts("WARNING: Failed to convert UTF-8 to UTF-16. (input encoding is not UTF-8)\nThe output may not be as expected.");//exlude this because this function is so weird and it gets called like 8 times even when the text is fine idek how it works
		//pause
		return something;
	}
	//printf("result: %i\n", res);
	char utf16str[strlen(input) * 2];
	memcpy(utf16str, utf16, strlen(input) * 2);

	for (size_t i = 0; i < strlen(input) * 2; i++)
		output += utf16str[i];
	
	delete[] utf8;
	delete[] utf16;
	return output;
}

void image_data_to_tiles(void* out, void* img, uint32_t width, uint32_t height) {//from bannertool, edited for rgb565 only
    for(uint32_t y = 0; y < height; y++) {
        for(uint32_t x = 0; x < width; x++) {
            uint32_t index = (((y >> 3) * (width >> 3) + (x >> 3)) << 6) + ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3));

            uint8_t* pixel = &((uint8_t*) img)[(y * width + x) * 3];
            uint16_t color = 0;
            color = (uint16_t) ((((uint8_t) (pixel[0]) & ~0x7) << 8) | (((uint8_t) (pixel[1]) & ~0x3) << 3) | ((uint8_t) (pixel[2]) >> 3));

            ((uint16_t*) out)[index] = color;
        }
    }
}

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
	if(!stbi_info(input.c_str(), &w, &h, &comp)) {
		puts("ERROR: Failed to get image info.");
		return false;
	}
	input_pixels = stbi_load(input.c_str(), &w, &h, &ch, 0);
	output_pixels = (unsigned char*) malloc(out_w*out_h*ch);
	if(w == out_w && h == out_h) memcpy(output_pixels, input_pixels, w*h*ch);
	else stbir_resize_uint8(input_pixels, w, h, 0, output_pixels, out_w, out_h, 0, ch);//scale to 200x120 if needed
	stbi_image_free(input_pixels);

	if(ch == 4) {//if png?
		output_3c = (unsigned char*) malloc(out_w*out_h*3);
		for (int i = 3; i < out_w*out_h*ch; i+=4) {//make background all white
			//https://stackoverflow.com/a/64655571
			uint8_t alpha_out = output_pixels[i] + (FF * (FF - output_pixels[i]) / FF);
			output_pixels[i-1] = (output_pixels[i-1] * output_pixels[i] + FF * FF * (FF - output_pixels[i]) / FF)/alpha_out;
			output_pixels[i-2] = (output_pixels[i-2] * output_pixels[i] + FF * FF * (FF - output_pixels[i]) / FF)/alpha_out;
			output_pixels[i-3] = (output_pixels[i-3] * output_pixels[i] + FF * FF * (FF - output_pixels[i]) / FF)/alpha_out;
			output_pixels[i] = alpha_out;
		}
		int newi = 3;
		for(int i = 3; i < out_w*out_h*4; i+=4) {//remove alpha channel
			output_3c[newi-3] = output_pixels[i-3];
			output_3c[newi-2] = output_pixels[i-2];
			output_3c[newi-1] = output_pixels[i-1];
			newi+=3;
		}
		free(output_pixels);
	}
	if(ch == 3) {
		output_3c = (unsigned char*) malloc(out_w*out_h*3);
		memcpy(output_3c, output_pixels, out_w*out_h*3);
		free(output_pixels);
	}
	
	//layer 200x120 image on a 256x128 image
	output_fin = (unsigned char*) malloc(new_w*new_h*3);
	memset(output_fin, 0, new_w*new_h*3);
	for (int y = 0; y < out_h; y++)
		for (int x = 0; x < out_w; x++) {
			output_fin[(y*(new_w)+x)*3] = output_3c[(y*(out_w)+x)*3];
			output_fin[(y*(new_w)+x)*3+1] = output_3c[(y*(out_w)+x)*3+1];
			output_fin[(y*(new_w)+x)*3+2] = output_3c[(y*(out_w)+x)*3+2];
		}

	unsigned char tiledbanner[65536];
	image_data_to_tiles(tiledbanner, output_fin, new_w, new_h);
	if(writeHeader) {
		memcpy(outBuffer, bimgheader, sizeof(bimgheader));
		memcpy(outBuffer + sizeof(bimgheader), tiledbanner, sizeof(tiledbanner));
	}
	else {
		memcpy(outBuffer, tiledbanner, sizeof(tiledbanner));
	}

	//stbi_write_png("imag.png", new_w, new_h, 3, output_fin, 0);
	free(output_3c);
	return true;
}

//note: text has to be utf16
bool convertToIcon(std::string input, std::string output, std::string shortname, std::string longname, std::string publisher) {//bare bones SMDH creation. thanks 3dbrew
	unsigned char* input_pixels;
	unsigned char* output_pixels;
	unsigned char* large_3c;
	unsigned char* small_3c;
	int w, h, ch, comp;
	const int largeLW = 48;
	const int smallLW = 24;
	const uint8_t FF = 0xFF;
	if(!stbi_info(input.c_str(), &w, &h, &comp)) {
		puts("ERROR: Failed to get image info.");
		return false;
	}
	input_pixels = stbi_load(input.c_str(), &w, &h, &ch, 0);
	output_pixels = (unsigned char*) malloc(largeLW*largeLW*ch);
	if(w == largeLW && h == largeLW) memcpy(output_pixels, input_pixels, w*h*ch);
	else stbir_resize_uint8(input_pixels, w, h, 0, output_pixels, largeLW, largeLW, 0, ch);//scale to 48x48 if needed

	if(ch == 4) {//if png?
		large_3c = (unsigned char*) malloc(largeLW*largeLW*3);
		for (int i = 3; i < largeLW*largeLW*ch; i+=4) {//make background all white
			//https://stackoverflow.com/a/64655571
			uint8_t alpha_out = output_pixels[i] + (FF * (FF - output_pixels[i]) / FF);
			output_pixels[i-1] = (output_pixels[i-1] * output_pixels[i] + FF * FF * (FF - output_pixels[i]) / FF)/alpha_out;
			output_pixels[i-2] = (output_pixels[i-2] * output_pixels[i] + FF * FF * (FF - output_pixels[i]) / FF)/alpha_out;
			output_pixels[i-3] = (output_pixels[i-3] * output_pixels[i] + FF * FF * (FF - output_pixels[i]) / FF)/alpha_out;
			output_pixels[i] = alpha_out;
		}
		int newi = 3;
		for(int i = 3; i < largeLW*largeLW*4; i+=4) {
			large_3c[newi-3] = output_pixels[i-3];
			large_3c[newi-2] = output_pixels[i-2];
			large_3c[newi-1] = output_pixels[i-1];
			newi+=3;
		}
		free(output_pixels);
	}
	if(ch == 3) {
		large_3c = (unsigned char*) malloc(largeLW*largeLW*3);
		for(int i = 0; i < largeLW*largeLW*3; i++) large_3c[i] = output_pixels[i];
		free(output_pixels);
	}

	small_3c = (unsigned char*) malloc(smallLW*smallLW*3);
	stbir_resize_uint8(large_3c, largeLW, largeLW, 0, small_3c, smallLW, smallLW, 0, 3);//make the small icon
	unsigned char tiledsmall[0x480];
	unsigned char tiledlarge[0x1200];
	image_data_to_tiles(tiledsmall, small_3c, smallLW, smallLW);
	image_data_to_tiles(tiledlarge, large_3c, largeLW, largeLW);
	std::ofstream smdh(output, std::ios_base::out | std::ios_base::binary);
	smdh << "SMDH";//make smdh!
	for(int i = 0; i < 4; i++)
		smdh << '\0';
	for(int i = 0; i < 16; i++) {
		smdh << shortname;
		for(size_t i = 0; i < 0x80 - shortname.size(); i++)
			smdh << '\0';
		smdh << longname;
		for(size_t i = 0; i < 0x100 - longname.size(); i++)
			smdh << '\0';
		smdh << publisher;
		for(size_t i = 0; i < 0x80 - publisher.size(); i++)
			smdh << '\0';
	}
	for(int i = 0; i < 0x10; i++)
		smdh << '\0';
	smdh << "\xFF\xFF\xFF\x7F";//region free
	for(int i = 0; i < 0xC; i++)
		smdh << '\0';
	smdh << "\x01\x04";//visible, no save backups
	for(int i = 0; i < 0x16; i++)
		smdh << '\0';
	smdh.write(reinterpret_cast<const char*>(tiledsmall), sizeof(tiledsmall));
	smdh.write(reinterpret_cast<const char*>(tiledlarge), sizeof(tiledlarge));
	

	//stbi_write_png(output.c_str(), largeLW, largeLW, 3, large_3c, 0);
	stbi_image_free(input_pixels);
	free(small_3c);
	free(large_3c);
	return true;
}

//these are from bannertool with slight edits
#define CBMD_NUM_CGFXS 14

typedef struct {
	char magic[4];
	uint32_t zero;
	uint32_t cgfxOffsets[CBMD_NUM_CGFXS];
	uint8_t padding[0x44];
	uint32_t cwavOffset;
} CBMDHeader;

typedef struct {
    void* cgfxs[CBMD_NUM_CGFXS];
    uint32_t cgfxSizes[CBMD_NUM_CGFXS];
    void* cwav;
    uint32_t cwavSize;
} CBMD;

#define MIN(a,b) (((a)<(b))?(a):(b))

// Ported from: https://github.com/svn2github/3DS-Explorer/blob/master/3DSExplorer/DSDecmp/Formats/Nitro/LZ11.cs

uint32_t lz11_get_occurence_length(uint8_t* newPtr, uint32_t newLength, uint8_t* oldPtr, uint32_t oldLength, uint32_t* disp) {
    if(disp != NULL) {
        *disp = 0;
    }

    if(newLength == 0) {
        return 0;
    }

    uint32_t maxLength = 0;
    if(oldLength > 0) {
        for(uint32_t i = 0; i < oldLength - 1; i++) {
            uint8_t* currentOldStart = oldPtr + i;
            uint32_t currentLength = 0;
            for(uint32_t j = 0; j < newLength; j++) {
                if(*(currentOldStart + j) != *(newPtr + j)) {
                    break;
                }

                currentLength++;
            }

            if(currentLength > maxLength) {
                maxLength = currentLength;
                if(disp != NULL) {
                    *disp = oldLength - i;
                }

                if(maxLength == newLength) {
                    break;
                }
            }
        }
    }

    return maxLength;
}

void* lz11_compress(uint32_t* size, void* input, uint32_t inputSize) {
    if(inputSize > 0xFFFFFF) {
        printf("ERROR: LZ11 input is too large.\n");
        return NULL;
    }

    std::stringstream ss;

    uint8_t header[4] = { 0x11, (uint8_t) (inputSize & 0xFF), (uint8_t) ((inputSize >> 8) & 0xFF), (uint8_t) ((inputSize >> 16) & 0xFF) };
    ss.write((char*) header, 4);

    uint32_t compressedLength = 4;
    uint8_t outbuffer[8 * 4 + 1];
    outbuffer[0] = 0;
    uint32_t bufferlength = 1;
    uint32_t bufferedBlocks = 0;
    uint32_t readBytes = 0;
    while(readBytes < inputSize) {
        if(bufferedBlocks == 8) {
            ss.write((char*) outbuffer, bufferlength);
            compressedLength += bufferlength;
            outbuffer[0] = 0;
            bufferlength = 1;
            bufferedBlocks = 0;
        }

        uint32_t disp = 0;
        uint32_t oldLength = MIN(readBytes, 0x1000);
        uint32_t length = lz11_get_occurence_length((uint8_t*) input + readBytes, MIN(inputSize - readBytes, 0x10110), (uint8_t*) input + readBytes - oldLength, oldLength, &disp);
        if(length < 3) {
            outbuffer[bufferlength++] = *((uint8_t*) input + (readBytes++));
        } else {
            readBytes += length;
            outbuffer[0] |= (uint8_t)(1 << (7 - bufferedBlocks));
            if(length > 0x110) {
                outbuffer[bufferlength] = 0x10;
                outbuffer[bufferlength] |= (uint8_t)(((length - 0x111) >> 12) & 0x0F);
                bufferlength++;
                outbuffer[bufferlength] = (uint8_t)(((length - 0x111) >> 4) & 0xFF);
                bufferlength++;
                outbuffer[bufferlength] = (uint8_t)(((length - 0x111) << 4) & 0xF0);
            } else if(length > 0x10) {
                outbuffer[bufferlength] = 0x00;
                outbuffer[bufferlength] |= (uint8_t)(((length - 0x111) >> 4) & 0x0F);
                bufferlength++;
                outbuffer[bufferlength] = (uint8_t)(((length - 0x111) << 4) & 0xF0);
            } else {
                outbuffer[bufferlength] = (uint8_t)(((length - 1) << 4) & 0xF0);
            }

            outbuffer[bufferlength] |= (uint8_t)(((disp - 1) >> 8) & 0x0F);
            bufferlength++;
            outbuffer[bufferlength] = (uint8_t)((disp - 1) & 0xFF);
            bufferlength++;
        }

        bufferedBlocks++;
    }

    if(bufferedBlocks > 0) {
        ss.write((char*) outbuffer, bufferlength);
        compressedLength += bufferlength;
    }

    if(compressedLength % 4 != 0) {
        uint32_t padLength = 4 - (compressedLength % 4);
        uint8_t pad[padLength];
        memset(pad, 0, (size_t) padLength);

        ss.write((char*) pad, padLength);
        compressedLength += padLength;
    }

    void* buf = malloc((size_t) compressedLength);
    ss.read((char*) buf, compressedLength);

    if(size != NULL) {
        *size = (uint32_t) compressedLength;
    }

    return buf;
}

static void* cbmd_build_data(uint32_t* size, CBMD cbmd) {
	CBMDHeader header;
	memset(&header, 0, sizeof(header));

	memcpy(header.magic, "CBMD", sizeof(header.magic));

	uint32_t outputSize = sizeof(CBMDHeader);

	void* compressedCGFXs[14] = {NULL};
	uint32_t compressedCGFXSizes[14] = {0};
	for(uint32_t i = 0; i < CBMD_NUM_CGFXS; i++) {
		if(cbmd.cgfxs[i] != NULL) {
			header.cgfxOffsets[i] = outputSize;

			compressedCGFXs[i] = lz11_compress(&compressedCGFXSizes[i], cbmd.cgfxs[i], cbmd.cgfxSizes[i]);
			outputSize += compressedCGFXSizes[i];
		}
	}

	outputSize = (outputSize + 0xF) & ~0xF;

	if(cbmd.cwav != NULL) {
		header.cwavOffset = outputSize;

		outputSize += cbmd.cwavSize;
	}

	void* output = calloc(outputSize, sizeof(uint8_t));
	if(output == NULL) {
		for(uint32_t i = 0; i < CBMD_NUM_CGFXS; i++) {
			if(cbmd.cgfxs[i] != NULL) {
				free(compressedCGFXs[i]);
			}
		}

		puts("ERROR: Could not allocate memory for CBMD data.");
		return NULL;
	}

	memcpy(output, &header, sizeof(header));

	for(uint32_t i = 0; i < CBMD_NUM_CGFXS; i++) {
		if(compressedCGFXs[i] != NULL) {
			memcpy(&((uint8_t*) output)[header.cgfxOffsets[i]], compressedCGFXs[i], compressedCGFXSizes[i]);
			free(compressedCGFXs[i]);
		}
	}

	if(cbmd.cwav != NULL) {
		memcpy(&((uint8_t*) output)[header.cwavOffset], cbmd.cwav, cbmd.cwavSize);
	}

	if(size != NULL) {
		*size = outputSize;
	}

	return output;
}

void removeQuotes(std::string &str) {
	std::string out;
	for (const auto &c : str)
		if(c != '\"') out += c;//pass through without the " if it has it there
	str = out;
}

void removeInvalids(std::string &str) {//remove more invalid characters
	std::string out;
	for (const auto &c : str) {
		if(c != '\\' && c != '/' && c != ':' && c != '<' && c != '>' && c != '*' && c != '?' && c != '|') out += c;
	}
	str = out;
}

std::string system_g(std::string input) {//system_g()! It's system(), but good!
	return system(input.c_str()), input;
}

bool goorQuit() {//true for go, false for quit
	std::string opt = "";
	puts("Press Enter to continue or Q+Enter to quit job . . .");
	std::getline(std::cin, opt);
	return tolower(opt[0]) != 'q';
}

void Clear() {//https://stackoverflow.com/a/52895729
	#if defined(_WIN32)
		system("cls");
	#else
		system("clear");
	#endif
}

void windowTitle(std::string title) {
	#if defined(_WIN32)
		system_g("title " + title);
	#else
		printf("%c]0;%s%c", '\033', title.c_str(), '\007');//https://stackoverflow.com/questions/2218159
	#endif
}

void Generate_Files(bool Multi) {
    puts("Generating files");
	miniz_cpp::zip_file file;
	file.load(Multi ? Multivid : Singlevid);
	std::vector<std::string> list = file.namelist();
	for(auto &member : list) {//plant seeds
		if(member.find_last_of("/") == member.size()-1)
			std::filesystem::create_directory(member);
	}
	file.extractall(".", list);//grow fruit
}

bool readTxt(std::string file, std::string &output) {//return true if it's untf16 or else false, who knows what that could be though
	std::string realoutput = "";
	std::string choiche = "";
	while(1) {
		puts("Use the contents of this .txt file for the input? (Y/N)");
		std::getline(std::cin, choiche);
		if(tolower(choiche[0]) == 'y') {
			removeQuotes(file);
			if(std::filesystem::exists(file)) {
				choiche = "";
				output = "";
				std::ifstream input(file, std::ios_base::in | std::ios_base::binary);//input file

				char Byte;
				input.read(&Byte, 1);//grab first byte of file
				while (input) {//continue until input stream fails
					output += Byte;//append byte to string
					//printf("%1X\n", Byte);
					input.read(&Byte, 1);//grab next byte of file
				}
				input.close();
				if((output[0] & 0xFF) == 0xFF && (output[1] & 0xFF) == 0xFE) {//if utf16 little endian
					output = output.substr(2);//remove bom
					return true;
				}
				else if((output[0] & 0xFF) == 0xFE && (output[1] & 0xFF) == 0xFF) {//if utf16 big endian
					for (size_t i = 2; i < output.size(); i+=2) {//swap every other byte
						realoutput += output[i+1];
						realoutput += output[i];
					}
					output = realoutput;//why doesnt this one die if it sees a null byte?
					return true;
				}
				else if((UTF8toUTF16(output)[0] & 0xFF) == 0xFF && (UTF8toUTF16(output)[1] & 0xFF) == 0xFE) {//if utf8
					output = output.substr(3);//remove bom (it is 3 bytes in utf8)
				}
				return false;
			}
			else {
				puts("ERROR: file does not exist\nReturning input as output...");
				return false; 
			}
		}
		else if(tolower(choiche[0]) == 'n') {
			puts("Returning input as output...");
			output = file;
			return false;
		}
		cls
	}
}

//big functions
void setAmount() {
	windowTitle("[MultiVidInjector5000] Set video amount");
	static bool good = false;
	good = false;//in case you do it again, idk why the line above this isnt enough also why should anyone be able to do it again? bruhh
	if (amount) amount = 0;
	std::filesystem::create_directories("romfs/settings");
	std::ofstream movie_bnrname("romfs/settings/movie_bnrname.csv", std::ios_base::out | std::ios_base::binary);
	while(!good) {
		cls
		puts("Enter the amount of videos you have:");
		std::getline(std::cin, name);
		if(name.size() > 2 || !stoul_s(amount, name) || amount > 27) {
			puts("Invalid input/The amount cannot be greater than 27. Try again");
			pause
		} else {
			movie_bnrname << "\xFF\xFE" + UTF8toUTF16(std::to_string(amount) + "\x0D\x0A");
			for (unsigned long i = 0; i < amount; i++) {
				movie_bnrname << UTF8toUTF16("movie_" + std::to_string(i) + ".bimg\x0D\x0A");
			}
			movie_bnrname.close();
			good = true;
		}
	}
	if(!std::filesystem::exists("romfs/settings/movie_bnrname.csv")) {
		puts("ERROR: Failed to generate romfs/settings/movie_bnrname.csv");
		pause
		return;
	}
}

void Movie_title() {
	type = MultiVid ? "MultiVidInjector5000" : "VidInjector9001";
	windowTitle("[" + type + "] Generate movie_title.csv");
	cls
	completed[0] = ' ';
	scompleted[0] = ' ';
	if (amount == 0) {
		puts("Set video amount first!");
		pause
		return;
	}
	std::filesystem::create_directories("romfs/movie");
	std::ofstream movie_title("romfs/movie/movie_title.csv", std::ios_base::out | std::ios_base::binary);
	bool utf16;
	movie_title << "\xFF\xFE" + UTF8toUTF16("#JP,#EN,#FR,#GE,#IT,#SP,#CH,#KO,#DU,#PO,#RU,#TW\x0D\x0A");
	for (unsigned long i = 0; i < amount; i++) {
		utf16 = false;
		name = "";
		while(name == "") {
			if(MultiVid) printf("Enter the title of video #%li\n", i+1);
			else puts("Enter the title of the video");
			std::getline(std::cin, name);
			if(name == "") cls
		}
		if(tolowerstr((std::string)&name[name.size()-4]) == ".txt" || tolowerstr((std::string)&name[name.size()-5]) == ".txt\"") {
			if(readTxt(name, name)) utf16 = true;
		}
		for(unsigned long long i = 0; i < name.size(); i++) {
			if(name[i] == ',') {
				std::string choiche = "";
				while(choiche == "") {
					printf("WARNING: Character #%lld (',') in the title will be a problem.\nReplace it with \"\\x2C\" to have a comma in the title? [Y/N]\n", i+1);
					std::getline(std::cin, choiche);
					if(tolower(choiche[0]) == 'y') {
						name[i] = '\\';
						name.insert(i+1, "x2C");
					}
					else if(tolower(choiche[0]) != 'n') {
						choiche = "";
						cls
					}
				}
			}
		}
		for (int j = 0; j < 11; j++) {//do it 11 times because it needs to
			if(utf16) movie_title << name + UTF8toUTF16(",");
			else movie_title << UTF8toUTF16(name + ",");
		}
		//put the last stuff
		if(utf16) movie_title << name + UTF8toUTF16("\x0D\x0A");
		else movie_title << UTF8toUTF16(name + "\x0D\x0A");
	}
	movie_title.close();
	if(!std::filesystem::exists("romfs/movie/movie_title.csv")) {
		puts("ERROR: Failed to generate romfs/movie/movie_title.csv");
		pause
		return;
	}
	completed[0] = 'X';
	scompleted[0] = 'X';
	pause
}

void makesettingsTL() {
	type = MultiVid ? "MultiVidInjector5000" : "VidInjector9001";
	windowTitle("[" + type + "] Generate settingsTL.csv");
	cls
	completed[1] = ' ';
	scompleted[1] = ' ';
	if (amount == 0) {
		puts("Set video amount first!");
		pause
		return;
	}

	std::string publisher = "";
	std::string buttons = "";
	std::string gentleness = "";
	bool utf16 = false;
	
	name = "";
	while(name == "") {
		if(MultiVid) puts("Enter the name of the series");
		else puts("Enter the name of the video");
		std::getline(std::cin, name);
	}
	if(tolowerstr((std::string)&name[name.size()-4]) == ".txt" || tolowerstr((std::string)&name[name.size()-5]) == ".txt\"") {
		if(readTxt(name, name)) utf16 = true;
	}
	for(unsigned long long i = 0; i < name.size(); i++) {
		if(name[i] == ',') {
			std::string choiche = "";
			while(choiche == "") {
				printf("WARNING: Character #%lld (',') in the name will be a problem.\nReplace it with \"\\x2C\" to have a comma in the name? [Y/N]\n", i+1);
				std::getline(std::cin, choiche);
				if(tolower(choiche[0]) == 'y') {
					name[i] = '\\';
					name.insert(i+1, "x2C");
				}
				else if(tolower(choiche[0]) != 'n') {
					choiche = "";
					cls
				}
			}
		}
	}
	cls

	buttons = "";
	while(buttons == "") {
		puts("Do you want fast forward and rewind buttons? [Y/N]");
		std::getline(std::cin, buttons);
		if(tolower(buttons[0]) == 'y') buttons = "true";
		else if(tolower(buttons[0]) == 'n') buttons = "false";
		else {
			buttons = "";
			cls
		}
	}

	gentleness = "";
	while(gentleness == "") {
		puts("Do you want the bottom screen to fade after a while? [Y/N]");
		std::getline(std::cin, gentleness);
		if(tolower(gentleness[0]) == 'y') gentleness = "true";
		else if(tolower(gentleness[0]) == 'n') gentleness = "false";
		else {
			gentleness = "";
			cls
		}
	}
	
	std::filesystem::create_directories("romfs/settings");
	std::ofstream settingsTL("romfs/settings/settingsTL.csv", std::ios_base::out | std::ios_base::binary);
	settingsTL << "\xFF\xFE" + 
				UTF8toUTF16("# おしらせURL\x0D\x0A"//hard to read because of line breaks but hey better than hex
							"# JP:\x0D\x0A"
							"none\x0D\x0A"
							"\x0D\x0A"
							"# EN:\x0D\x0A"
							"none\x0D\x0A"
							"\x0D\x0A"
							"# FR:\x0D\x0A"
							"none\x0D\x0A"
							"\x0D\x0A"
							"# GE:\x0D\x0A"
							"none\x0D\x0A"
							"\x0D\x0A"
							"# IT:\x0D\x0A"
							"none\x0D\x0A"
							"\x0D\x0A"
							"# SP:\x0D\x0A"
							"none\x0D\x0A"
							"\x0D\x0A"
							"# CN:\x0D\x0A"
							"none\x0D\x0A"
							"\x0D\x0A"
							"# KO:\x0D\x0A"
							"none\x0D\x0A"
							"\x0D\x0A"
							"# DU:\x0D\x0A"
							"none\x0D\x0A"
							"\x0D\x0A"
							"# PO:\x0D\x0A"
							"none\x0D\x0A"
							"\x0D\x0A"
							"# RU:\x0D\x0A"
							"none\x0D\x0A"
							"\x0D\x0A"
							"# TW:\x0D\x0A"
							"none\x0D\x0A"
							"\x0D\x0A"
							"# アプリ名（ロングネーム）\x0D\x0A"//long name
							"# JP:\x0D\x0A");
	if(utf16) settingsTL << name + UTF8toUTF16("\x0D\x0A");//euh
	else settingsTL << UTF8toUTF16(name + "\x0D\x0A");
  settingsTL << UTF8toUTF16("\x0D\x0A"
							"# EN:\x0D\x0A");
	if(utf16) settingsTL << name + UTF8toUTF16("\x0D\x0A");
	else settingsTL << UTF8toUTF16(name + "\x0D\x0A");
  settingsTL << UTF8toUTF16("\x0D\x0A"
							"# FR:\x0D\x0A");
	if(utf16) settingsTL << name + UTF8toUTF16("\x0D\x0A");
	else settingsTL << UTF8toUTF16(name + "\x0D\x0A");
  settingsTL << UTF8toUTF16("\x0D\x0A"
							"# GE:\x0D\x0A");
	if(utf16) settingsTL << name + UTF8toUTF16("\x0D\x0A");
	else settingsTL << UTF8toUTF16(name + "\x0D\x0A");
  settingsTL << UTF8toUTF16("\x0D\x0A"
							"# IT:\x0D\x0A");
	if(utf16) settingsTL << name + UTF8toUTF16("\x0D\x0A");
	else settingsTL << UTF8toUTF16(name + "\x0D\x0A");
  settingsTL << UTF8toUTF16("\x0D\x0A"
							"# SP:\x0D\x0A");
	if(utf16) settingsTL << name + UTF8toUTF16("\x0D\x0A");
	else settingsTL << UTF8toUTF16(name + "\x0D\x0A");
  settingsTL << UTF8toUTF16("\x0D\x0A"
							"# CN:\x0D\x0A");
	if(utf16) settingsTL << name + UTF8toUTF16("\x0D\x0A");
	else settingsTL << UTF8toUTF16(name + "\x0D\x0A");
  settingsTL << UTF8toUTF16("\x0D\x0A"
							"# KO:\x0D\x0A");
	if(utf16) settingsTL << name + UTF8toUTF16("\x0D\x0A");
	else settingsTL << UTF8toUTF16(name + "\x0D\x0A");
  settingsTL << UTF8toUTF16("\x0D\x0A"
							"# DU:\x0D\x0A");
	if(utf16) settingsTL << name + UTF8toUTF16("\x0D\x0A");
	else settingsTL << UTF8toUTF16(name + "\x0D\x0A");
  settingsTL << UTF8toUTF16("\x0D\x0A"
							"# PO:\x0D\x0A");
	if(utf16) settingsTL << name + UTF8toUTF16("\x0D\x0A");
	else settingsTL << UTF8toUTF16(name + "\x0D\x0A");
  settingsTL << UTF8toUTF16("\x0D\x0A"
							"# RU:\x0D\x0A");
	if(utf16) settingsTL << name + UTF8toUTF16("\x0D\x0A");
	else settingsTL << UTF8toUTF16(name + "\x0D\x0A");
  settingsTL << UTF8toUTF16("\x0D\x0A"
							"# TW:\x0D\x0A");
	if(utf16) settingsTL << name + UTF8toUTF16("\x0D\x0A");
	else settingsTL << UTF8toUTF16(name + "\x0D\x0A");
  settingsTL << UTF8toUTF16("\x0D\x0A"
							"# 拡張セーブデータのID（16進数）\x0D\x0A"
							"12345\x0D\x0A"//idk if any other game uses 0x12345 as its save data ID, nor do i understand if it matters, so sorry if this breaks everything
							"\x0D\x0A"
							"# NADLタスクのID\x0D\x0A"
							"none\x0D\x0A"
							"\x0D\x0A"
							"# タスクの実行間隔（h）（10進数）\x0D\x0A"
							"0\x0D\x0A"
							"\x0D\x0A"
							"# タスクの実行回数（10進数）\x0D\x0A"
							"0\x0D\x0A"
							"\x0D\x0A"
							"# おしらせのあり、なし\x0D\x0A"//not sure what this is, but if you enable it in single vid it instantly crashes
							"false\x0D\x0A"
							"\x0D\x0A"
							"# 早送り、巻戻しボタンのあり、なし\x0D\x0A"
							+ buttons + "\x0D\x0A"
							"\x0D\x0A"
							"# 優しさ演出のあり、なし\x0D\x0A"
							+ gentleness + "\x0D\x0A");
	if(MultiVid) {
		cls
		publisher = "";
		utf16 = false;
		while(publisher == "") {
			puts("Enter the Publisher of the video");
			std::getline(std::cin, publisher);
			if(publisher == "") cls
		}
		if(tolowerstr((std::string)&publisher[publisher.size()-4]) == ".txt" || tolowerstr((std::string)&publisher[publisher.size()-5]) == ".txt\"") {
			if(readTxt(publisher, publisher)) utf16 = true;
		}
		for(unsigned long long i = 0; i < publisher.size(); i++) {
			if(publisher[i] == ',') {
				std::string choiche = "";
				while(choiche == "") {
					printf("WARNING: Character #%lld (',') in the name will be a problem.\nReplace it with \"\\x2C\" to have a comma in the name? [Y/N]\n", i+1);
					std::getline(std::cin, choiche);
					if(tolower(choiche[0]) == 'y') {
						publisher[i] = '\\';
						publisher.insert(i+1, "x2C");
					}
					else if(tolower(choiche[0]) != 'n') {
						choiche = "";
						cls
					}
				}
			}
		}
	settingsTL << UTF8toUTF16("\x0D\x0A"
							"# 動画の数\x0D\x0A"//amount of videos
							+ std::to_string(amount) + "\x0D\x0A"
							"\x0D\x0A"
							"# 動画パブリッシャー名\x0D\x0A"//publisher name
							"# JP:\x0D\x0A");
	if(utf16) settingsTL << publisher + UTF8toUTF16("\x0D\x0A");//euh part II
	else settingsTL << UTF8toUTF16(publisher + "\x0D\x0A");
  settingsTL << UTF8toUTF16("\x0D\x0A"
							"# EN:\x0D\x0A");
	if(utf16) settingsTL << publisher + UTF8toUTF16("\x0D\x0A");
	else settingsTL << UTF8toUTF16(publisher + "\x0D\x0A");
  settingsTL << UTF8toUTF16("\x0D\x0A"
							"# FR:\x0D\x0A");
	if(utf16) settingsTL << publisher + UTF8toUTF16("\x0D\x0A");
	else settingsTL << UTF8toUTF16(publisher + "\x0D\x0A");
  settingsTL << UTF8toUTF16("\x0D\x0A"
							"# GE:\x0D\x0A");
	if(utf16) settingsTL << publisher + UTF8toUTF16("\x0D\x0A");
	else settingsTL << UTF8toUTF16(publisher + "\x0D\x0A");
  settingsTL << UTF8toUTF16("\x0D\x0A"
							"# IT:\x0D\x0A");
	if(utf16) settingsTL << publisher + UTF8toUTF16("\x0D\x0A");
	else settingsTL << UTF8toUTF16(publisher + "\x0D\x0A");
  settingsTL << UTF8toUTF16("\x0D\x0A"
							"# SP:\x0D\x0A");
	if(utf16) settingsTL << publisher + UTF8toUTF16("\x0D\x0A");
	else settingsTL << UTF8toUTF16(publisher + "\x0D\x0A");
  settingsTL << UTF8toUTF16("\x0D\x0A"
							"# CN:\x0D\x0A");
	if(utf16) settingsTL << publisher + UTF8toUTF16("\x0D\x0A");
	else settingsTL << UTF8toUTF16(publisher + "\x0D\x0A");
  settingsTL << UTF8toUTF16("\x0D\x0A"
							"# KO:\x0D\x0A");
	if(utf16) settingsTL << publisher + UTF8toUTF16("\x0D\x0A");
	else settingsTL << UTF8toUTF16(publisher + "\x0D\x0A");
  settingsTL << UTF8toUTF16("\x0D\x0A"
							"# DU:\x0D\x0A");
	if(utf16) settingsTL << publisher + UTF8toUTF16("\x0D\x0A");
	else settingsTL << UTF8toUTF16(publisher + "\x0D\x0A");
  settingsTL << UTF8toUTF16("\x0D\x0A"
							"# PO:\x0D\x0A");
	if(utf16) settingsTL << publisher + UTF8toUTF16("\x0D\x0A");
	else settingsTL << UTF8toUTF16(publisher + "\x0D\x0A");
  settingsTL << UTF8toUTF16("\x0D\x0A"
							"# RU:\x0D\x0A");
	if(utf16) settingsTL << publisher + UTF8toUTF16("\x0D\x0A");
	else settingsTL << UTF8toUTF16(publisher + "\x0D\x0A");
  settingsTL << UTF8toUTF16("\x0D\x0A"
							"# TW:\x0D\x0A");
	if(utf16) settingsTL << publisher + UTF8toUTF16("\x0D\x0A");
	else settingsTL << UTF8toUTF16(publisher + "\x0D\x0A");
  settingsTL << UTF8toUTF16("\x0D\x0A"
							"# WEBブラウザ用のURL\x0D\x0A"//web browser URL (?)
							"# JP:\x0D\x0A"
							"\x0D\x0A"
							"\x0D\x0A"
							"# EN:\x0D\x0A"
							"\x0D\x0A"
							"\x0D\x0A"
							"# FR:\x0D\x0A"
							"\x0D\x0A"
							"\x0D\x0A"
							"# GE:\x0D\x0A"
							"\x0D\x0A"
							"\x0D\x0A"
							"# IT:\x0D\x0A"
							"\x0D\x0A"
							"\x0D\x0A"
							"# SP:\x0D\x0A"
							"\x0D\x0A"
							"\x0D\x0A"
							"# CN:\x0D\x0A"
							"\x0D\x0A"
							"\x0D\x0A"
							"# KO:\x0D\x0A"
							"\x0D\x0A"
							"\x0D\x0A"
							"# DU:\x0D\x0A"
							"\x0D\x0A"
							"\x0D\x0A"
							"# PO:\x0D\x0A"
							"\x0D\x0A"
							"\x0D\x0A"
							"# RU:\x0D\x0A"
							"\x0D\x0A"
							"\x0D\x0A"
							"# TW:");
		if(std::filesystem::exists("romfs/settings/settingsTL.csv")) completed[1] = 'X';
		else puts("ERROR: Failed to generate romfs/settings/settingsTL.csv");
	}
	else {
		if(std::filesystem::exists("romfs/settings/settingsTL.csv")) scompleted[1] = 'X';
		else puts("ERROR: Failed to generate romfs/settings/settingsTL.csv");
	}
	settingsTL.close();
	pause
}

void copyright() {
	windowTitle("[MultiVidInjector5000] Copyright options");
	cls
	name = "";
	bool utf16 = false;
	completed[2] = ' ';
	std::filesystem::create_directories("romfs/settings");
	while(name == "") {
		std::ofstream information_buttons("romfs/settings/information_buttons.csv", std::ios_base::out | std::ios_base::binary);
		puts("Do you want the menu to have the Copyright button? [Y/N]");
		std::getline(std::cin, name);
		if(name == "") cls
		else if(tolower(name[0]) == 'y') {
			information_buttons << "\xFF\xFE" << UTF8toUTF16("Copyright");
			information_buttons.close();
		}
		else {
			information_buttons << "\xFF\xFE";
			information_buttons.close();
			completed[2] = 'X';
			return;
		}
	}
	cls
	name = "";
	while(name == "") {
		puts("Enter the text for the copyright menu:");
		std::getline(std::cin, name);
		if(name == "") cls
	}
	if(tolowerstr((std::string)&name[name.size()-4]) == ".txt" || tolowerstr((std::string)&name[name.size()-5]) == ".txt\"") {
		if(readTxt(name, name)) utf16 = true;
	}
	std::ofstream copyrighttxt("romfs/settings/copyright.txt", std::ios_base::out | std::ios_base::binary);
	copyrighttxt << "\xFF\xFE";
	if(utf16) copyrighttxt << name;
	else copyrighttxt << UTF8toUTF16(name);
	copyrighttxt.close();
	if(!std::filesystem::exists("romfs/settings/copyright.txt")) {//how tho like what, your hard drive is 100% full???
		puts("ERROR: Failed to generate romfs/settings/copyright.txt");
		pause
		return;
	}
	completed[2] = 'X';
	pause
}

void tobimg() {
	windowTitle("[MultiVidInjector5000] Image to .bimg");
	cls
	completed[3] = ' ';
	if (amount == 0) {
		puts("Set video amount first!");
		pause
		return;
	}
	std::filesystem::create_directories("romfs/movie");
	for (unsigned long i = 0; i < amount; i++) {
		name = "";
		while(name == "") {
			cls
			printf("Enter/drag and drop image for video #%li\n(The image should be 200x120 for best results)\n", i+1);
			std::getline(std::cin, name);
			removeQuotes(name);
			if(!std::filesystem::exists(name)) {
				printf("Error with the file (%s) Try again.\n", name.c_str());
				name = "";
				if(!goorQuit()) return;
			}
		}
		
		unsigned char bimg[65568];
		if(!convertToBimg(name, bimg, true)) {
			printf("ERROR: Failed to generate romfs/movie/movie_%li.bimg, try again.\n", i);
			i--;
			if(!goorQuit()) return;
		}
		std::ofstream bimgfile("romfs/movie/movie_" + std::to_string(i) + ".bimg", std::ios_base::out | std::ios_base::binary);
		bimgfile.write(reinterpret_cast<const char*>(bimg), sizeof(bimg));
		bimgfile.close();
	}
	completed[3] = 'X';
	pause
}

void moflexMover() {
	type = MultiVid ? "MultiVidInjector5000" : "VidInjector9001";
	windowTitle("[" + type + "] Injecting .moflex");
	cls
	static bool pass = false;
	completed[4] = ' ';
	scompleted[2] = ' ';
	unsigned char Checker[4];
	if (amount == 0) {
		puts("Set video amount first!");
		pause
		return;
	}
	std::filesystem::create_directories("romfs/movie");
	for (unsigned long i = 0; i < amount; i++) {
		while(!pass) {
			cls

			if(MultiVid) printf("Enter/drag and drop moflex file #%li\n", i+1);
			else puts("Enter/drag and drop the moflex file");
			std::getline(std::cin, name);
			removeQuotes(name);
			
			std::string extension = name;
			extension.erase (extension.begin(), extension.end()-7);
			std::ifstream inmoflex (name, std::ios::binary);
			for (int i = 0; i < 4; i++) {
				inmoflex >> Checker[i];//https://stackoverflow.com/a/2974735
				if(extension != ".moflex" || Checker[i] != moflexMagic[i]) {
					printf("The input file (%s) is broken or not in moflex format. Try again.\n", name.c_str());
					name = "";
					if(!goorQuit()) return;
					break;
				} else pass = true;
			}
		}
		if(MultiVid) {
			copyfile(name, "romfs/movie/movie_" + std::to_string(i) + ".moflex");
			if(!std::filesystem::exists("romfs/movie/movie_" + std::to_string(i) + ".moflex")) {//this probably only happens if there's no disk space
				printf("ERROR: Failed to copy \"%s\" to romfs/movie/movie_%li.moflex\n", name.c_str(), i);
				name = "";
				if(!goorQuit()) return;
			}
		}
		else {
			copyfile(name, "romfs/movie/movie.moflex");
			if(!std::filesystem::exists("romfs/movie/movie.moflex")) {//this probably only happens if there's no disk space
				printf("ERROR: Failed to copy \"%s\" to romfs/movie/movie.moflex\n", name.c_str());
				name = "";
				if(!goorQuit()) return;
			}
		}
		pass = false;
	}
	completed[4] = 'X';
	scompleted[2] = 'X';
	pause
}

void makebanner() {
	type = MultiVid ? "MultiVidInjector5000" : "VidInjector9001";
	windowTitle("[" + type + "] Generate banner");
	cls
	std::filesystem::create_directory("exefs");
	name = "";
	completed[5] = ' ';
	scompleted[3] = ' ';
	while(name == "") {
		cls
		puts("Enter/drag and drop your home screen banner image:\n(The image should be 200x120 for best results)");
		std::getline(std::cin, name);
		removeQuotes(name);
		if(!std::filesystem::exists(name)) {
			printf("Error with the file (%s) Try again.\n", name.c_str());
			name = "";
			if(!goorQuit()) return;
		}
	}

	unsigned char buffer[65536];
	if(!convertToBimg(name, buffer, false)) {
		puts("ERROR: Failed to convert image.");
		pause
		return;
	}
	
	//create bcmdl
	unsigned char* bcmdl;
	bcmdl = (unsigned char*) malloc(sizeof(bannerheader) + sizeof(buffer) + sizeof(bannerfooter));
	memcpy(bcmdl, bannerheader, sizeof(bannerheader));
	memcpy(bcmdl + sizeof(bannerheader), buffer, sizeof(buffer));
	memcpy(bcmdl + sizeof(bannerheader) + sizeof(buffer), bannerfooter, sizeof(bannerfooter));
	
	//build banner (stolen from bannertool)
    CBMD cbmd;
    memset(&cbmd, 0, sizeof(cbmd));
	
	cbmd.cgfxSizes[0] = sizeof(bannerheader) + sizeof(buffer) + sizeof(bannerfooter);
	cbmd.cgfxs[0] = bcmdl;
	
	cbmd.cwavSize = sizeof(BCWAV_array);
	cbmd.cwav = (void*)BCWAV_array;

	uint32_t bnrSize = 0;
	void* bnr = cbmd_build_data(&bnrSize, cbmd);

	std::ofstream bnrfile("exefs/banner.bin", std::ios_base::out | std::ios_base::binary);
	bnrfile.write(reinterpret_cast<const char*>(bnr), bnrSize);
	bnrfile.close();

	puts("Created banner \"exefs/banner.bin\".");

	//system_g(_toolsPath + _bannertoolPath + " makebanner -ci exefs/banner0.bcmdl -ca Vidinjector9000Resources/files/banner.bcwav -o exefs/banner.bin");
	//clean up time
	remove("exefs/banner0.bcmdl");
	free(bcmdl);
	if(!std::filesystem::exists("exefs/banner.bin")) {
		puts("ERROR: Failed to generate exefs/banner.bin");
		pause
		return;
	}
	completed[5] = 'X';
	completed[7] = ' ';
	scompleted[3] = 'X';//i realized this has to be like this for the checks
	scompleted[4] = ' ';
	pause
}

void makeIcon() {
	type = MultiVid ? "MultiVidInjector5000" : "VidInjector9001";
	windowTitle("[" + type + "] Generate icon");
	cls
	std::string shortname;
	std::string publisher;
	bool utf16[3] = { false, false, false };//shortname, longname, publisher
	remove("exefs/Icon.bin");
	name = "";
	completed[6] = ' ';
	scompleted[4] = ' ';
	while (name == "") {
		cls
		puts("Enter/drag and drop your icon image:\n(The image should be 48x48 for best results)");
		std::getline(std::cin, name);
		removeQuotes(name);
		if(!std::filesystem::exists(name)) {
			printf("Error with the file (%s) Try again.\n", name.c_str());
			name = "";
			if(!goorQuit()) return;
		}
	}
	cls
	shortname = "";
	while (shortname == "") {//stupid-proofing
		puts("Enter the short name:");
		std::getline(std::cin, shortname);
		std::string temptxt = UTF8toUTF16(shortname);
		if(temptxt.size() > 0x80) {
			puts("ERROR: The short name must be no longer than 128 characters. Try again.");
			shortname = "";
			pause
		}
		if(shortname == "") cls
	}
	if(tolowerstr((std::string)&shortname[shortname.size()-4]) == ".txt" || tolowerstr((std::string)&shortname[shortname.size()-5]) == ".txt\"") {
		if(readTxt(shortname, shortname)) utf16[0] = true;
	}
	longname = "";
	while (longname == "") {
		puts("Enter the long name:");
		std::getline(std::cin, longname);
		std::string temptxt = UTF8toUTF16(longname);
		if(temptxt.size() > 0x100) {
			puts("ERROR: The long name must be no longer than 256 characters. Try again.");
			longname = "";
			pause
		}
		if(longname == "") cls
	}
	if(tolowerstr((std::string)&longname[longname.size()-4]) == ".txt" || tolowerstr((std::string)&longname[longname.size()-5]) == ".txt\"") {
		if(readTxt(longname, longname)) utf16[1] = true;
	}
	publisher = "";
	while (publisher == "") {
		puts("Enter the publisher:");
		std::getline(std::cin, publisher);
		std::string temptxt = UTF8toUTF16(publisher);
		if(temptxt.size() > 0x80) {
			puts("ERROR: The publisher name must be no longer than 128 characters. Try again.");
			publisher = "";
			pause
		}
		if(publisher == "") cls

	}
	if(tolowerstr((std::string)&publisher[publisher.size()-4]) == ".txt" || tolowerstr((std::string)&publisher[publisher.size()-5]) == ".txt\"") {
		if(readTxt(publisher, publisher)) utf16[1] = true;
	}

	convertToIcon(name, "exefs/icon.bin", utf16[0] ? shortname : UTF8toUTF16(shortname), utf16[1] ? longname : UTF8toUTF16(longname), utf16[2] ? publisher : UTF8toUTF16(publisher));

	if(MultiVid) {
		copyfile("exefs/icon.bin", "romfs/icon.icn");
		if(!std::filesystem::exists("romfs/icon.icn")) {
			puts("ERROR: Failed to copy exefs/icon.bin to romfs/icon.icn");
			pause
			return;
		}
	}
	if(!std::filesystem::exists("exefs/icon.bin")) {//more stupid-proofing
		puts("ERROR: Failed to generate exefs/icon.bin");
		pause
		return;
	}
	completed[6] = 'X';
	scompleted[4] = 'X';
	pause
}

void customBanner() {
	type = MultiVid ? "MultiVidInjector5000" : "VidInjector9001";
	windowTitle("[" + type + "] Generate banner");

	static bool pass = false;
	unsigned char Checker[4];

	while(!pass) {
		cls

		puts("Enter/drag and drop your banner:\n(pre-built file)");
		std::getline(std::cin, name);
		removeQuotes(name);
		std::ifstream inbanner (name, std::ios::binary);
		for (int i = 0; i < 4; i++) {
			inbanner >> Checker[i];//https://stackoverflow.com/a/2974735
			if(Checker[i] != bannerMagic[i]) {
				printf("The input file (%s) is not a valid banner. Try again.\n", name.c_str());
				name = "";
				if(!goorQuit()) return;
				break;
			} else pass = true;
		}
	}
	copyfile(name, "exefs/banner.bin");
	if(!std::filesystem::exists("exefs/banner.bin")) {
		printf("ERROR: Failed to copy \"%s\" to exefs/banner.bin.\n", name.c_str());
		pause
		return;
	}
	completed[5] = '-';
	completed[7] = 'X';
	scompleted[3] = '-';
	scompleted[5] = 'X';
	pause
}

void makeCIA() {
	type = MultiVid ? "MultiVidInjector5000" : "VidInjector9001";
	windowTitle("[" + type + "] Generate CIA");
	cls
	
	unsigned long min = 0xC0000;
	unsigned long max = 0xF0000;
	unsigned long TID = max;
	static std::mt19937 rng;

	for (unsigned int i = 0; i < sizeof(MultiVid ? completed : scompleted)-3; i++)
		if((MultiVid ? completed : scompleted)[i] == ' ') {
			printf("Job #%i has not been done. Do you really want to continue? [Y/N]\n", i+1);
			std::getline(std::cin, name);
			if(tolower(name[0]) == 'y') break;
			return;
		}
	
	while(TID == max) {
		cls
		puts("Enter 5 hex integers for the ID of your cia (C0000 - EFFFF) or\njust type \"0\" for a random unique ID.\n(TID is in format 000400000XXXXX00 (that's hex), the rest will auto fill)");
		std::getline(std::cin, name);
		if(name.size() > 5) name = "F0000";//more stupid-proofing
		if(!stoul_s(TID, name, true)) {
			puts("Invalid input, try again");
			if(!goorQuit()) return;
			TID = max;
		}
		if (TID == 0) {
			rng.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
			std::uniform_int_distribution<unsigned long> uniform(min, max);
			TID = uniform(rng);
		}
		switch(TID)
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
				printf("Oops, you ran into a blacklisted ID! (%05lX) Try again.\n", TID);
				TID = max;
				if(!goorQuit()) return;
			}
			break;
			default:
				if(TID < max && TID > min+1) printf("%05lX Passed all checks!\n", TID);
				else {
					printf("Oops, you ran into a blacklisted ID! (%05lX) Try again.\n", TID);
					TID = max;
					if(!goorQuit()) return;
				}
		}
	}
	puts("Generating CIA...");
	removeQuotes(longname);
	removeInvalids(longname);
	std::filesystem::create_directory("output");
	char buffer[6];
	sprintf(buffer, "%05lX", TID);
	std::string cmd = system_g(_toolsPath + _makeromPath + " -f cia -o \"output/" + longname + " [000400000" + std::string(buffer) + "00].cia\" -banner \"exefs/banner.bin\" -icon \"exefs/icon.bin\" -code \"exefs/code.bin\" -exheader \"exheader.bin\" -rsf \"Vidinjector9000Resources/files/template.rsf\" -DAPP_UNIQUE_ID=" + std::to_string(TID));
	if(Debug) {printf("[cmd] %s\n", cmd.c_str()); pause}
	if(!std::filesystem::exists("output/" + longname + " [000400000" + std::string(buffer) + "00].cia")) {
		printf("ERROR: Failed to build: \"output\\%s [000400000%s00].cia\"\n", longname.c_str(), std::string(buffer).c_str());
	}
	size_t cia_size = 0;
	if(std::filesystem::exists("output/" + longname + " [000400000" + std::string(buffer) + "00].cia"))
		cia_size = std::filesystem::file_size("output/" + longname + " [000400000" + std::string(buffer) + "00].cia");
	if(cia_size > 4294967296) {
		printf("WARNING: The file size (%zu) of the cia file is too big and will\nnot install to a 3ds nor work in the emulator.\n", cia_size);
	}
	pause
	cls
	puts("Do you want this to delete the following folders/files? [Y/N]\n- exefs\n- romfs\n- exheader.bin");
	std::getline(std::cin, name);
	if(tolower(name[0]) == 'y') {
		main();
	}
	pause
}

void Settings() {
	while(1) {
		cls
		printf("Type a letter:\n\n"
		" __________________________________________\n"
		"|                                          |\n"
		"| Software Version: 2.6.9                  |\n"
		"|                                          |\n"
		"| D: Toggle Debug Information       [");
							if(Debug) printf("ON]   |\n");
							else      printf("OFF]  |\n");
 printf("|                                          |\n"
		"| X: Go to the main menu                   |\n"
		"|__________________________________________|\n\n");
		std::getline(std::cin, name);
		if(tolower(name[0]) == 'd') Debug = Debug ? false : true;//if true, make it false and if false, make it true
		else if(tolower(name[0]) == 'x') return;
	}
}

//2nd menu
void finalize() {
	type = MultiVid ? "MultiVidInjector5000" : "VidInjector9001";
	windowTitle("[" + type + "] Finalizing");
	cls
	while(1) {
		cls
		type = MultiVid ? "multi video menu            |" : "single video menu           |";
		printf("Type a letter:\n\n"
		" __________________________________________\n"
		"|                                          |\n"
		"| B: Generate banner                   [%c] |\n"
		"| I: Generate icon                     [%c] |\n"
		"| U: Use a custom banner file (CBMD)   [%c] |\n"//do U because i cant do C or B or idk it just means "use"
		"|                                          |\n"
		"| C: Generate CIA                          |\n"
		"| X: Go to the %s\n"
		"|__________________________________________|\n\n", completed[5], completed[6], completed[7], type.c_str());
		std::getline(std::cin, name);
		if(tolower(name[0]) == 'b') makebanner();//5
		else if(tolower(name[0]) == 'i') makeIcon();//6
		else if(tolower(name[0]) == 'u') customBanner();//7
		else if(tolower(name[0]) == 'c') makeCIA();
		else if(tolower(name[0]) == 'x') return;
	}
}

//multi video menu (duh)
void MultiVideo() {
	cls
	MultiVid = true;
	amount = 0;
	std::string amountstr;
	Generate_Files(true);
	while(1) {
		if (amount == 0)	amountstr = "                  [ ";
		else if(amount <= 9)amountstr = "                  [" + std::to_string(amount);//this will make sense in the end trust me
		else				amountstr = "                 [" + std::to_string(amount);//see? no?
		windowTitle("MultiVidInjector5000 by Foofoo_the_guy");
		cls
		printf("Type a letter:\n\n"
		" __________________________________________\n"
		"|                                          |\n"
		"| A: Set video amount%s] |\n"//just look
		"| T: movie_title.csv generator         [%c] |\n"
		"| S: settingsTL.csv generator          [%c] |\n"
		"| C: Copyright options                 [%c] |\n"
		"| B: Image to banner image converter   [%c] |\n"
		"| M: Select/Copy over the moflex files [%c] |\n"
		"|                                          |\n"
		"| F: Finalize/build the CIA                |\n"
		"| X: Go to the main menu                   |\n"
		"|__________________________________________|\n\n", amountstr.c_str(), completed[0], completed[1], completed[2], completed[3], completed[4]);
		std::getline(std::cin, name);
		if(tolower(name[0]) == 'a' && amount == 0) setAmount();
		else if(tolower(name[0]) == 't') Movie_title();//0
		else if(tolower(name[0]) == 's') makesettingsTL();//1
		else if(tolower(name[0]) == 'c') copyright();//2
		else if(tolower(name[0]) == 'b') tobimg();//3
		else if(tolower(name[0]) == 'm') moflexMover();//4
		else if(tolower(name[0]) == 'f') finalize();
		else if(tolower(name[0]) == 'x') {
			cls
			name = "";
			while(name == "") {
				puts("Exit now? You will lose progress. [Y/N]");
				std::getline(std::cin, name);
				if(tolower(name[0]) == 'y') return;
				else if(tolower(name[0]) == 'n') break;
				else name = "";
				cls
			}
		}
	}
}

//single video menu (duh)
void SingleVideo() {
	cls
	MultiVid = false;
	amount = 1;//huhuhu ez way to reuse the functions (i made multi video first)
	Generate_Files(false);
	while(1) {
		windowTitle("VidInjector9001 by Foofoo_the_guy");
		cls
		printf("Type a letter:\n\n"
		" __________________________________________\n"
		"|                                          |\n"
		"| T: movie_title.csv generator         [%c] |\n"
		"| S: settingsTL.csv generator          [%c] |\n"
		"| M: Select/Copy over the moflex file  [%c] |\n"
		"|                                          |\n"
		"| F: Finalize/build the CIA                |\n"
		"| X: Go to the main menu                   |\n"
		"|__________________________________________|\n\n", scompleted[0], scompleted[1], scompleted[2]);
		std::getline(std::cin, name);
		if(tolower(name[0]) == 't') Movie_title();//0
		else if(tolower(name[0]) == 's') makesettingsTL();//1
		else if(tolower(name[0]) == 'm') moflexMover();//2
		else if(tolower(name[0]) == 'f') finalize();
		else if(tolower(name[0]) == 'x') {
			cls
			name = "";
			while(name == "") {
				puts("Exit now? You will lose progress. [Y/N]");
				std::getline(std::cin, name);
				if(tolower(name[0]) == 'y') return;//massive brain solution: type anything as long as it starts with y
				else if(tolower(name[0]) == 'n') break;
				else name = "";
				cls
			}
		}
	}
}

//main menu
int main() {
	while(1) {
		windowTitle("VidInjector9001 by Foofoo_the_guy");
		cls
		//wipe the stuff
		for (auto& c : completed)
			c = ' ';
		for (auto& c : scompleted)
			c = ' ';
		std::filesystem::remove_all("exefs");
		std::filesystem::remove_all("romfs");
		remove("exheader.bin");
		
		cls
		printf("Type a letter:\n\n"
		" __________________________________________\n"//woah ah it looks so cool how did you do that? weee
		"|                                          |\n"
		"| S: Singular Video Injector               |\n"
		"| M: Multiple Video Injector               |\n"
		"| P: Program Settings                      |\n"
		"|__________________________________________|\n\n");
		std::getline(std::cin, name);
		if(tolower(name[0]) == 's') SingleVideo();//0
		else if(tolower(name[0]) == 'm') MultiVideo();//1
		else if(tolower(name[0]) == 'p') Settings();//2
	}
	return 0;
}