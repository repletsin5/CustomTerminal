// dllmain.cpp : Defines the entry point for the DLL application.
#include <ConsoleInputHandler.h>
#include <json.hpp>
#include <fstream>
#include <map>
#include <vector>
#include <any>

std::map<std::string, std::vector<std::any>> jsonMap;



void launchExecutable(string file, string args) {
	string temp = "\"" + file + "\"" + " " + args;
	system(temp.c_str());

}

void launchExplorer(std::vector<string> args) {
	string explorer = "C:\\Windows\\explorer.exe";
	if (args[0] == ".") {
		launchExecutable(explorer, **(CIH::curPath.get()));
	}
	else {
		launchExecutable(explorer, args[0]);
	}


}

void launchNoArgs(std::vector<string> args) {
	std::vector<std::any> temp = jsonMap[CIH::currentCommand];
	launchExecutable(std::any_cast<std::string>(temp[0]), "");
}
void launchWithArgs(std::vector<string> args) {
	std::vector<std::any> temp = jsonMap[CIH::currentCommand];
	if (args[0] == ".") {
		launchExecutable(std::any_cast<std::string>(temp[0]), **(CIH::curPath.get()));
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

void cdCommand(std::vector<string> args) {
	/*
	* TODO:
	* not implemented
	* Idea:
	* parse ../ and ./ 
	* recognise root specifier
	*/
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
	//CIH::addCommand("cd", {}, "opens explorer",launchExplorer); //Not implemented yet
	return 1;
}