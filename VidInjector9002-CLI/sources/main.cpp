#include <filesystem>
#include <iostream>
#include <cstring>
#include <vector>
#include "CTR_stuff.hpp"
#include "strings.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

int progmain(int argc, char** argv) {
	for(int i = 1; i < argc; i++) {
		if(strcmp(argv[i], HArg.c_str()) == 0 || strcmp(argv[i], HArgShort.c_str()) == 0) {
			std::cout << LogoASCII << '\n' << ProgramName << '\n' << ByMeText << "\n(" << Version << ")\n" <<
			InfoText << '\n' << UsageText << ' ' << argv[0] << ' ' << ExampleText << std::endl <<
			ArgumentText << '\n' <<
			HArg << " | " << HArgShort << '\n' <<
			NArg << " | " << NArgShort << '\n' <<
			SpArg << " | " << SpArgShort << '\n' <<
			SsArg << " | " << SsArgShort << '\n' <<
			PpArg << " | " << PpArgShort << '\n' <<
			ArArg << " | " << ArArgShort << '\n' <<
			SrArg << " | " << SrArgShort << '\n' <<
			BcArg << " | " << BcArgShort << '\n' <<
			EcArg << " | " << EcArgShort << '\n' <<
			EtArg << " | " << EtArgShort << '\n' <<
			GpArg << " | " << GpArgShort << '\n' <<
			RrArg << " | " << RrArgShort << std::endl;
			return 0;
		}
		else if(strcmp(argv[i], NArg.c_str()) == 0 || strcmp(argv[i], NArgShort.c_str()) == 0) {
			if(argc == 3) {
				VI9Pparameters parameters;
				return saveParameters(argv[2], parameters);
			}
			else {
				std::cout << NArg << " | " << NArgShort << " : " << NewInfoText << "\n\n" << UsageText << '\n' << argv[0] << ' ' << NArg << " <" << OutVi9pFile << ">" << std::endl;
			}
			return 1;
		}
		else if(strcmp(argv[i], SpArg.c_str()) == 0 || strcmp(argv[i], SpArgShort.c_str()) == 0) {
			if(argc == 6) {
				unsigned long inNumber = 0;
				if(!stoul_s(inNumber, argv[3])) {
					std::cout << ErrorText << " <" << number << "> " << '\n' << BadValue << " \"" << argv[3] << '\"' << std::endl;
					return 1;
				}
				return setParameter(argv[2], inNumber, argv[4], argv[5]);
			}
			else {
				std::cout << SpArg << " | " << SpArgShort << " : " << SpInfoText << "\n\n" << UsageText << '\n' << argv[0] << ' ' << SpArg << " <" << InVi9pFile << "> <" << number << "> <" << NewValueText << "> <" << OutVi9pFile << ">" << std::endl;
			}
			return 1;
		}
		else if(strcmp(argv[i], SsArg.c_str()) == 0 || strcmp(argv[i], SsArgShort.c_str()) == 0) {
			if(argc == 5) {
				return SetSMDH(argv[2], argv[3], argv[4]);
			}
			else {
				std::cout << SsArg << " | " << SsArgShort << " : " << SsInfoText << "\n\n" << UsageText << '\n' << argv[0] << ' ' << SsArg << " <" << InVi9pFile << "> <" << NewValueText << "> <" << OutVi9pFile << ">" << std::endl;
			}
			return 1;
		}
		else if(strcmp(argv[i], PpArg.c_str()) == 0 || strcmp(argv[i], PpArgShort.c_str()) == 0) {
			if(argc == 3) {
				return printParameter(argv[2]);
			}
			else {
				std::cout << PpArg << " | " << PpArgShort << " : " << PpInfoText << "\n\n" << UsageText << '\n' << argv[0] << ' ' << PpArg << " <" << InVi9pFile << ">" << std::endl;
			}
			return 1;
		}
		else if(strcmp(argv[i], ArArg.c_str()) == 0 || strcmp(argv[i], ArArgShort.c_str()) == 0) {
			if(argc == 4) {
				return add_row(argv[2], argv[3]);
			}
			else {
				std::cout << ArArg << " | " << ArArgShort << " : " << ArInfoText << "\n\n" << UsageText << '\n' << argv[0] << ' ' << ArArg << " <" << InVi9pFile << "> <" << OutVi9pFile << ">" << std::endl;
			}
			return 1;
		}
		else if(strcmp(argv[i], SrArg.c_str()) == 0 || strcmp(argv[i], SrArgShort.c_str()) == 0) {
			if(argc == 4) {
				return sub_row(argv[2], argv[3]);
			}
			else {
				std::cout << SrArg << " | " << SrArgShort << " : " << SrInfoText << "\n\n" << UsageText << '\n' << argv[0] << ' ' << SrArg << " <" << InVi9pFile << "> <" << OutVi9pFile << ">" << std::endl;
			}
			return 1;
		}
		else if(strcmp(argv[i], BcArg.c_str()) == 0 || strcmp(argv[i], BcArgShort.c_str()) == 0) {
			if(argc == 4) {
				return build_archive(argv[2], argv[3]);
			}
			else if(argc == 5) {
				return build_archive(argv[2], argv[3], argv[4]);
			}
			else if(argc == 7) {
				uint32_t inNumber = 0;
				if(!stoul_s(inNumber, argv[3], true)) {
					std::cout << ErrorText << " <" << UniqueIDText << "> " << '\n' << BadValue << " \"" << argv[3] << '\"' << std::endl;
					return 1;
				}
				if(!TIDisValid(inNumber)) {
					std::cout << ErrorText << " <" << UniqueIDText << "> " << '\n' << BadValue << " \"" << argv[3] << '\"' << std::endl;
					return 2;
				}
				return build_archive(argv[2], argv[6], "", inNumber, argv[4], argv[5]);
			}
			else if(argc == 8) {
				uint32_t inNumber = 0;
				if(!stoul_s(inNumber, argv[3], true)) {
					std::cout << ErrorText << " <" << UniqueIDText << "> " << '\n' << BadValue << " \"" << argv[3] << '\"' << std::endl;
					return 1;
				}
				if(!TIDisValid(inNumber)) {
					std::cout << ErrorText << " <" << UniqueIDText << "> " << '\n' << BadValue << " \"" << argv[3] << '\"' << std::endl;
					return 2;
				}
				return build_archive(argv[2], argv[6], argv[7], inNumber, argv[4], argv[5]);
			}
			else {
				std::cout << BcArg << " | " << BcArgShort << " : " << BcInfoText << "\n\n" << UsageText << '\n' << argv[0] << ' ' << BcArg << " <" << InVi9pFile << "> <" << UniqueIDText << "> <" << AppTitleText << "> <" << ProdCodeLatt << "> <" << OutCiaFile << ">\n\n----\n" << std::endl;
				std::cout << BcArg << " | " << BcArgShort << " : " << Bc3InfoText << "\n\n" << UsageText << '\n' << argv[0] << ' ' << BcArg << " <" << InVi9pFile << "> <" << UniqueIDText << "> <" << AppTitleText << "> <" << ProdCodeLatt << "> <" << OutCiaFile << "> <" << OutTarFile << ">\n\n----\n" << std::endl;
				std::cout << BcArg << " | " << BcArgShort << " : " << Bc2InfoText << "\n\n" << UsageText << '\n' << argv[0] << ' ' << BcArg << " <" << InVi9pFile << "> <" << OutCiaFile << ">\n\n----\n" << std::endl;
				std::cout << BcArg << " | " << BcArgShort << " : " << Bc3InfoText << "\n\n" << UsageText << '\n' << argv[0] << ' ' << BcArg << " <" << InVi9pFile << "> <" << OutCiaFile << "> <" << OutTarFile << ">" << std::endl;
			}
			return 1;
		}
		else if(strcmp(argv[i], EcArg.c_str()) == 0 || strcmp(argv[i], EcArgShort.c_str()) == 0) {
			if(argc == 4) {
				return extract_archive(argv[2], argv[3]);
			}
			else if(argc == 5) {
				return extract_archive(argv[2], argv[4], false, argv[3]);
			}
			else {
				std::cout << EcArg << " | " << EcArgShort << " : " << EcInfoText << "\n\n" << UsageText << '\n' << argv[0] << ' ' << EcArg << " <" << InCiaFile << "> <" << OutDir << ">\n\n----\n" << std::endl;
				std::cout << EcArg << " | " << EcArgShort << " : " << Ec2InfoText << "\n\n" << UsageText << '\n' << argv[0] << ' ' << EcArg << " <" << InCiaFile << "> <" << SeedFile << "> <" << OutDir << ">" << std::endl;
			}
			return 1;
		}
		else if(strcmp(argv[i], EtArg.c_str()) == 0 || strcmp(argv[i], EtArgShort.c_str()) == 0) {
			if(argc == 4) {
				return extract_archive(argv[2], argv[3], true);
			}
			else {
				std::cout << EtArg << " | " << EtArgShort << " : " << EtInfoText << "\n\n" << UsageText << '\n' << argv[0] << ' ' << EtArg << " <" << InTarFile << "> <" << OutDir << ">" << std::endl;
			}
			return 1;
		}
		else if(strcmp(argv[i], GpArg.c_str()) == 0 || strcmp(argv[i], GpArgShort.c_str()) == 0) {
			if(argc == 5) {
				unsigned long inNumber = 0;
				if(!stoul_s(inNumber, argv[3])) {
					std::cout << ErrorText << " <" << number << "> " << '\n' << BadValue << " \"" << argv[3] << '\"' << std::endl;
					return 1;
				}
				return generate_preview(argv[2], inNumber, argv[4]);
			}
			else {
				std::cout << GpArg << " | " << GpArgShort << " : " << GpInfoText << "\n\n" << UsageText << '\n' << argv[0] << ' ' << GpArg << " <" << InVi9pFile << "> <" << number << "> <" << OutPngFile << ">" << std::endl;
			}
			return 1;
		}
		else if(strcmp(argv[i], RrArg.c_str()) == 0 || strcmp(argv[i], RrArgShort.c_str()) == 0) {
			if(argc == 3) {
				return return_rows(argv[2]);
			}
			else {
				std::cout << RrArg << " | " << RrArgShort << " : " << RrInfoText << "\n\n" << UsageText << '\n' << argv[0] << ' ' << RrArg << " <" << InVi9pFile << ">" << std::endl;
			}
			return -1;
		}
		else {
			if(argc == 2) {
				std::cout << ErrorText << ' ' << UnknownArgText << '"' << argv[1] << "\"\n" << std::endl;
			}
			break;
		}
	}
	std::cout << UsageText << ' ' << argv[0] << ' ' << ExampleText << '\n' << ArgumentText << '\n' <<
	HArg << " | " << HArgShort << '\n' <<
	NArg << " | " << NArgShort << '\n' <<
	SpArg << " | " << SpArgShort << '\n' <<
	SsArg << " | " << SsArgShort << '\n' <<
	PpArg << " | " << PpArgShort << '\n' <<
	ArArg << " | " << ArArgShort << '\n' <<
	SrArg << " | " << SrArgShort << '\n' <<
	BcArg << " | " << BcArgShort << '\n' <<
	EcArg << " | " << EcArgShort << '\n' <<
	EtArg << " | " << EtArgShort << '\n' <<
	GpArg << " | " << GpArgShort << '\n' <<
	RrArg << " | " << RrArgShort << std::endl;
	return 1;
}

