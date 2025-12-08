#pragma once

#include "widgets.hpp"

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
void applyLanguage(InitWidgets* wid, VI9Pparameters* parameters);
