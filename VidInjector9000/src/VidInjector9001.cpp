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

bool pathExists(const std::filesystem::path& p)//https://en.cppreference.com/w/cpp/filesystem/exists
{
	if(std::filesystem::exists(p)) return true;
    return false;
}

void copyfile(std::string inpath, std::string outpath) {//also works with directories
	if(pathExists(outpath))
		std::filesystem::remove_all(outpath);
	if(pathExists(inpath))
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

bool convertToBanner(std::string input, std::string outputpath)
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
	stbir_resize_uint8(input_pixels, w, h, 0, output_pixels, out_w, out_h, 0, ch);
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
	stbi__vertical_flip(output_fin, new_w, new_h, 3);//because 3dstex is broken
	stbi_write_png(outputpath.c_str(), new_w, new_h, 3, output_fin, 0);
	//stbi_write_png("imag.png", new_w, new_h, 3, output_fin, 0);
	free(output_3c);
	return true;
}

bool convertToIcon(std::string input, std::string output) {
	unsigned char* input_pixels;
	unsigned char* output_pixels;
	unsigned char* output_3c;
	int w, h, ch, comp;
	const int out_w = 48;
	const int out_h = 48;
	const uint8_t FF = 0xFF;
	if(!stbi_info(input.c_str(), &w, &h, &comp)) {
		puts("ERROR: Failed to get image info.");
		return false;
	}
	input_pixels = stbi_load(input.c_str(), &w, &h, &ch, 0);
	output_pixels = (unsigned char*) malloc(out_w*out_h*ch);
	stbir_resize_uint8(input_pixels, w, h, 0, output_pixels, out_w, out_h, 0, ch);

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
}

bool goorQuit() {//false for quit, true for go
	std::string opt = "";
	puts("Press Enter to continue or Q+Enter to quit job . . .");
	std::getline(std::cin, opt);
	return tolower(opt[0])-0x71;
}

void Clear() {//https://stackoverflow.com/a/52895729
#if defined(_WIN32)
    system("cls");
#else
    system("clear");
#endif
}

bool Generate_Code(bool Multi) {
    std::string path = "exefs/code.bin";
    puts("Generating code.bin...");
    std::filesystem::create_directory("exefs");
	std::ofstream codebin(path, std::ios_base::out | std::ios_base::binary);
	if(pathExists(path)) {
		codebin.write(reinterpret_cast<const char*>(Multi ? Multivid : Singlevid), Multi ? sizeof(Multivid) : sizeof(Singlevid));
		return true;
	}
    return false;
}

//big functions
void setAmount() {
	#if defined(_WIN32)
		system("title [MultiVidInjector5000] Set video amount");
	#endif
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
	if(!pathExists("romfs/settings/movie_bnrname.csv")) {
		puts("ERROR: Failed to generate romfs/settings/movie_bnrname.csv");
		pause
		return;
	}
}

void Movie_title() {
	#if defined(_WIN32)
		type = MultiVid ? "MultiVidInjector5000" : "VidInjector9001";
		system_g("title [" + type + "] Generate movie_title.csv");
	#endif
	cls
	if (amount == 0) {
		puts("Set video amount first!");
		pause
		return;
	}
	std::filesystem::create_directories("romfs/movie");
	std::ofstream movie_title("romfs/movie/movie_title.csv", std::ios_base::out | std::ios_base::binary);
	movie_title << "\xFF\xFE" + UTF8toUTF16("#JP,#EN,#FR,#GE,#IT,#SP,#CH,#KO,#DU,#PO,#RU,#TW\x0D\x0A");
	for (unsigned long i = 0; i < amount; i++) {
		name = "";
		while(name == "") {
			if(MultiVid) printf("Enter the title of video #%li\n", i+1);
			else puts("Enter the title of the video");
			std::getline(std::cin, name);
			if(name == "") cls
		}
		for (int j = 0; j < 11; j++) {//do it 11 times because it needs to
			movie_title << UTF8toUTF16(name + ",");
		}
		movie_title << UTF8toUTF16(name + "\x0D\x0A");//put the last stuff
	}
	movie_title.close();
	if(!pathExists("romfs/movie/movie_title.csv")) {
		puts("ERROR: Failed to generate romfs/movie/movie_title.csv");
		pause
		return;
	}
	if(MultiVid) completed[0] = 'X';
	else scompleted[0] = 'X';
	pause
}

