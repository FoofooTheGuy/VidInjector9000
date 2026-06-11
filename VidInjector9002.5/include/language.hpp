#pragma once

#include <inLang.hpp>

class Languages {
	public:
		struct Language {
			std::string Dir;
			std::string File;
		};
		static std::vector<Language> LanguageFiles;
};

bool loadLanguage(std::string LangPath);
