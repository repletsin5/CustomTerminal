#pragma once
#include <chrono>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <assert.h> 
#include <iterator>
#include <future>
#include <pthread.h>
#include "export.h"
using namespace std;
enum Code {
	RESET = 0,
	FG_RED = 31,
	FG_GREEN = 32,
	FG_BLUE = 34,
	FG_DEFAULT = 39,
	BG_RED = 41,
	BG_GREEN = 42, 
	BG_BLUE = 44,
	BG_DEFAULT = 49
};


TERMINAL_API extern struct commandStruct
{
	string commandName;
	vector<string> aliases;
	string description;
	void (*callback)(vector<string>);
};
namespace CIH::Color {

	class Modifier {
		Code code;
	public:
		Modifier(Code pCode) : code(pCode) { colour = string("\33[") + std::to_string(pCode) + string("m"); }
		std::string colour;
		friend std::ostream&
			 operator<<(std::ostream& os, const Modifier& mod) {
			return os << mod.colour;
		}
		friend std::string  operator+(std::string tmp , const Modifier& mod) {
			
			return tmp + mod.colour;
		}
		friend std::string  operator+(const char* tmp, const Modifier& mod) {

			return  string(tmp) + mod.colour;
		}
		friend std::string  operator+(char* tmp, const Modifier& mod) {

			return string(tmp) + mod.colour;
		}
		friend std::string  operator+(char tmp, const Modifier& mod) {
			char temp[2];
			temp[0] = tmp;
			temp[1] = '\0';

			return string(temp) + mod.colour;
		}
		friend std::string  operator+(const Modifier mod, std::string tmp) {

			return mod.colour + tmp;
		}
		friend std::string  operator+(const Modifier& mod, const char* tmp) {

			return mod.colour + string(tmp);
		}
		friend std::string  operator+(const Modifier& mod, char* tmp) {

			return mod.colour + string(tmp);
		}
		friend std::string  operator+(const Modifier& mod, char tmp) {
			char temp[2];
			temp[0] = tmp;
			temp[1] = '\0';

			return mod.colour + string(temp);
		}
	};
}

namespace CIH
{	
	TERMINAL_API extern void* initciHandler(void*);
	TERMINAL_API extern int initciHandlerOnNewThread();
	TERMINAL_API extern bool setWindowAlpha(BYTE aplha);
	TERMINAL_API extern std::wstring convert(const std::string& as);
	TERMINAL_API extern std::string utf8_encode(const std::wstring& wstr);
	TERMINAL_API extern std::shared_ptr < std::vector<commandStruct>> commands;
	TERMINAL_API void addCommand(string cmd, vector<string> aliases, string description, void (*callback)(vector<string>));
	TERMINAL_API extern std::shared_ptr<std::string> outputStringPtr;
	TERMINAL_API extern std::shared_ptr<std::string> curPath;
	TERMINAL_API extern std::shared_ptr<std::string> suffex;
	TERMINAL_API extern std::string inputText;
	TERMINAL_API extern std::string currentCommand;
	TERMINAL_API extern HWND GetConsoleHwnd();

	extern bool replace(std::string& str, const std::string& from, const std::string& to);
	extern void handleCommands(string command, vector<string>args, const std::vector<commandStruct>& commands);
	extern void helpCommand(vector<string> args);

};

