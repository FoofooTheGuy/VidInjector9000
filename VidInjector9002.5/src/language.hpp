#pragma once

#include "widgets.hpp"

class Languages {
	public:
	struct Language {
		std::string Dir;
		std::string File;
	};
	static std::vector<Language> LanguageFiles;
};

bool loadLanguage(std::string LangPath);//generated by the helper thing
void applyLanguage(InitWidgets* wid, VI9Pparameters* parameters);