void makesettingsTL() {
	#if defined(_WIN32)
	type = MultiVid ? "MultiVidInjector5000" : "VidInjector9001";
	system_g("title [" + type + "] Generate settingsTL.csv");
	#endif
	cls

	if (amount == 0) {
		puts("Set video amount first!");
		pause
		return;
	}

	std::string publisher = "";
	std::string buttons = "";
	std::string gentleness = "";
	
	name = "";
	while(name == "") {
		if(MultiVid) puts("Enter the name of the series");
		else puts("Enter the name of the video");
		std::getline(std::cin, name);
		cls
	}

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
							"# JP:\x0D\x0A"
							+ name + "\x0D\x0A"
							"\x0D\x0A"
							"# EN:\x0D\x0A"
							+ name + "\x0D\x0A"
							"\x0D\x0A"
							"# FR:\x0D\x0A"
							+ name + "\x0D\x0A"
							"\x0D\x0A"
							"# GE:\x0D\x0A"
							+ name + "\x0D\x0A"
							"\x0D\x0A"
							"# IT:\x0D\x0A"
							+ name + "\x0D\x0A"
							"\x0D\x0A"
							"# SP:\x0D\x0A"
							+ name + "\x0D\x0A"
							"\x0D\x0A"
							"# CN:\x0D\x0A"
							+ name + "\x0D\x0A"
							"\x0D\x0A"
							"# KO:\x0D\x0A"
							+ name + "\x0D\x0A"
							"\x0D\x0A"
							"# DU:\x0D\x0A"
							+ name + "\x0D\x0A"
							"\x0D\x0A"
							"# PO:\x0D\x0A"
							+ name + "\x0D\x0A"
							"\x0D\x0A"
							"# RU:\x0D\x0A"
							+ name + "\x0D\x0A"
							"\x0D\x0A"
							"# TW:\x0D\x0A"
							+ name + "\x0D\x0A"
							"\x0D\x0A"
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
		while(publisher == "") {
			puts("Enter the Publisher of the video");
			std::getline(std::cin, publisher);
			if(publisher == "") cls
		}
		settingsTL << UTF8toUTF16("\x0D\x0A"
								"# 動画の数\x0D\x0A"//amount of videos
								+ std::to_string(amount) + "\x0D\x0A"
								"\x0D\x0A"
								"# 動画パブリッシャー名\x0D\x0A"//publisher name
								"# JP:\x0D\x0A"
								+ publisher + "\x0D\x0A"
								"\x0D\x0A"
								"# EN:\x0D\x0A"
								+ publisher + "\x0D\x0A"
								"\x0D\x0A"
								"# FR:\x0D\x0A"
								+ publisher + "\x0D\x0A"
								"\x0D\x0A"
								"# GE:\x0D\x0A"
								+ publisher + "\x0D\x0A"
								"\x0D\x0A"
								"# IT:\x0D\x0A"
								+ publisher + "\x0D\x0A"
								"\x0D\x0A"
								"# SP:\x0D\x0A"
								+ publisher + "\x0D\x0A"
								"\x0D\x0A"
								"# CN:\x0D\x0A"
								+ publisher + "\x0D\x0A"
								"\x0D\x0A"
								"# KO:\x0D\x0A"
								+ publisher + "\x0D\x0A"
								"\x0D\x0A"
								"# DU:\x0D\x0A"
								+ publisher + "\x0D\x0A"
								"\x0D\x0A"
								"# PO:\x0D\x0A"
								+ publisher + "\x0D\x0A"
								"\x0D\x0A"
								"# RU:\x0D\x0A"
								+ publisher + "\x0D\x0A"
								"\x0D\x0A"
								"# TW:\x0D\x0A"
								+ publisher + "\x0D\x0A"
								"\x0D\x0A"
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
		if(pathExists("romfs/settings/settingsTL.csv")) completed[1] = 'X';
		else puts("ERROR: Failed to generate romfs/settings/settingsTL.csv");
	}
	else {
		if(pathExists("romfs/settings/settingsTL.csv")) scompleted[1] = 'X';
		else puts("ERROR: Failed to generate romfs/settings/settingsTL.csv");
	}
	settingsTL.close();
	pause
}

void copyright() {
	#if defined(_WIN32)
	system("title [MultiVidInjector5000] Copyright options");
	#endif
	cls
	name = "";
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
	std::ofstream copyrighttxt("romfs/settings/copyright.txt", std::ios_base::out | std::ios_base::binary);
	copyrighttxt << "\xFF\xFE" << UTF8toUTF16(name);
	copyrighttxt.close();
	if(!pathExists("romfs/settings/copyright.txt")) {//how tho like what, your hard drive is 100% full???
		puts("ERROR: Failed to generate romfs/settings/copyright.txt");
		pause
		return;
	}
	completed[2] = 'X';
	pause
}

