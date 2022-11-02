#include "VidInjector9001.h"

//globals
#define pause betterPause();
#define cls Clear();

unsigned long amount = 0;
std::string name = "";
std::string type;
std::string longname;//you need this global because i said so also because make cia needs it too
static bool MultiVid;//false if single video, true if multi video
static bool Debug;//false if off true if on
char completed[8] { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', };
char scompleted[6] { ' ', ' ', ' ', ' ', ' ', ' ', };

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

void GetDirSize(std::filesystem::path dir, size_t &size) {
	for (const auto &entry : std::filesystem::directory_iterator(dir)) {
		//std::cout << entry.path() << std::endl;
		std::filesystem::path p = entry;

		try {
			size += std::filesystem::file_size(p);
		} catch(std::filesystem::filesystem_error& e) {
			GetDirSize(entry.path(), size);
		}
	}
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

//this UTF stuff is from libctru lol
#ifdef SIZE_MAX
#define SSIZE_MAX ((SIZE_MAX) >> 1)
#endif

//https://github.com/devkitPro/libctru/raw/4e25fb1d6c2ea124a9011c4b65f76f2968a9fb97/libctru/source/util/utf/encode_utf16.c
ssize_t
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
ssize_t
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
ssize_t
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

      if(SSIZE_MAX - units >= rc)
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
	utf8_to_utf16(utf16, utf8, input.size());
	char utf16str[strlen(input) * 2];
	memcpy(utf16str, utf16, strlen(input) * 2);

	for (size_t i = 0; i < strlen(input) * 2; i++)
		output += utf16str[i];
	
	delete[] utf8;
	delete[] utf16;
	return output;
}

//based on https://raw.githubusercontent.com/nothings/stb/master/tests/resample_test.cpp
class stbir_context {
public:
	stbir_context()
	{
		size = 1000000;
		memory = malloc(size);
	}

	~stbir_context()
	{
		free(memory);
	}

	size_t size;
	void* memory;
} g_context;

void* stbir_malloc(size_t size, void* context)
{
	if (!context)
		return malloc(size);

	stbir_context* real_context = (stbir_context*)context;
	if (size > real_context->size)
		return 0;

	return real_context->memory;
}

void stbir_free(void* memory, void* context)
{
	if (!context)
		free(memory);
}

static void image_data_to_tiles(void* out, void* img, uint32_t width, uint32_t height) {//from bannertool, edited for rgb565 only
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

bool convertToBimg(std::string input, std::string outputpath, bool writeHeader)// true for write header, false for dont write header
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
	printf("w %d, h %d, out_w %d, out_h %d\n", w, h, out_w, out_h);
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
		for(int i = 3; i < out_w*out_h*4; i+=4) {
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
	image_data_to_tiles(tiledbanner, output_fin, 256, 128);
	remove(outputpath.c_str());
	std::ofstream bimg(outputpath.c_str(), std::ios_base::app | std::ios_base::binary);
	if(writeHeader) bimg.write(reinterpret_cast<const char*>(bimgheader), sizeof(bimgheader));
	bimg.write(reinterpret_cast<const char*>(tiledbanner), sizeof(tiledbanner));
	bimg.close();

	stbi_write_png("imag.png", new_w, new_h, 3, output_fin, 0);
	free(output_3c);
	return true;
}

bool convertToIcon(std::string input, std::string output) {
	unsigned char* input_pixels;
	unsigned char* output_pixels;
	unsigned char* output_3c;
	int w, h, ch, comp;
	const int out_w = 48;
	const int out_h = 48;//lol these are the same
	const uint8_t FF = 0xFF;
	if(!stbi_info(input.c_str(), &w, &h, &comp)) {
		puts("ERROR: Failed to get image info.");
		return false;
	}
	input_pixels = stbi_load(input.c_str(), &w, &h, &ch, 0);
	output_pixels = (unsigned char*) malloc(out_w*out_h*ch);
	if(w == out_w && h == out_h) memcpy(output_pixels, input_pixels, w*h*ch);
	else stbir_resize_uint8(input_pixels, w, h, 0, output_pixels, out_w, out_h, 0, ch);//scale to 48x48 if needed

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
		for(int i = 3; i < out_w*out_h*4; i+=4) {
			output_3c[newi-3] = output_pixels[i-3];
			output_3c[newi-2] = output_pixels[i-2];
			output_3c[newi-1] = output_pixels[i-1];
			newi+=3;
		}
		free(output_pixels);
	}
	if(ch == 3) {
		output_3c = (unsigned char*) malloc(out_w*out_h*3);
		for(int i = 0; i < out_w*out_h*3; i++) output_3c[i] = output_pixels[i];
		free(output_pixels);
	}
	stbi_write_png(output.c_str(), out_w, out_w, 3, output_3c, 0);
	stbi_image_free(input_pixels);
	free(output_3c);
	return true;
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

void betterPause() {
	puts("Press Enter to continue . . .");
	std::cin.ignore();
	std::cin.sync();//clears it if you typed any characters
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

bool Generate_Code(bool Multi) {
    std::string path = "exefs/code.bin";
    puts("Generating code.bin...");
    std::filesystem::create_directory("exefs");
	std::ofstream codebin(path, std::ios_base::out | std::ios_base::binary);
	if(std::filesystem::exists(path)) {
		codebin.write(reinterpret_cast<const char*>(Multi ? Multivid : Singlevid), Multi ? sizeof(Multivid) : sizeof(Singlevid));
		return true;
	}
    return false;
}

bool readTxt(std::string file, std::string &output) {//return true if it's unicode or else fale, who knows what that could be though
	std::string realoutput = "";
	while(1) {
		puts("Use the contents of this .txt file for the input? (Y/N)");
		std::getline(std::cin, name);
		if(tolower(name[0]) == 'y') {
			removeQuotes(file);
			if(std::filesystem::exists(file)) {
				name = "";
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
				if((output[0] & 0xFF) == 0xFF && (output[1] & 0xFF) == 0xFE) {//if it's a unicode file
					realoutput = (std::string)&output[2];
					output = realoutput;
					return true;
				}
				return false;
			}
			else {
				puts("ERROR: file does not exist\nReturning input as output...");
				return false; 
			}
		}
		else if(tolower(name[0]) == 'n') {
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
	if (amount == 0) {
		puts("Set video amount first!");
		pause
		return;
	}
	std::filesystem::create_directories("romfs/movie");
	std::ofstream movie_title("romfs/movie/movie_title.csv", std::ios_base::out | std::ios_base::binary);
	bool utf16 = false;
	movie_title << "\xFF\xFE" + UTF8toUTF16("#JP,#EN,#FR,#GE,#IT,#SP,#CH,#KO,#DU,#PO,#RU,#TW\x0D\x0A");
	for (unsigned long i = 0; i < amount; i++) {
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
	if(MultiVid) completed[0] = 'X';
	else scompleted[0] = 'X';
	pause
}

void makesettingsTL() {
	type = MultiVid ? "MultiVidInjector5000" : "VidInjector9001";
	windowTitle("[" + type + "] Generate settingsTL.csv");
	cls

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
		else buttons = "";
		cls
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

		if(!convertToBimg(name, "romfs/movie/movie_" + std::to_string(i) + ".bimg", true)) {
			printf("ERROR: Failed to generate romfs/movie/movie_%li.bimg, try again.\n", i);
			i--;
			if(!goorQuit()) return;
		}
	}
	completed[3] = 'X';
	pause
}

void moflexMover() {
	type = MultiVid ? "MultiVidInjector5000" : "VidInjector9001";
	windowTitle("[" + type + "] Injecting .moflex");
	cls
	static bool pass = false;
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
	if(MultiVid) completed[4] = 'X';
	else scompleted[2] = 'X';
	pause
}

void makebanner() {
	type = MultiVid ? "MultiVidInjector5000" : "VidInjector9001";
	windowTitle("[" + type + "] Generate banner");
	cls
	std::filesystem::create_directory("exefs");
	name = "";
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

	if(!convertToBimg(name, "exefs/banner.bimg.part", false) || !std::filesystem::exists("exefs/banner.bimg.part")) {
		puts("ERROR: Failed to convert image.");
		pause
		return;
	}
	
	std::ifstream bimgfile("exefs/banner.bimg.part", std::ios::binary);
	char buffer[65536];
	char Byte;
	int it = 0;
	bimgfile.read(&Byte, 1);
	while(bimgfile) {
		buffer[it] = Byte;
		bimgfile.read(&Byte, 1);
		it++;
	}
	bimgfile.close();
	bimgfile.read(buffer, sizeof(buffer));
	bimgfile.close();
	
	//create bcmdl
	remove("exefs/banner0.bcmdl");
	std::ofstream bannerbcmdl("exefs/banner0.bcmdl", std::ios_base::app | std::ios_base::binary);
	bannerbcmdl.write(reinterpret_cast<const char*>(bannerheader), sizeof(bannerheader));
	bannerbcmdl.write(reinterpret_cast<const char*>(buffer), sizeof(buffer));
	bannerbcmdl.write(reinterpret_cast<const char*>(bannerfooter), sizeof(bannerfooter));
	bimgfile.close();
	bannerbcmdl.close();
	remove("exefs/banner.bimg.part");
	//build banner
	system_g(_toolsPath + _bannertoolPath + " makebanner -ci exefs/banner0.bcmdl -ca Vidinjector9000Resources/files/banner.bcwav -o exefs/banner.bin");
	//clean up time
	remove("exefs/banner0.bcmdl");

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

void makeIcon() {//doesnt support utf-16 name
	type = MultiVid ? "MultiVidInjector5000" : "VidInjector9001";
	windowTitle("[" + type + "] Generate icon");
	cls
	std::string shortname;
	std::string publisher;
	remove("exefs/Icon.bin");
	name = "";
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
		if(shortname == "") cls
	}
	longname = "";
	while (longname == "") {
		puts("Enter the long name:");
		std::getline(std::cin, longname);
		if(longname == "") cls
	}
	publisher = "";
	while (publisher == "") {
		puts("Enter the publisher:");
		std::getline(std::cin, publisher);
		if(publisher == "") cls
	}

	copyfile(name, "exefs/temp.png");
	convertToIcon("exefs/temp.png", "exefs/Icon.png");
	remove("exefs/temp.png");
	std::string cmd = system_g(_toolsPath + _bannertoolPath + " makesmdh -i \"exefs/Icon.png\" -s \"" + shortname + "\" -l \"" + longname + "\" -p \"" + publisher + "\" -f visible,nosavebackups -o \"exefs/icon.bin\"");
	if(Debug) {printf("[cmd] %s\n", cmd.c_str()); pause}
	remove("exefs/Icon.png");

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
		"| Software Version: 2.6.2                  |\n"
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
	/*for (unsigned int i = 0; i < (MultiVid ? sizeof(completed)-3 : sizeof(scompleted)-3); i++)
		if((MultiVid ? completed : scompleted)[i] == ' ') {
			printf("Job #%i has not been done. Do you really want to continue? [Y/N]\n", i+1);
			std::getline(std::cin, name);
			if(tolower(name[0]) == 'y') break;
			return;
		}*/
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
	if(!Generate_Code(true)) {
		puts("Failed to generate files.");
		pause
		return;
	}
	copyfile("Vidinjector9000Resources/files/templates/MultiVideo/romfs", "romfs");
	copyfile("Vidinjector9000Resources/files/templates/MultiVideo/exheader.bin", "exheader.bin");
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
	if(!Generate_Code(false)) {
		puts("Failed to generate files.");
		pause
		return;
	}
	copyfile("Vidinjector9000Resources/files/templates/SingleVideo/romfs", "romfs");
	copyfile("Vidinjector9000Resources/files/templates/SingleVideo/exheader.bin", "exheader.bin");
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