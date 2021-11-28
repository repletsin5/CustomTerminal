//BSD 3-Clause License
//Copyright(c) 2021, the SingleHeaderLibs developers
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
		Modifier(Code pCode) : code(pCode) { color = string("\33[") + std::to_string(pCode) + string("m"); }
		std::string color;
		friend std::ostream&
			 operator<<(std::ostream& os, const Modifier& mod) {
			return os << mod.color;
		}
		friend std::string  operator+(std::string tmp , const Modifier& mod) {
			
			return tmp + mod.color;
		}
		friend std::string  operator+(const char* tmp, const Modifier& mod) {

			return  string(tmp) + mod.color;
		}
		friend std::string  operator+(char* tmp, const Modifier& mod) {

			return string(tmp) + mod.color;
		}
		friend std::string  operator+(char tmp, const Modifier& mod) {
			char temp[2];
			temp[0] = tmp;
			temp[1] = '\0';

			return string(temp) + mod.color;
		}
		friend std::string  operator+(const Modifier mod, std::string tmp) {

			return mod.color + tmp;
		}
		friend std::string  operator+(const Modifier& mod, const char* tmp) {

			return mod.color + string(tmp);
		}
		friend std::string  operator+(const Modifier& mod, char* tmp) {

			return mod.color + string(tmp);
		}
		friend std::string  operator+(const Modifier& mod, char tmp) {
			char temp[2];
			temp[0] = tmp;
			temp[1] = '\0';

			return mod.color + string(temp);
		}
	};
}

namespace CIH
{
	TERMINAL_API extern void initciHandler(const string& output);
	TERMINAL_API extern void* initciHandler(void* output);
	TERMINAL_API extern int initciHandlerOnNewThread(const string& output);
	TERMINAL_API extern std::shared_ptr < std::vector<commandStruct>> commands;
	TERMINAL_API void addCommand(string cmd, vector<string> aliases, string description, void (*callback)(vector<string>));
	TERMINAL_API extern std::shared_ptr<std::string*> outputStringPtr;
	TERMINAL_API extern std::shared_ptr<std::string*> curPath;
	TERMINAL_API extern std::string inputText;
	TERMINAL_API extern std::string currentCommand;

	extern bool replace(std::string& str, const std::string& from, const std::string& to);
	extern void handleCommands(string command, vector<string>args, const std::vector<commandStruct>& commands);
	extern void helpCommand(vector<string> args);

};

