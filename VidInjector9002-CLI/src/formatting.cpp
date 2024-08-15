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

std::string to_UTF8(const nnc_u16* UTF16, const size_t UTF16size) {
	size_t UTF8size = nnc_utf16_to_utf8(NULL, 0, UTF16, UTF16size) + 1;
	std::vector<uint8_t> str = std::vector<uint8_t>(UTF8size);
	size_t rlen = nnc_utf16_to_utf8(str.data(), UTF8size, UTF16, UTF16size);
	std::string outstr(reinterpret_cast<char*>(str.data()), rlen);
	return outstr;
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

//stolen from https://github.com/Zer0Mem0ry/CRC32/blob/master/CRC32/CRC32.cpp with some edits to the types
uint32_t uiCRC32_Table[256] = {
	0x00000000L, 0x77073096L, 0xEE0E612CL,
	0x990951BAL, 0x076DC419L, 0x706AF48FL,
	0xE963A535L, 0x9E6495A3L, 0x0EDB8832L,
	0x79DCB8A4L, 0xE0D5E91EL, 0x97D2D988L,
	0x09B64C2BL, 0x7EB17CBDL, 0xE7B82D07L,
	0x90BF1D91L, 0x1DB71064L, 0x6AB020F2L,
	0xF3B97148L, 0x84BE41DEL, 0x1ADAD47DL,
	0x6DDDE4EBL, 0xF4D4B551L, 0x83D385C7L,
	0x136C9856L, 0x646BA8C0L, 0xFD62F97AL,
	0x8A65C9ECL, 0x14015C4FL, 0x63066CD9L,
	0xFA0F3D63L, 0x8D080DF5L, 0x3B6E20C8L,
	0x4C69105EL, 0xD56041E4L, 0xA2677172L,
	0x3C03E4D1L, 0x4B04D447L, 0xD20D85FDL,
	0xA50AB56BL, 0x35B5A8FAL, 0x42B2986CL,
	0xDBBBC9D6L, 0xACBCF940L, 0x32D86CE3L,
	0x45DF5C75L, 0xDCD60DCFL, 0xABD13D59L,
	0x26D930ACL, 0x51DE003AL, 0xC8D75180L,
	0xBFD06116L, 0x21B4F4B5L, 0x56B3C423L,
	0xCFBA9599L, 0xB8BDA50FL, 0x2802B89EL,
	0x5F058808L, 0xC60CD9B2L, 0xB10BE924L,
	0x2F6F7C87L, 0x58684C11L, 0xC1611DABL,
	0xB6662D3DL, 0x76DC4190L, 0x01DB7106L,
	0x98D220BCL, 0xEFD5102AL, 0x71B18589L,
	0x06B6B51FL, 0x9FBFE4A5L, 0xE8B8D433L,
	0x7807C9A2L, 0x0F00F934L, 0x9609A88EL,
	0xE10E9818L, 0x7F6A0DBBL, 0x086D3D2DL,
	0x91646C97L, 0xE6635C01L, 0x6B6B51F4L,
	0x1C6C6162L, 0x856530D8L, 0xF262004EL,
	0x6C0695EDL, 0x1B01A57BL, 0x8208F4C1L,
	0xF50FC457L, 0x65B0D9C6L, 0x12B7E950L,
	0x8BBEB8EAL, 0xFCB9887CL, 0x62DD1DDFL,
	0x15DA2D49L, 0x8CD37CF3L, 0xFBD44C65L,
	0x4DB26158L, 0x3AB551CEL, 0xA3BC0074L,
	0xD4BB30E2L, 0x4ADFA541L, 0x3DD895D7L,
	0xA4D1C46DL, 0xD3D6F4FBL, 0x4369E96AL,
	0x346ED9FCL, 0xAD678846L, 0xDA60B8D0L,
	0x44042D73L, 0x33031DE5L, 0xAA0A4C5FL,
	0xDD0D7CC9L, 0x5005713CL, 0x270241AAL,
	0xBE0B1010L, 0xC90C2086L, 0x5768B525L,
	0x206F85B3L, 0xB966D409L, 0xCE61E49FL,
	0x5EDEF90EL, 0x29D9C998L, 0xB0D09822L,
	0xC7D7A8B4L, 0x59B33D17L, 0x2EB40D81L,
	0xB7BD5C3BL, 0xC0BA6CADL, 0xEDB88320L,
	0x9ABFB3B6L, 0x03B6E20CL, 0x74B1D29AL,
	0xEAD54739L, 0x9DD277AFL, 0x04DB2615L,
	0x73DC1683L, 0xE3630B12L, 0x94643B84L,
	0x0D6D6A3EL, 0x7A6A5AA8L, 0xE40ECF0BL,
	0x9309FF9DL, 0x0A00AE27L, 0x7D079EB1L,
	0xF00F9344L, 0x8708A3D2L, 0x1E01F268L,
	0x6906C2FEL, 0xF762575DL, 0x806567CBL,
	0x196C3671L, 0x6E6B06E7L, 0xFED41B76L,
	0x89D32BE0L, 0x10DA7A5AL, 0x67DD4ACCL,
	0xF9B9DF6FL, 0x8EBEEFF9L, 0x17B7BE43L,
	0x60B08ED5L, 0xD6D6A3E8L, 0xA1D1937EL,
	0x38D8C2C4L, 0x4FDFF252L, 0xD1BB67F1L,
	0xA6BC5767L, 0x3FB506DDL, 0x48B2364BL,
	0xD80D2BDAL, 0xAF0A1B4CL, 0x36034AF6L,
	0x41047A60L, 0xDF60EFC3L, 0xA867DF55L,
	0x316E8EEFL, 0x4669BE79L, 0xCB61B38CL,
	0xBC66831AL, 0x256FD2A0L, 0x5268E236L,
	0xCC0C7795L, 0xBB0B4703L, 0x220216B9L,
	0x5505262FL, 0xC5BA3BBEL, 0xB2BD0B28L,
	0x2BB45A92L, 0x5CB36A04L, 0xC2D7FFA7L,
	0xB5D0CF31L, 0x2CD99E8BL, 0x5BDEAE1DL,
	0x9B64C2B0L, 0xEC63F226L, 0x756AA39CL,
	0x026D930AL, 0x9C0906A9L, 0xEB0E363FL,
	0x72076785L, 0x05005713L, 0x95BF4A82L,
	0xE2B87A14L, 0x7BB12BAEL, 0x0CB61B38L,
	0x92D28E9BL, 0xE5D5BE0DL, 0x7CDCEFB7L,
	0x0BDBDF21L, 0x86D3D2D4L, 0xF1D4E242L,
	0x68DDB3F8L, 0x1FDA836EL, 0x81BE16CDL,
	0xF6B9265BL, 0x6FB077E1L, 0x18B74777L,
	0x88085AE6L, 0xFF0F6A70L, 0x66063BCAL,
	0x11010B5CL, 0x8F659EFFL, 0xF862AE69L,
	0x616BFFD3L, 0x166CCF45L, 0xA00AE278L,
	0xD70DD2EEL, 0x4E048354L, 0x3903B3C2L,
	0xA7672661L, 0xD06016F7L, 0x4969474DL,
	0x3E6E77DBL, 0xAED16A4AL, 0xD9D65ADCL,
	0x40DF0B66L, 0x37D83BF0L, 0xA9BCAE53L,
	0xDEBB9EC5L, 0x47B2CF7FL, 0x30B5FFE9L,
	0xBDBDF21CL, 0xCABAC28AL, 0x53B39330L,
	0x24B4A3A6L, 0xBAD03605L, 0xCDD70693L,
	0x54DE5729L, 0x23D967BFL, 0xB3667A2EL,
	0xC4614AB8L, 0x5D681B02L, 0x2A6F2B94L,
	0xB40BBE37L, 0xC30C8EA1L, 0x5A05DF1BL,
	0x2D02EF8DL };

uint32_t CRC32(void* pData, size_t iLen)
{
	uint32_t uiCRC32 = 0xFFFFFFFF;
	uint8_t* pszData = reinterpret_cast<uint8_t*>(pData);

	for (size_t i = 0; i < iLen; ++i)
		uiCRC32 = ((uiCRC32 >> 8) & 0x00FFFFFF) ^ uiCRC32_Table[(uiCRC32 ^ (uint32_t)*pszData++) & 0xFF];

	return (uiCRC32 ^ 0xFFFFFFFF);
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

std::string fixSlashes(std::string instr) {
	for (size_t i = 0; i < instr.size(); i++) {
		if (instr[i] == '\\') {
			instr[i] = '/';
		}
	}
	return instr;
}

std::error_code add_file(mtar_t* tar, std::string filename, std::string arcname, size_t buffersize) {
	std::error_code ec;
	size_t filesize = std::filesystem::file_size(filename, ec);
	if (ec) {
		return ec;
	}

	mtar_write_file_header(tar, arcname.c_str(), filesize);
	for (size_t i = 0; i < filesize; i += buffersize) {
		std::fstream file(filename, std::ios::binary | std::ios::in);
		file.seekg(0, std::ios::end);//https://stackoverflow.com/a/2602258
		std::vector<uint8_t> bytes = std::vector<uint8_t>(buffersize);
		file.seekg(i);
		if (i + buffersize > filesize) {//filesize will probably not be a multiple of buffersize
			file.read(reinterpret_cast<char*>(bytes.data()), filesize - i);
			mtar_write_data(tar, bytes.data(), filesize - i);
		}
		else {
			file.read(reinterpret_cast<char*>(bytes.data()), buffersize);
			mtar_write_data(tar, bytes.data(), buffersize);
		}
	}
	return ec;
}

std::error_code add_directory(mtar_t* tar, std::string dirname, size_t buffersize) {
	std::string arcname = dirname.substr(dirname.find_last_of("/\\") + 1);
	std::error_code ec;
	mtar_write_dir_header(tar, (arcname + '/').c_str());
	for (auto const& dir_entry : std::filesystem::recursive_directory_iterator(dirname, ec)) {
		auto filename_c = std::filesystem::canonical(dir_entry);
		std::string filename = fixSlashes(filename_c.string());
		if (!std::filesystem::is_directory(filename)) {
			if (ec) return ec;
			ec = add_file(tar, filename_c.string(), filename.substr(dirname.size() - arcname.size()), buffersize);
			if (ec) return ec;
		}
		else {
			mtar_write_dir_header(tar, (filename.substr(filename.rfind(arcname + '/')) + '/').c_str());
		}
	}
	return ec;
}

//do read on our own because mtar_read_data didnt work (???)
int read_record_data(mtar_t* tar, mtar_header_t* h, std::string inputfile, std::string outputdir, size_t buffersize) {
	std::ofstream file(std::filesystem::path((const char8_t*)&*(outputdir + '/' + std::string(h->name)).c_str()), std::ios_base::out | std::ios_base::binary);
	std::vector<uint8_t> bytes = std::vector<uint8_t>(buffersize);
	size_t pos = tar->pos + sizeof(mtar_raw_header_t);
	std::fstream tarfile(inputfile, std::ios::binary | std::ios::in);//do tar read like this because something is broken or im missing something
	if (!tarfile.is_open()) return 1;
	for (size_t i = 0; i < h->size; i += buffersize) {
		tarfile.seekg(pos);
		if (i + buffersize > h->size) {//h->size will probably not be a multiple of buffersize
			//puts("write hsize");
			//printf("pos: %lli, h.size: %lli\n", pos, h->size);
			tarfile.read(reinterpret_cast<char*>(bytes.data()), h->size - i);
			file.write(reinterpret_cast<char*>(bytes.data()), h->size - i);
			pos += h->size - i;
		}
		else {
			//puts("write buffersize");
			//printf(">pos: %lli, h.size: %lli\n", pos, h->size);
			tarfile.read(reinterpret_cast<char*>(bytes.data()), buffersize);
			file.write(reinterpret_cast<char*>(bytes.data()), buffersize);
			pos += buffersize;
			//mtar_seek(tar, tar->pos + buffersize);
		}
	}
	return 0;
}

int extract_content(mtar_t* tar, std::string inputfile, std::string outputdir, size_t buffersize) {
	int ret = 0;
	mtar_header_t h;
	//size_t pos = 0;
	std::error_code error;

	while ((mtar_read_header(tar, &h)) != MTAR_ENULLRECORD) {

		mtar_find(tar, h.name, &h);
		//printf("mtar_find tar->pos: %zo\n", tar->pos);

		//printf("%s (%zo bytes), type: %i\n", h.name, h.size, h.type);

		if (h.type == 53) {//directory
			std::filesystem::create_directories(std::filesystem::path((const char8_t*)&*(outputdir + '/' + std::string(h.name)).c_str()), error);
			if (error) return -1;
		}
		else if (h.type == 48) {//file
			if (read_record_data(tar, &h, inputfile, outputdir, buffersize))//0 (false) = good (this isnt bool)
				return MTAR_EFAILURE;
		}
		ret = mtar_next(tar);
		if (ret) return ret;
	}
	return ret;
}