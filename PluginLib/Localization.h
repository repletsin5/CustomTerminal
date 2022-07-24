#pragma once
#include <fstream>
#include <string>
class LocalizedString {


	inline static std::string currentLang = "en_GB";
	static std::string getLocalizedString(std::string key);

	static void init() {
		//inipp::Ini<char> ini;
		//std::ifstream is("settings.ini");
		//ini.strip_trailing_comments();
		//ini.default_section(ini.sections["Main"]);
		//ini.interpolate();	
		//inipp::get_value(ini.sections["Main"], "Language", currentLang);
	}
	static void reloadLanguage(){

	}
	static void setLanguageFile() {

	}
	static void getLanguage() {

	}
};