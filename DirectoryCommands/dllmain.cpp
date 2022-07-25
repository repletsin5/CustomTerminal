// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <tchar.h>
#include <ConsoleInputHandler.h>
#include <json.hpp>
#include <fstream>
#include <map>
#include <vector>
#include <any>
#include <filesystem>
string temp1;
std::map<std::string, std::vector<std::any>> jsonMap;



void launchExecutable(string file, string args) {
	string temp = "\"" + file + "\"" + " " + args;
	system(temp.c_str());

}


void launchNoArgs(std::vector<string> args) {
	std::vector<std::any> temp = jsonMap[CIH::currentCommand];
	launchExecutable(std::any_cast<std::string>(temp[0]), "");
}
void launchWithArgs(std::vector<string> args) {
	std::vector<std::any> temp = jsonMap[CIH::currentCommand];
	if (args[0] == ".") {
		launchExecutable(std::any_cast<std::string>(temp[0]), *(CIH::curPath.get()));
	}
	else {
		launchExecutable(std::any_cast<std::string>(temp[0]), args[0]);
	}
}


void initJSONcommands(const char* cmd,bool args,const char* desc) {
	if (args) {
		CIH::addCommand(cmd, {}, desc, launchWithArgs);
	}
	else {
		CIH::addCommand(cmd, {}, desc, launchNoArgs);
	}
}

inline bool ends_with(std::string const& value, std::string const& ending)
{
	if (ending.size() > value.size()) return false;
	return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}
void cdCommand(std::vector<string> args) {
	/*
	* TODO:
	* not implemented
	* Idea:
	* parse ../ and ./ 
	* recognise root specifier
	*/
	filesystem::path temp;
	if ((0 < args[0].length()) || !(args[0] == "")) {

		
		temp.assign(args[0]);
		if (!temp.has_root_path()) {

			if (args[0]._Starts_with("../")|| args[0]._Starts_with("..\\") || args[0]._Starts_with("..")) {
				if (std::filesystem::is_directory(*CIH::curPath.get() + "\\" + args[0])) {
					
					//**CIH::curPath.get() = **CIH::curPath.get() + "\\" + args[0];
					temp1 = filesystem::absolute(*CIH::curPath.get() + "\\" + args[0]).lexically_normal().string();
					if (ends_with(temp1, "\\")) {
						temp1 = temp1.substr(0, temp1.size() - 1);
					}
					CIH::curPath = make_shared<std::string>(temp1);
				
				}
				else {
					cout << "Path does not exsist\n";
				}
			}
			else if (args[0]._Starts_with("./") || args[0]._Starts_with(".\\")) {
				temp1 =args[0];
				temp1 = temp1.erase(0, 1);
				temp1 = *CIH::curPath.get() + temp1;
				if (std::filesystem::is_directory(temp1)){
					CIH::curPath = make_shared<std::string>(filesystem::absolute(temp1).lexically_normal().string());
				}
				else {
					cout << "Path does not exsist\n";
				}
			}
			else {
				if (std::filesystem::is_directory(*CIH::curPath.get() + "\\" + args[0])) {
					CIH::curPath = make_shared<std::string>(filesystem::absolute(*CIH::curPath.get() + "\\" + args[0]).lexically_normal().string());
				}
				else {
					cout << "Path does not exsist\n";
				}
			}
		}
		else {
			if (std::filesystem::is_directory(args[0])) {

				//**CIH::curPath.get() = **CIH::curPath.get() + "\\" + args[0];
				auto temp2 = filesystem::absolute(args[0]).lexically_normal().string();
				if (ends_with(temp2, "\\")) {
					temp2 = temp2.substr(0, temp2.size() - 1);
				}
				CIH::curPath = make_shared<std::string>(temp2);

			}
			else {
				cout << "Path does not exsist\n";
			}
		}
	}

}


//code below is from stackoverflow
int SubdirCount(const TCHAR* parent_path) {
	// The hideous string manipulation code below
	// prepares a TCHAR wildcard string (sub_wild)
	// matching any subdirectory immediately under 
	// parent_path by appending "\*"
	size_t len = _tcslen(parent_path);
	const size_t alloc_len = len + 3;
	TCHAR* sub_wild = new TCHAR[alloc_len];
	_tcscpy_s(sub_wild, alloc_len, parent_path);
	if (len && sub_wild[len - 1] != _T('\\')) { sub_wild[len++] = _T('\\'); }
	sub_wild[len++] = _T('*');
	sub_wild[len++] = _T('\0');

	// File enumeration starts here
	WIN32_FIND_DATA fd;
	HANDLE hfind;
	int count = 0;
	if (INVALID_HANDLE_VALUE != (hfind = FindFirstFile(sub_wild, &fd))) {
		do {
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				// is_alias_dir is true if directory name is "." or ".."
				const bool is_alias_dir = fd.cFileName[0] == _T('.') &&
					(!fd.cFileName[1] || (fd.cFileName[1] == _T('.') &&
						!fd.cFileName[2]));

				count += !is_alias_dir;
			}
		} while (FindNextFile(hfind, &fd));
		FindClose(hfind);
	}

	delete[] sub_wild;
	return count;
}

