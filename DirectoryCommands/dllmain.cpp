// dllmain.cpp : Defines the entry point for the DLL application.
#include <ConsoleInputHandler.h>
void launchExecutable(string file, string args) {
	string temp = file + " " + args;
	system(temp.c_str());

}

void launchExplorer(std::vector<std::string> args) {

	string explorer = "C:\\Windows\\explorer.exe";
	if (args[0] == ".") {
		launchExecutable(explorer, **(CIH::curPath.get()));
	}
	else {
		launchExecutable(explorer, args[0]);
	}


}

extern "C" int __declspec(dllexport) pluginMain() {

	CIH::addCommand("explorer", {}, "opens explorer",launchExplorer);
	return 1;
}