// entry point (real)
#ifndef _WIN32
int main(int argc, char** argv) {
	int ret = progmain(argc, argv);
	return ret;
}
#else

// https://stackoverflow.com/a/41665246
std::string WstrToUtf8Str(const std::wstring& wstr)
{
	std::string retStr = "";
	if (!wstr.empty())
	{
		int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
		
		if (sizeRequired > 0)
		{
			retStr.resize(sizeRequired);
			int bytesConverted = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &retStr[0], sizeRequired, NULL, NULL);
			if (!bytesConverted)
			{
				std::wcout << L"WstrToUtf8Str failed to convert wstring '" << wstr.c_str() << L"'";
				return "";
			}
		}
	}
	return retStr;
}	

int wmain(int argc, wchar_t** argv) {
	// put argv in std::vector (yay!)
	std::vector<std::string> argvecmb;
	{
		int j = 0;
		for(size_t i = 0; i < argc; i++) {
			std::string arg = WstrToUtf8Str(argv[i]);
			argvecmb.push_back(arg);
		}
	}

	// string vector to char**
	char** charcharArray = new char*[argvecmb.size()];

	for (size_t i = 0; i < argvecmb.size(); i++) {
		charcharArray[i] = new char[argvecmb.at(i).length() + 1];
		strcpy_s(charcharArray[i], argvecmb.at(i).length() + 1, argvecmb.at(i).c_str());
	}
	
	int ret = progmain(argc, charcharArray);
	
	//delete everything from the char**
	for (size_t i = 0; i < argvecmb.size(); ++i) {
		delete[] charcharArray[i];
	}
	delete[] charcharArray;
	
	return ret;
}
#endif