void lsCommand(std::vector<string> args) {
	//probably a bad way to implement this 

	auto dirAndFileCount = std::distance(std::filesystem::directory_iterator(*CIH::curPath.get()), std::filesystem::directory_iterator{});
	std::map<const char*, bool>::iterator it;
	std::map<const char*, bool> allArgsUsed = { { "-f",false},{"-d",false},{"-l",false} };
	if ((0 < args[0].length()) || !(args[0] == "")) {
		bool printed = false;
		for (auto b : args) {
			cout << b << endl;
			cout <<"val = " << allArgsUsed[b.c_str()] << endl;
			it = allArgsUsed.find(b.c_str());
			if (it != allArgsUsed.end())
				allArgsUsed[b.c_str()] = true;

			cout << "val = " << allArgsUsed[b.c_str()] << endl;
		

			for (auto& a : std::filesystem::directory_iterator(*CIH::curPath.get())) {


				DWORD attributes = GetFileAttributes(a.path().c_str());
				if (allArgsUsed["-f"] == true && allArgsUsed["-d"] == true) {
					DWORD attributes = GetFileAttributes(a.path().c_str());
					if (allArgsUsed["-l"] == true && printed == false) {
						printed = true;
						cout << dirAndFileCount << endl;
					}
					if (a.is_directory()) {

						if (attributes & FILE_ATTRIBUTE_HIDDEN)
							cout << CIH::Color::Modifier(BG_GREEN) << a.path().filename().string() << CIH::Color::Modifier(RESET) << " [Hidden]" << endl;
						else
							cout << CIH::Color::Modifier(BG_GREEN) << a.path().filename().string() << CIH::Color::Modifier(RESET) << endl;
					}
					else {
						if (attributes & FILE_ATTRIBUTE_HIDDEN)
							cout << a.path().filename().string() << " [Hidden]" << endl;
						else
							cout << a.path().filename().string() << endl;
					}
				}
				else if (!a.is_directory() == true && allArgsUsed["-f"] == true && allArgsUsed["-d"] == false) {
					if (allArgsUsed["-l"] == true && printed == false) {
						printed = true;
						auto fileCount = dirAndFileCount - SubdirCount(CIH::convert(**CIH::curPath.get()).c_str());
						auto fileCount = dirAndFileCount - SubdirCount(CIH::convert(*CIH::curPath.get()).c_str());
						cout << fileCount << endl;
					}
					if (attributes & FILE_ATTRIBUTE_HIDDEN)
						cout << a.path().filename().string() << " [Hidden]" << endl;
					else
						cout << a.path().filename().string() << endl;
				}
				else if (a.is_directory() == true && allArgsUsed["-d"] == true && allArgsUsed["-f"] == false) {
					if (allArgsUsed["-l"] == true && printed == false) {
						printed = true;
						cout << SubdirCount(CIH::convert(*CIH::curPath.get()).c_str()) << endl;
					}
					if (attributes & FILE_ATTRIBUTE_HIDDEN)
						cout << a.path().filename().string() << " [Hidden]" << endl;
					else
						cout << a.path().filename().string() << endl;

				}
				else if (allArgsUsed["-l"] == true && printed == false) {
					printed = true;
					cout << dirAndFileCount << endl;
				}


			}

		}
	}
	else {
		for (auto& a : std::filesystem::directory_iterator(*CIH::curPath.get())) {
			DWORD attributes = GetFileAttributes(a.path().c_str());
			if (a.is_directory()) {
				if (attributes & FILE_ATTRIBUTE_HIDDEN)
					cout << CIH::Color::Modifier(BG_GREEN) << a.path().filename().string() << CIH::Color::Modifier(RESET) << " [Hidden]" << endl;
				else
					cout << CIH::Color::Modifier(BG_GREEN) << a.path().filename().string() << CIH::Color::Modifier(RESET) << endl;
			}
			else {
				if (attributes & FILE_ATTRIBUTE_HIDDEN)
					cout << a.path().filename().string() << " [Hidden]" << endl;
				else
					cout << a.path().filename().string() << endl;
			}
		}
	}


}

extern "C" int __declspec(dllexport) pluginMain() {
	std::ifstream jsonFile;
	jsonFile.open("path.json");
	try {
		json root = json::parse(jsonFile);
		for (auto it = root.begin(); it != root.end(); ++it) {

			if (it.value().is_object()) {
				for (auto a = it.value().begin(); a != it.value().end(); ++a) {
					jsonMap[it.key()] = {
						it.value().find("path").value().get<std::string>() ,
						it.value().find("args").value().get<bool>(),
						it.value().find("description").value().get<std::string>(),
					};

				}
				initJSONcommands(it.key().c_str(), it.value().find("args").value().get<bool>(), it.value().find("description").value().get<std::string>().c_str());
			}


		}

	}
	catch (json::exception& e) {
		std::cout << e.what() << std::endl;
	}
	std::vector<std::any> temp = jsonMap[CIH::currentCommand];
	for (auto& hmm:  temp) {
		std::cout << std::any_cast<std::string>(hmm) << endl;
	}
	CIH::addCommand("cd", {}, "changes current directory",cdCommand); //Not implemented yet
	CIH::addCommand("ls", {}, "list files and folders in current directory",lsCommand);
	return 1;
}