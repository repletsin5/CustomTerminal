#include <memory>
#include "ConsoleInputHandler.h"
#include <ostream>
#include <iostream>


std::shared_ptr < std::vector<commandStruct>> CIH::commands = make_shared<std::vector<commandStruct>>(0);
std::shared_ptr<std::string*> CIH::outputStringPtr = make_shared<std::string*>();
std::shared_ptr<std::string*> CIH::curPath = make_shared<std::string*>();
std::string CIH::inputText;
std::string CIH::currentCommand;

int CIH::initciHandlerOnNewThread(const string& output) {

	pthread_t thread;
	int ret;
	ret = pthread_create(&thread, NULL, initciHandler, (void*)&output);
	if (ret != 0) {
		printf("pthread_create() failed\n");
		exit(0);
	}
	return ret;
}
void* CIH::initciHandler(void* output) {

	CIH::commands.get()->clear();
	CIH::addCommand("help", {}, "", helpCommand);
	CIH::outputStringPtr = make_shared<std::string*>(static_cast<std::string*>(output));
	string outputString = *(static_cast<string*>(static_cast<std::string*>(output)));

	cout <<outputString;
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
		cout <<  outputString ;
	}
}



void CIH::initciHandler(const string& output) {
	CIH::outputStringPtr = make_shared<std::string*>(const_cast<std::string*> (&output));
	CIH::commands.get()->clear();
	CIH::addCommand("help", { "" }, "", helpCommand);
	while (true)
	{
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
		temp = CIH::currentCommand + " ";
		CIH::inputText.erase(CIH::inputText.begin(), CIH::inputText.begin() + (temp.size() - 1));
		temp = "";
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
		auto args = CIH::inputText;
		vector<string>argsArray;
		temp = "";
		for (size_t i = 0; i < args.length(); i++) {
			char c = args[i];
			if (c == ' ') {
				argsArray.push_back(temp);
				temp = "";
			}
			else if (c == '\"') {
				i++;
				while (args[i] != '\"') { temp += args[i]; i++; }
			}
			else {
				temp += c;
			}
		}
		handleCommands(CIH::currentCommand, argsArray, *CIH::commands.get());

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