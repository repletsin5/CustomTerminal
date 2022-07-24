
#include <Windows.h>
#include <memory>
#include "ConsoleInputHandler.h"
#include <ostream>
#include <iostream>
#include <codecvt>
#include <Lmcons.h>



std::shared_ptr < std::vector<commandStruct>> CIH::commands = make_shared<std::vector<commandStruct>>(0);
std::shared_ptr<std::string> CIH::outputStringPtr = make_shared<std::string>();
std::shared_ptr<std::string> CIH::curPath = make_shared<std::string>();
std::shared_ptr<std::string> CIH::suffex = make_shared<std::string>();
std::string CIH::inputText;
std::string CIH::currentCommand;

int CIH::initciHandlerOnNewThread() {

	pthread_t thread;
	int ret;
	ret = pthread_create(&thread, NULL, initciHandler,(void*)0);
	if (ret != 0) {
		printf("pthread_create() failed\n");
		exit(0);
	}
	return ret;
}


//code below from stackoverflow
inline std::wstring CIH::convert(const std::string& as)
{
	wchar_t* buf = new wchar_t[as.size() * 2 + 2];
	std::swprintf(buf, L"%S", as.c_str());
	std::wstring rval = buf;
	delete[] buf;
	return rval;
}
//code below from stackoverflow
std::string CIH::utf8_encode(const std::wstring& wstr)
{
	if (wstr.empty()) return std::string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

bool CIH::setWindowAlpha(BYTE aplha) {
	return SetLayeredWindowAttributes(
		GetConsoleHwnd(),
		NULL,
		aplha,
		LWA_ALPHA
	);
}
HWND CIH::GetConsoleHwnd()
{
#define MY_BUFSIZE 1024 // Buffer size for console window titles.
	HWND hwndFound;         // This is what is returned to the caller.
	wchar_t pszNewWindowTitle[MY_BUFSIZE]; // Contains fabricated
										// WindowTitle.
	wchar_t pszOldWindowTitle[MY_BUFSIZE]; // Contains original
										// WindowTitle.

	// Fetch current window title.

	GetConsoleTitle((LPWSTR)pszOldWindowTitle, MY_BUFSIZE);

	// Format a "unique" NewWindowTitle.

	wsprintf((LPWSTR)pszOldWindowTitle, L"%d/%d",
		GetTickCount(),
		GetCurrentProcessId());

	// Change current window title.

	SetConsoleTitle(pszNewWindowTitle);

	// Ensure window title has been updated.

	Sleep(40);

	// Look for NewWindowTitle.

	hwndFound = FindWindow(NULL, pszNewWindowTitle);

	// Restore original window title.

	SetConsoleTitle(pszOldWindowTitle);

	return(hwndFound);
}

void* CIH::initciHandler(void*) {

	CIH::commands.get()->clear();
	CIH::addCommand("help", {}, "", helpCommand);
	setWindowAlpha(255);
	cout << CIH::Color::Modifier(FG_GREEN) + *CIH::curPath.get() + *CIH::suffex.get();
	while (true)
	{
		std::this_thread::sleep_for(16ms);

		CIH::currentCommand = "";

		getline(cin, CIH::inputText);
		for (auto c : CIH::inputText) {
			if (c != ' ')
				CIH::currentCommand += c;
			else
				if (CIH::currentCommand.size() > 0)
					break;
		}

		string temp;
		int toRemove = 0;

		for (auto c : CIH::currentCommand) {
			if (c == ' ')
				toRemove++;
			else
				break;
		}
		if (toRemove > 0) {
			CIH::currentCommand.erase(CIH::inputText.begin(), CIH::inputText.begin() + (toRemove - 1));
		}
		temp = CIH::currentCommand + " ";
		CIH::inputText.erase(CIH::inputText.begin(), CIH::inputText.begin() + (temp.size() - 1));
		temp = "";
		auto args = CIH::inputText;
		vector<string>argsArray;

		temp = "";
		for (size_t i = 0; i < args.length(); i++) {
			char c = args[i];
			if (c == ' ') {
				if (temp.size() > 0) {
					argsArray.push_back(temp);
					temp = "";
				}
			}
			else if (c == '\"') {
				i++;
				while (args[i] != '\"') { temp += args[i]; i++; }
			}
			else {
				temp += c;
			}

		}
		argsArray.push_back(temp);
		handleCommands(CIH::currentCommand, argsArray, *CIH::commands.get());
		cout << CIH::Color::Modifier(FG_GREEN) + *CIH::curPath.get() + *CIH::suffex.get();
	}
}




void CIH::handleCommands(string command, vector<string>args, const std::vector<commandStruct>& commands) {
	for (auto c : commands) {
		if (c.commandName == command) {
			c.callback(args);
			return;
		}
		else if (command.empty())
			return;
		else
			for (auto b : c.aliases) {
				if (b == command && !(b == string(""))) {
					c.callback(args);
					return;
				}
			}
	}
	cout << "Unkown command '" << command << "'. Please type 'help' for list of commands\n";
	return;
}

bool CIH::replace(std::string& str, const std::string& from, const std::string& to)
{
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

void CIH::addCommand(string cmd, vector<string> aliases, string description, void (*callback)(vector<string>)) {
	
	if (!(CIH::commands.get()->empty())) {
		for (auto& c : *CIH::commands.get()) {
			string temp = "";
			if (cmd == "help") {
				cout << "Cannnot overrite help command" << endl;
				assert(!(c.commandName == "help"));
			}
			else if (c.commandName == cmd) {
				c.aliases = aliases;
				c.callback = callback;
				c.description = description;
				break;
			}
			else {
				for (auto b : c.aliases) {
					for (auto i : aliases)
					{
						if (i == b && !(b == "")) {
							cout << "'" << cmd << "' " << "Aliase matches existing alias on command '" << c.commandName << "'" << endl;
							assert(!(i == b));
							exit(0);
						}
					}
				}
				commandStruct command;
				command.commandName = cmd;
				command.aliases = aliases;
				command.description = description;
				command.callback = callback;
				CIH::commands.get()->push_back(command);
				return;

			}

		}

	}
	else {
		commandStruct command;
		command.commandName = cmd;
		command.aliases = aliases;
		command.description = description;
		command.callback = callback;
		CIH::commands.get()->push_back(command);
		return;
	}

}

void CIH::helpCommand(vector<string> args) {
	if (!args.empty() && !(args[0] == "")) {
		for (auto c : args) {
			bool foundCmd = false;
			for (auto b : *CIH::commands.get()) {
				if (c == b.commandName && !(c == "help")) {
					foundCmd = true;
					if (!(b.description.length() == 0))
						cout << b.commandName << "\n  " << b.description << endl;
					if (!(b.aliases.size() == 0)) {
						cout << "aliases" << endl;
						for (auto d : b.aliases) {
							cout << "  " << d << endl;
						}
					}
				}
			}
			if (!foundCmd)
				cout << "Cant find '" << c << "' command" << endl;
		}
		return;
	}
	else {
		for (auto c : *CIH::commands.get()) {
			if (!(c.commandName == "help")) {
				if (!(c.description.length() == 0))
					cout << c.commandName << "\n  " << c.description << endl;
				if (!(c.aliases.size() == 0)) {
					cout << "aliases" << endl;
					for (auto b : c.aliases) {
						cout << "  " << b << endl;
					}
				}
				cout << endl;
			}
		}
	}
	return;
}