void tobimg() {
	#if defined(_WIN32)
	system("title [MultiVidInjector5000] Image to .bimg");
	#endif
	cls
	if (amount == 0) {
		puts("Set video amount first!");
		pause
		return;
	}
	std::filesystem::create_directories("romfs/movie");
	size_t size = 0;
	for (unsigned long i = 0; i < amount; i++) {
		name = "";
		while(name == "") {
			cls
			printf("Enter/drag and drop image for video #%li\n(The image should be 200x120 for best results)\n", i+1);
			std::getline(std::cin, name);
			removeQuotes(name);
			if(!pathExists(name)) {
				printf("Error with the file (%s) Try again.\n", name.c_str());
				name = "";
				if(!goorQuit()) return;
			}
		}
		
		//cmd code stuff heh
		copyfile(name, "romfs/movie/temp.png");
		convertToBanner("romfs/movie/temp.png", "romfs/movie/COMMON0.png");
		remove("romfs/movie/temp.png");
		std::string cmd = system_g(_toolsPath + _3dstexPath + " -ro rgb565 \"romfs/movie/COMMON0.png\" \"romfs/movie/movie_" + std::to_string(i) + ".bimg.part2\"");
		if(Debug) {printf("[cmd] %s\n", cmd.c_str()); pause}
		remove("romfs/movie/COMMON0.png");
		
		std::ifstream bimgfile ("romfs/movie/movie_" + std::to_string(i) + ".bimg.part2", std::ios::binary);
		std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(bimgfile), {});//https://stackoverflow.com/a/5420568
		
		std::ofstream finalbimgfile("romfs/movie/movie_" + std::to_string(i) + ".bimg", std::ios_base::out | std::ios_base::binary);
		//write data to the file one byte at a time because for some dang reason vector::data(); was being annoying with it WHYHWYHWHYWHYWHY
		for (const auto &i : bimgheader)
			finalbimgfile << i;
		for (const auto &c : buffer)
			finalbimgfile << c;
		//sweep everything away (do i need to?)
		bimgfile.close();
		finalbimgfile.close();
		buffer.clear();
		std::filesystem::remove("romfs/movie/movie_" + std::to_string(i) + ".bimg.part2");//instead of fixing this so that remove() works im gonna use std::filesystem HEHEHEHEHHEH
		size = std::filesystem::file_size("romfs/movie/movie_" + std::to_string(i) + ".bimg");
		if(size < 0x10020) {
			printf("ERROR: Failed to generate romfs/movie/movie_%li.bimg, try again.\n", i);
			i--;
			if(!goorQuit()) return;
		}
		puts("");//hah
	}
	completed[3] = 'X';
	for (unsigned long i = 0; i < amount; i++) {
		size = std::filesystem::file_size("romfs/movie/movie_" + std::to_string(i) + ".bimg");
		if(size < 0x10020) completed[3] = ' ';
	}
	pause
}

