#include "VidInjector9001.h"

//globals
#define pause system("pause");
#define cls system("cls");
unsigned long amount = 0;
std::string name = "";
std::string type;
std::string longname;//you need this global because i said so also because make cia needs it too
static bool MultiVid;//false if single video, true if multi video
char completed[9] { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', };
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

inline bool exists_test0 (const std::string& name) {//https://stackoverflow.com/a/12774387
    std::ifstream f(name.c_str());
    return f.good();
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

unsigned long currentTime() {//originally from https://stackoverflow.com/a/30898778
	time_t now = time(0);
	struct tm tstruct;
	char stime[80];
	tstruct = *localtime(&now);
	time_t t = time(0);   // get time now
	struct tm * nowtime = localtime(&t);
	char date[7];

	sprintf(date, "%i%i%i", nowtime->tm_mon + 1, nowtime->tm_mday + 1, nowtime->tm_year + 1900);
	strftime(stime, sizeof(stime), "%H%M%S", &tstruct);

	unsigned long dOut = 0;
	unsigned long sOut = 0;
	if(!stoul_s(dOut, std::string(date))) return 0;
	if(!stoul_s(sOut, std::string(stime))) return 0;

	return dOut + sOut;
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
	uint16_t *utf16 = new uint16_t[input.size() * 2];
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

void removeQuotes(std::string &str) {
	std::string out;
	for (size_t i = 0; i < str.size(); i++) {
		if(str[i] != '\"') out += str[i];//pass through without the " if it has it there
	}
	str = out;
}

void system_g(std::string input) {//system_g()! It's system() but good!
	system(input.c_str());
}

bool Generate_Code(bool Multi) {
	std::string path = "exefs/code.bin";
	puts("Generating code.bin...");
	_mkdir("exefs");
	if(Multi) {
		std::ofstream codebin(path, std::ios_base::out | std::ios_base::binary);
		for (unsigned int i = 0; i < sizeof(Multivid); i++)
			codebin << Multivid[i];
	}
	else {
		std::ofstream codebin(path, std::ios_base::out | std::ios_base::binary);
		for (unsigned int i = 0; i < sizeof(Singlevid); i++)
			codebin << Singlevid[i];
	}
	if(exists_test0(path)) return true;
	return false;
}

//big functions
void setAmount() {
	system("title [MultiVidInjector5000] Set video amount");
	static bool good = false;
	good = false;//in case you do it again, idk why the line above this isnt enough also why should anyone be able to do it again? bruhh
	if (amount) amount = 0;
	_mkdir("romfs/settings");
	std::ofstream movie_bnrname("romfs/settings/movie_bnrname.csv", std::ios_base::out | std::ios_base::binary);
	while(!good) {
		cls
		std::cout << "Enter the amount of videos you have:\n";
		std::getline(std::cin, name);
		if(name.size() > 2 || !stoul_s(amount, name) || amount > 27) {
			std::cout << "Invalid input/The amount cannot be greater than 27. Try again\n";
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
	completed[0] = 'X';
}

void Movie_title() {
	type = MultiVid ? "MultiVidInjector5000" : "VidInjector9001";
	system_g("title [" + type + "] Generate movie_title.csv");
	cls
	if (amount == 0) {
		std::cout << "Set video amount first!\n";
		pause
		return;
	}
	_mkdir("romfs/movie");
	std::ofstream movie_title("romfs/movie/movie_title.csv", std::ios_base::out | std::ios_base::binary);
	movie_title << "\xFF\xFE" + UTF8toUTF16("#JP,#EN,#FR,#GE,#IT,#SP,#CH,#KO,#DU,#PO,#RU,#TW\x0D\x0A");
	for (unsigned long i = 0; i < amount; i++) {
		name = "";
		while(name == "") {
			if(MultiVid) std::cout << "Enter the title of video #" << i+1 << std::endl;
			else std::cout << "Enter the title of the video\n";
			std::getline(std::cin, name);
			if(name == "") cls
		}
		for (int j = 0; j < 11; j++) {//do it 11 times because it needs to
			movie_title << UTF8toUTF16(name + ",");
		}
		movie_title << UTF8toUTF16(name + "\x0D\x0A");//put the last stuff
	}
	movie_title.close();
	if(MultiVid) completed[1] = 'X';
	else scompleted[0] = 'X';
	pause
}

void makesettingsTL() {
	type = MultiVid ? "MultiVidInjector5000" : "VidInjector9001";
	system_g("title [" + type + "] Generate settingsTL.csv");
	cls

	if (amount == 0) {
		std::cout << "Set video amount first!\n";
		pause
		return;
	}

	std::string publisher = "";
	
	name = "";
	while(name == "") {
		if(MultiVid) std::cout << "Enter the name of the series\n";
		else std::cout << "Enter the name of the video\n";
		std::getline(std::cin, name);
		if(name == "") cls
	}
	
	_mkdir("romfs/settings");
	std::ofstream settingsTL("romfs/settings/settingsTL.csv", std::ios_base::out | std::ios_base::binary);
	settingsTL << "\xFF\xFE" + 
				  UTF8toUTF16("# おしらせURL\x0D\x0A"//this is unreadable but oh well HAHHHEHEHEHHE
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
							  "# アプリ名（ロングネーム）\x0D\x0A"
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
							  "12345\x0D\x0A"//idk if any other game uses 12345 as its save data ID, nor do i understand if it matters, so sorry if this breaks everything
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
							  "# おしらせのあり、なし\x0D\x0A"
							  "false\x0D\x0A"
							  "\x0D\x0A"
							  "# 早送り、巻戻しボタンのあり、なし\x0D\x0A"
							  "true\x0D\x0A"
							  "\x0D\x0A"
							  "# 優しさ演出のあり、なし\x0D\x0A"
							  "true\x0D\x0A");
	if(MultiVid) {
		publisher = "";
		while(publisher == "") {
			std::cout << "Enter the Publisher of the video\n";
			std::getline(std::cin, publisher);
			if(publisher == "") cls
		}
		settingsTL << UTF8toUTF16("\x0D\x0A"
								  "# 動画の数\x0D\x0A"
								  + std::to_string(amount) + "\x0D\x0A"
								  "\x0D\x0A"
								  "# 動画パブリッシャー名\x0D\x0A"
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
								  "# WEBブラウザ用のURL\x0D\x0A"
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
		completed[2] = 'X';
	}
	else scompleted[1] = 'X';
	settingsTL.close();
	pause
}

void copyright() {
	system("title [MultiVidInjector5000] Copyright options");
	cls
	name = "";
	_mkdir("romfs/settings");
	while(name == "") {
		std::ofstream information_buttons("romfs/settings/information_buttons.csv", std::ios_base::out | std::ios_base::binary);
		std::cout << "Do you want the menu to have the Copyright button? [Y/N]\n";
		std::getline(std::cin, name);
		if(name == "") cls
		else if(tolowerstr(name) == "y") {
			information_buttons << "\xFF\xFE" << UTF8toUTF16("Copyright");
			information_buttons.close();
		}
		else {
			information_buttons << "\xFF\xFE";
			information_buttons.close();
			completed[3] = 'X';
			return;
		}
	}
	cls
	name = "";
	while(name == "") {
		std::cout << "Enter the text for the copyright menu:\n";
		std::getline(std::cin, name);
		if(name == "") cls
	}
	std::ofstream copyrighttxt("romfs/settings/copyright.txt", std::ios_base::out | std::ios_base::binary);
	copyrighttxt << "\xFF\xFE" << UTF8toUTF16(name);
	copyrighttxt.close();
	completed[3] = 'X';
	pause
}

void tobimg() {
	system("title [MultiVidInjector5000] Image to .bimg");
	cls
	if (amount == 0) {
		std::cout << "Set video amount first!\n";
		pause
		return;
	}
	_mkdir("romfs/movie");
	for (unsigned long i = 0; i < amount; i++) {
		name = "";
		while(name == "") {
			cls
			std::cout << "Enter/drag and drop image for video #" << i+1 << "\n(The image should be 200x120 for best results)\n";
			std::getline(std::cin, name);
			removeQuotes(name);
			if(!exists_test0(name)) {
				std::cout << "Error with the file (" + name + ") Try again.\n";
				name = "";
				pause
			}
		}
		
		//cmd code stuff heh
		system_g("Vidinjector9000Resources\\tools\\imagemagick\\magick.exe \"" + name + "\" -resize 200x120! -background black -compose Copy -gravity northwest -extent 256x128 -flip \"romfs\\movie\\COMMON0.png\"");
		system_g("Vidinjector9000Resources\\tools\\3dstex-win-x86.exe -ro rgb565 \"romfs\\movie\\COMMON0.png\" \"romfs\\movie\\movie_" + std::to_string(i) + ".bimg.part2\"");
		system("del romfs\\movie\\COMMON0.png");
		
		std::ifstream bimgfile ("romfs\\movie\\movie_" + std::to_string(i) + ".bimg.part2", std::ios::binary);
		std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(bimgfile), {});//https://stackoverflow.com/a/5420568
		
		std::ofstream finalbimgfile("romfs\\movie\\movie_" + std::to_string(i) + ".bimg", std::ios_base::out | std::ios_base::binary);
		//write data to the file one byte at a time because for some dang reason vector::data(); was being annoying with it WHYHWYHWHYWHYWHY
		for (unsigned int i = 0; i < sizeof(bimgheader); i++)
			finalbimgfile << bimgheader[i];
		for (size_t i = 0; i < buffer.size(); i++)
			finalbimgfile << buffer.at(i);
		//sweep everything away
		bimgfile.close();
		finalbimgfile.close();
		buffer.clear();
		system_g("del romfs\\movie\\movie_" + std::to_string(i) + ".bimg.part2");
		std::cout << std::endl;
	}
	completed[4] = 'X';
	pause
}

void moflexMover() {
	type = MultiVid ? "MultiVidInjector5000" : "VidInjector9001";
	system_g("title [" + type + "] Injecting .moflex");
	cls
	static bool pass = false;
	unsigned char Checker[4];
	if (amount == 0) {
		std::cout << "Set video amount first!\n";
		pause
		return;
	}
	_mkdir("romfs/movie");
	for (unsigned long i = 0; i < amount; i++) {
		while(!pass) {
			cls

			if(MultiVid) std::cout << "Enter/drag and drop moflex file #" << i+1 << std::endl;
			else std::cout << "Enter/drag and drop the moflex file\n";
			std::getline(std::cin, name);
			removeQuotes(name);
			
			std::string extension = name;
			extension.erase (extension.begin(), extension.end()-7);
			std::ifstream inmoflex (name, std::ios::binary);
			for (int i = 0; i < 4; i++) {
				inmoflex >> Checker[i];//https://stackoverflow.com/a/2974735
				if(extension != ".moflex" || Checker[i] != moflexMagic[i]) {
					std::cout << "The input file (" << name << ") is broken or not in moflex format. Try again.\n";
					name == "";
					pause
					break;
				} else pass = true;
			}
		}
		if(MultiVid) system_g("copy /b \"" + name + "\" \"romfs\\movie\\movie_" + std::to_string(i) + ".moflex\"");
		else system_g("copy /b \"" + name + "\" \"romfs\\movie\\movie.moflex\"");
		pass = false;
	}
	if(MultiVid) completed[5] = 'X';
	else scompleted[2] = 'X';
	pause
}

void makebanner() {
	type = MultiVid ? "MultiVidInjector5000" : "VidInjector9001";
	system_g("title [" + type + "] Generate banner");
	cls
	_mkdir("exefs");
	name = "";
	while(name == "") {
		cls
		std::cout << "Enter/drag and drop your home screen banner image:\n(The image should be 200x120 for best results)\n";
		std::getline(std::cin, name);
		removeQuotes(name);
		if(!exists_test0(name)) {
			std::cout << "Error with the file (" + name + ") Try again.\n";
			name = "";
			pause
		}
	}
	
	system_g("Vidinjector9000Resources\\tools\\imagemagick\\magick.exe \"" + name + "\" -resize 200x120! -background black -compose Copy -gravity northwest -extent 256x128 -flip \"exefs\\COMMON0.png\"");
	system_g("Vidinjector9000Resources\\tools\\3dstex-win-x86.exe -ro rgb565 \"exefs\\COMMON0.png\" \"exefs\\banner.bimg.part\"");
	system("del exefs\\COMMON0.png");
	std::cout << std::endl;//haha pwetty cmd
	
	std::ifstream bimgfile ("exefs\\banner.bimg.part", std::ios::binary);
	std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(bimgfile), {});//https://stackoverflow.com/a/5420568
	//create bcmdl
	std::ofstream bannerbcmdl("exefs\\banner0.bcmdl", std::ios_base::out | std::ios_base::binary);
	for (unsigned int i = 0; i < sizeof(bannerheader); i++)
		bannerbcmdl << bannerheader[i];
	for (size_t i = 0; i < buffer.size(); i++)
		bannerbcmdl << buffer.at(i);
	for (unsigned int i = 0; i < sizeof(bannerfooter); i++)
		bannerbcmdl << bannerfooter[i];
	bimgfile.close();
	bannerbcmdl.close();
	remove("exefs/banner.bimg.part");
	//build banner
	system("copy /b \"Vidinjector9000Resources\\files\\banner.bcwav\" \"exefs\\banner.bcwav\"");
	system("copy /b \"Vidinjector9000Resources\\files\\banner.cbmd\" \"exefs\\banner.cbmd\"");
	system("Vidinjector9000Resources\\tools\\3dstool.exe -cvtf banner \"exefs\\banner.bin\" --banner-dir \"exefs\"");
	//clean up time
	remove("exefs/banner.bcwav");
	remove("exefs/banner0.bcmdl");
	remove("exefs/banner.cbmd");
	completed[6] = 'X';
	completed[8] = ' ';
	scompleted[3] = 'X';//i realized this has to be like this for the checks
	scompleted[4] = ' ';
	pause
}

void makeIcon() {
	type = MultiVid ? "MultiVidInjector5000" : "VidInjector9001";
	system_g("title [" + type + "] Generate icon");
	cls
	std::string shortname;
	std::string publisher;
	name = "";
	while (name == "") {
		cls
		std::cout << "Enter/drag and drop your icon image:\n(The image should be 48x48 for best results)\n";
		std::getline(std::cin, name);
		removeQuotes(name);
		if(!exists_test0(name)) {
			std::cout << "Error with the file (" + name + ") Try again.\n";
			name = "";
			pause
		}
	}
	cls
	shortname = "";
	while (shortname == "") {//stupid-proofing
		std::cout << "Enter the short name:\n";
		std::getline(std::cin, shortname);
		if(shortname == "") cls
	}
	longname = "";
	while (longname == "") {
		std::cout << "Enter the long name:\n";
		std::getline(std::cin, longname);
		if(longname == "") cls
	}
	publisher = "";
	while (publisher == "") {
		std::cout << "Enter the publisher:\n";
		std::getline(std::cin, publisher);
		if(publisher == "") cls
	}
	
	system_g("Vidinjector9000Resources\\tools\\imagemagick\\magick.exe convert \"" + name + "\" -resize 48x48! -background black -flatten \"exefs\\Icon.png\"");
	system_g("Vidinjector9000Resources\\tools\\bannertool.exe makesmdh -i \"exefs\\Icon.png\" -s \"" + shortname + "\" -l \"" + longname + "\" -p \"" + publisher + "\" -f visible,nosavebackups -o \"exefs/icon.bin");
	remove("exefs\\Icon.png");
	completed[7] = 'X';
	scompleted[4] = 'X';
	pause
}

void customBanner() {
	type = MultiVid ? "MultiVidInjector5000" : "VidInjector9001";
	system_g("title [" + type + "] Generate banner");

	static bool pass = false;
	unsigned char Checker[4];

	while(!pass) {
		cls

		std::cout << "Enter/drag and drop your banner:\n(pre-built file)\n";
		std::getline(std::cin, name);
		removeQuotes(name);
		std::ifstream inbanner (name, std::ios::binary);
		for (int i = 0; i < 4; i++) {
			inbanner >> Checker[i];//https://stackoverflow.com/a/2974735
			if(Checker[i] != bannerMagic[i]) {
				std::cout << "The input file (" << name << ") is not a valid banner. Try again.\n";
				name == "";
				pause
				break;
			} else pass = true;
		}
	}
	system_g("copy /b \"" + name + "\" \"exefs\\banner.bin\"");
	completed[6] = '-';
	completed[8] = 'X';
	scompleted[3] = '-';
	scompleted[5] = 'X';
	pause
}

void makeCIA() {
	type = MultiVid ? "MultiVidInjector5000" : "VidInjector9001";
	system_g("title [" + type + "] Generate CIA");
	cls
	srand(currentTime());
	unsigned long TID = 0xF0000;
	if(MultiVid) {
		for (unsigned int i = 0; i < sizeof(completed)-1; i++)
			if(completed[i] == ' ') {
				printf("Job #%i has not been done. Do you really want to continue? [Y/N]\n", i+1);
				std::getline(std::cin, name);
				if(tolowerstr(name) == "y") break;
				return;
			}
	}
	else {
		for (unsigned int i = 0; i < sizeof(scompleted); i++)
			if(scompleted[i] == ' ') {
				printf("Job #%i has not been done. Do you really want to continue? [Y/N]\n", i+1);
				std::getline(std::cin, name);
				if(tolowerstr(name) == "y") break;
				return;
			}
	}
	while(TID == 0xF0000) {
		cls
		std::cout << "Enter 5 hex integers for the ID of your cia (C0000 - EFFFF) or\njust type \"0\" for a random title ID.\n(TID is in format 000400000XXXXX00 (that's hex), the rest will auto fill)\n";
		std::getline(std::cin, name);
		if(name.size() > 5) name = "F0000";//more stupid-proofing
		if(!stoul_s(TID, name, true)) {
			std::cout << "Invalid input, try again\n";
			pause
			continue;
		}
		if (TID == 0) {
			TID = rand() % (0xF0000 - 0xC0000) + 0xC0000;//0xC0000 is minimum, 0xEFFFF is maximum, 0xF0000 makes it include 0xEFFFF in it)
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
				pause
				continue;
			}
			break;
			default:
				if(TID < 0xF0000 && TID > 0xC0001) printf("%05lX Passed all checks!\n", TID);
				else {
					printf("Oops, you ran into a blacklisted ID! (%05lX) Try again.\n", TID);
					pause
					continue;
				}
		}
		break;//this will only be called if it doesn't do continue
		TID = 0xF0000;
	}
	std::cout << "Generating CIA...\n";
	_mkdir("output");
	char buffer[6];
    sprintf(buffer, "%05lX", TID);
	system_g("Vidinjector9000Resources\\tools\\makerom.exe -f cia -o \"output\\" + longname + " [000400000" + std::string(buffer) + "00].cia\" -banner \"exefs\\banner.bin\" -icon \"exefs\\icon.bin\" -code \"exefs\\code.bin\" -exheader \"exheader.bin\" -rsf \"Vidinjector9000Resources\\files\\template.rsf\" -DAPP_UNIQUE_ID=" + std::to_string(TID));
	pause
	cls
	std::cout << "Do you want this to delete the following folders/files? [Y/N]\n- exefs\n- romfs\n- exheader.bin\n";
	std::getline(std::cin, name);
	if(tolowerstr(name) == "y") {
		system("rmdir exefs /s /q");
		system("rmdir romfs /s /q");
		remove("exheader.bin");
		main();
	}
	pause
}

//2nd menu
void finalize() {
	type = MultiVid ? "MultiVidInjector5000" : "VidInjector9001";
	system_g("title [" + type + "] Finalizing");
	cls
	if(MultiVid) {
		for (unsigned int i = 0; i < sizeof(completed)-3; i++)
			if(completed[i] == ' ') {
				printf("Job #%i has not been done. Do you really want to continue? [Y/N]\n", i+1);
				std::getline(std::cin, name);
				if(tolowerstr(name) == "y") break;
				return;
			}
	}
	else {
		for (unsigned int i = 0; i < sizeof(scompleted)-3; i++)
			if(scompleted[i] == ' ') {
				printf("Job #%i has not been done. Do you really want to continue? [Y/N]\n", i+1);
				std::getline(std::cin, name);
				if(tolowerstr(name) == "y") break;
				return;
			}
	}
	while(1) {
		cls
		type = MultiVid ? "multi video menu            |" : "single video menu           |";
		printf("Type a letter:\n\n"
		" __________________________________________\n"
		"|                                          |\n"
		"| B: Generate banner                   [%c] |\n"
		"| I: Generate icon                     [%c] |\n"
		"| U: Use a custom banner file          [%c] |\n"//do U because i cant do C or B or idk it just means "use"
		"|                                          |\n"
		"| C: Generate CIA                          |\n"
		"| X: Go to the %s\n"
		"|__________________________________________|\n\n", completed[6], completed[7], completed[8], type.c_str());
		std::getline(std::cin, name);
		if(tolowerstr(name) == "b") makebanner();//6
		else if(tolowerstr(name) == "i") makeIcon();//7
		else if(tolowerstr(name) == "u") customBanner();//8
		else if(tolowerstr(name) == "c") makeCIA();
		else if(tolowerstr(name) == "x") return;
	}
}

//multi video menu (duh)
void MultiVideo() {
	cls
	MultiVid = true;
	amount = 0;
	if(!Generate_Code(true)) {
		puts("Failed to generate files.");
		pause
		return;
	}
	system("xcopy Vidinjector9000Resources\\files\\templates\\MultiVideo\\romfs romfs\\ /y /e");
	system("copy Vidinjector9000Resources\\files\\templates\\MultiVideo\\exheader.bin exheader.bin");
	while(1) {
		system("title MultiVidInjector5000 by Foofoo_the_guy");
		cls
		printf("Type a letter:\n\n"
		" __________________________________________\n"
		"|                                          |\n"
		"| A: Set the amount of videos you have [%c] |\n"
		"| T: movie_title.csv generator         [%c] |\n"
		"| S: settingsTL.csv generator          [%c] |\n"
		"| C: Copyright options                 [%c] |\n"
		"| B: Image to banner image converter   [%c] |\n"
		"| M: Select/Copy over the moflex files [%c] |\n"
		"|                                          |\n"
		"| F: Finalize/build the CIA                |\n"
		"| X: Go to the main menu                   |\n"
		"|__________________________________________|\n\n", completed[0], completed[1], completed[2], completed[3], completed[4], completed[5]);
		std::getline(std::cin, name);
		if(tolowerstr(name) == "a") setAmount();//0
		else if(tolowerstr(name) == "t") Movie_title();//1
		else if(tolowerstr(name) == "s") makesettingsTL();//2
		else if(tolowerstr(name) == "c") copyright();//3
		else if(tolowerstr(name) == "b") tobimg();//4
		else if(tolowerstr(name) == "m") moflexMover();//5
		else if(tolowerstr(name) == "f") finalize();
		else if(tolowerstr(name) == "x") return;
	}
}

//single video menu (duh)
void SingleVideo() {
	cls
	MultiVid = false;
	amount = 1;//huhuhu ez way to reuse the functions
	if(!Generate_Code(false)) {
		puts("Failed to generate files.");
		pause
		return;
	}
	system("xcopy Vidinjector9000Resources\\files\\templates\\SingleVideo\\romfs romfs\\ /y /e");
	system("copy Vidinjector9000Resources\\files\\templates\\SingleVideo\\exheader.bin exheader.bin");
	while(1) {
		system("title VidInjector9001 by Foofoo_the_guy");
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
		if(tolowerstr(name) == "t") Movie_title();//0
		else if(tolowerstr(name) == "s") makesettingsTL();//1
		else if(tolowerstr(name) == "m") moflexMover();//2
		else if(tolowerstr(name) == "f") finalize();
		else if(tolowerstr(name) == "x") return;
	}
}

//main menu
int main() {
	while(1) {
		system("title VidInjector9001 by Foofoo_the_guy");
		cls
		//wipe the stuff
		for (unsigned int i = 0; i < sizeof(completed); i++)
			completed[i] = ' ';
		for (unsigned int i = 0; i < sizeof(scompleted); i++)
			scompleted[i] = ' ';
		system("rmdir exefs /s /q");
		system("rmdir romfs /s /q");
		remove("exheader.bin");
		
		cls
		printf("Type a letter:\n\n"
		" __________________________________________\n"//woah ah it looks so cool how did you do that? weee
		"|                                          |\n"
		"| S: Singular Video Injector               |\n"
		"| M: Multiple Video Injector               |\n"
		"|__________________________________________|\n\n");
		std::getline(std::cin, name);
		if(tolowerstr(name) == "s") SingleVideo();//0
		else if(tolowerstr(name) == "m") MultiVideo();//1
	}
	return 0;
}