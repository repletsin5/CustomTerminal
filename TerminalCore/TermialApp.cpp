#define _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <filesystem>
#include <iostream>
#include <ConsoleInputHandler.h>
#include <Lmcons.h>
#include "UpdateChecker.h"
#include <codecvt>
#include <locale.h>




std::wstring GetExeFileName()
{
    wchar_t buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    return std::wstring(buffer);
}
std::wstring GetExePath()
{
    std::wstring f = GetExeFileName();

    return f.substr(0, f.find_last_of(L"\\/"));
}

typedef int(*pluginFunc)(void);

std::string getFileExt(const std::string& s) {

    size_t i = s.rfind('.', s.length());
    if (i != std::string::npos) {
        return(s.substr(i + 1, s.length() - i));
    }

    return("");
}



bool EnableVTMode()
{
    // Set output mode to handle virtual terminal sequences
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
    {
        return false;
    }

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode))
    {
        return false;
    }
    return true;
}
void clearConsole(std::vector < std::string > args) {
    system("CLS");
}

void initCommands() {
    CIH::addCommand("clr", { "cls"}, "clears console", clearConsole);
}

int main(int argc, char** argv)
{   

    EnableVTMode();
    system("CLS");

    version curVer("0", "0", "0-alpha");
    cout << "Termial version: " << curVer  << endl;
    
    if (!isUpToDate(curVer)) {
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));
        si.cb = sizeof(STARTUPINFO);
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_SHOW;
        std::wstring appName = GetExePath() + L"\\Updater.exe";

        BOOL bSucces = CreateProcess(NULL, (LPWSTR)appName.c_str(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP, NULL, NULL, &si, &pi);
        if (!bSucces) {
            printf("CreateProcess failed (%d).\n", GetLastError());
        }
        if (pi.hThread)
            CloseHandle(pi.hThread);
        if (pi.hProcess)
            CloseHandle(pi.hProcess);
        exit(1);
    }
    std::wstring name;



    WCHAR username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    GetUserName(username, &username_len);


        

    if (argc > 1) {
        DWORD attributes = GetFileAttributes(CIH::convert(argv[1]).c_str());
        if (attributes == INVALID_FILE_ATTRIBUTES) {
            cout << "Directory not valid\n";
            name = L"C:\\Users\\" + wstring(username);
        }

        if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
            name = CIH::convert(argv[1]).c_str();
        }
        else {
            cout << "Directory not valid\n";
            name = L"C:\\Users\\" + wstring(username);
        }
    }
    else
        name = L"C:\\Users\\" + wstring(username);



    string dir = CIH::utf8_encode(name);
    CIH::curPath = make_shared<std::string*>(&dir);

    string output = CIH::Color::Modifier(FG_GREEN) + CIH::utf8_encode(name) + CIH::Color::Modifier(FG_RED) + ">>" + CIH::Color::Modifier(RESET);
    //you can modify the output string while running but currently only works with initciHandlerOnNewThread
   
    
 
    //args = std::string&
    HINSTANCE proc;
    CIH::initciHandlerOnNewThread(output);
    std::this_thread::sleep_for(16ms); // because help and first command of init commands will init as the first command and help will take priority.
    initCommands();
    //cout << GetExePath();
    std::wstring path = GetExePath() + L"\\plugins";
    try {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {

            if (getFileExt(entry.path().string()) == "dll") {
                proc = LoadLibrary(entry.path().c_str());
                if (!proc) {
                    std::cout << "could not load [" << entry.path() << "]" << std::endl;
                }
                else {
                    pluginFunc funci = (pluginFunc)GetProcAddress(proc, "pluginMain");
                    if (!funci) {
                        std::cout << "could not locate 'pluginMain' " << std::endl;
                    }
                    else {
                        funci();
                    }
                }
            }
        }
        while (true) {
            std::this_thread::sleep_for(100ms); // to lower cpu usage
        }
        FreeLibrary(proc);
    }
    catch (std::exception &e) {
        cout << e.what() << endl;

    }
    catch (...) {
        cout << "unkown Exception\n";
    }
}