void moflexMover() {
	#if defined(_WIN32)
	type = MultiVid ? "MultiVidInjector5000" : "VidInjector9001";
	system_g("title [" + type + "] Injecting .moflex");
	#endif
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
			if(!pathExists("romfs/movie/movie_" + std::to_string(i) + ".moflex")) {//this probably only happens if there's no disk space
				printf("ERROR: Failed to copy \"%s\" to romfs/movie/movie_%li.moflex\n", name.c_str(), i);
				name = "";
				if(!goorQuit()) return;
			}
		}
		else {
			copyfile(name, "romfs/movie/movie.moflex");
			if(!pathExists("romfs/movie/movie.moflex")) {//this probably only happens if there's no disk space
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
	#if defined(_WIN32)
	type = MultiVid ? "MultiVidInjector5000" : "VidInjector9001";
	system_g("title [" + type + "] Generate banner");
	#endif
	cls
	std::filesystem::create_directory("exefs");
	name = "";
	while(name == "") {
		cls
		puts("Enter/drag and drop your home screen banner image:\n(The image should be 200x120 for best results)");
		std::getline(std::cin, name);
		removeQuotes(name);
		if(!pathExists(name)) {
			printf("Error with the file (%s) Try again.\n", name.c_str());
			name = "";
			if(!goorQuit()) return;
		}
	}
	
	copyfile(name, "exefs/temp.png");
	convertToBanner("exefs/temp.png", "exefs/COMMON0.png");
	remove("exefs/temp.png");
	std::string cmd = system_g(_toolsPath + _3dstexPath + " -ro rgb565 \"exefs/COMMON0.png\" \"exefs/banner.bimg.part\"");
	if(Debug) {printf("[cmd] %s\n", cmd.c_str()); pause}
	remove("exefs/COMMON0.png");
	if(!pathExists("exefs/banner.bimg.part")) {
		puts("ERROR: Failed to convert image.");
		pause
		return;
	}
	puts("");//haha pwetty cmd
	
	std::ifstream bimgfile ("exefs/banner.bimg.part", std::ios::binary);
	std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(bimgfile), {});//https://stackoverflow.com/a/5420568
	//create bcmdl
	std::ofstream bannerbcmdl("exefs/banner0.bcmdl", std::ios_base::out | std::ios_base::binary);
	for (const auto& i : bannerheader)
		bannerbcmdl << i;
	for (const auto& i : buffer)
		bannerbcmdl << i;
	for (const auto& i : bannerfooter)
		bannerbcmdl << i;
	bimgfile.close();
	bannerbcmdl.close();
	remove("exefs/banner.bimg.part");
	//build banner
	copyfile("Vidinjector9000Resources/files/banner.bcwav", "exefs/banner.bcwav");
	copyfile("Vidinjector9000Resources/files/banner.cbmd", "exefs/banner.cbmd");
	system_g(_toolsPath + _3dstoolPath + " -cvtf banner \"exefs/banner.bin\" --banner-dir \"exefs\"");
	//clean up time
	remove("exefs/banner.bcwav");
	remove("exefs/banner0.bcmdl");
	remove("exefs/banner.cbmd");

	if(!pathExists("exefs/banner.bin")) {
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
	#if defined(_WIN32)
	type = MultiVid ? "MultiVidInjector5000" : "VidInjector9001";
	system_g("title [" + type + "] Generate icon");
	#endif
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
		if(!pathExists(name)) {
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
	std::string cmd = system_g(_toolsPath + _bannertoolPath + " makesmdh -i \"exefs/Icon.png\" -s \"" + shortname + "\" -l \"" + longname + "\" -p \"" + publisher + "\" -f visible,nosavebackups -o \"exefs/icon.bin");
	if(Debug) {printf("[cmd] %s\n", cmd.c_str()); pause}
	remove("exefs/Icon.png");

	if(MultiVid) {
		copyfile("exefs/icon.bin", "romfs/icon.icn");
		if(!pathExists("romfs/icon.icn")) {
			puts("ERROR: Failed to copy exefs/icon.bin to romfs/icon.icn");
			pause
			return;
		}
	}
	if(!pathExists("exefs/icon.bin")) {//more stupid-proofing
		puts("ERROR: Failed to generate exefs/icon.bin");
		pause
		return;
	}
	completed[6] = 'X';
	scompleted[4] = 'X';
	pause
}

void customBanner() {
	#if defined(_WIN32)
	type = MultiVid ? "MultiVidInjector5000" : "VidInjector9001";
	system_g("title [" + type + "] Generate banner");
	#endif

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
				name == "";
				if(!goorQuit()) return;
				break;
			} else pass = true;
		}
	}
	copyfile(name, "exefs/banner.bin");
	if(!pathExists("exefs/banner.bin")) {
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
	#if defined(_WIN32)
	type = MultiVid ? "MultiVidInjector5000" : "VidInjector9001";
	system_g("title [" + type + "] Generate CIA");
	#endif
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
	/*if(romfsize + exefsize + exheadersize >= 4294967295) {//the fat32 file size limit (the output cia will be a little bit lower than this estimate but idc so cry about it)
		printf("ERROR: The estimated file size (%lli) of the cia file is too big and will\nnot install to a 3ds nor work in the emulator.\n", (romfsize + exefsize));
		pause
		return;
	}*/
	
	while(TID == max) {
		cls
		puts("Enter 5 hex integers for the ID of your cia (C0000 - EFFFF) or\njust type \"0\" for a random title ID.\n(TID is in format 000400000XXXXX00 (that's hex), the rest will auto fill)");
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
	if(!pathExists("output/" + longname + " [000400000" + std::string(buffer) + "00].cia")) {
		printf("ERROR: Failed to build: \"output\\%s [000400000%s00].cia\"\n", longname.c_str(), std::string(buffer).c_str());
	}
	size_t cia_size(std::filesystem::file_size("output/" + longname + " [000400000" + std::string(buffer) + "00].cia"));
	if(cia_size > 4294967296) {
		printf("WARNING: The file size (%lli) of the cia file is too big and will\nnot install to a 3ds nor work in the emulator.\n", cia_size);
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
		"| Software Version: 2.5.8                  |\n"
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
	#if defined(_WIN32)
	type = MultiVid ? "MultiVidInjector5000" : "VidInjector9001";
	system_g("title [" + type + "] Finalizing");
	#endif
	cls
	for (unsigned int i = 0; i < (MultiVid ? sizeof(completed)-3 : sizeof(scompleted)-3); i++)
		if((MultiVid ? completed : scompleted)[i] == ' ') {
			printf("Job #%i has not been done. Do you really want to continue? [Y/N]\n", i+1);
			std::getline(std::cin, name);
			if(tolower(name[0]) == 'y') break;
			return;
		}
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
		#if defined(_WIN32)
		system("title MultiVidInjector5000 by Foofoo_the_guy");
		#endif
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
		#if defined(_WIN32)
		system("title VidInjector9001 by Foofoo_the_guy");
		#endif
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
		#if defined(_WIN32)
		system("title VidInjector9001 by Foofoo_the_guy");
		#endif
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