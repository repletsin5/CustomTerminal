#define _CRT_SECURE_NO_WARNINGS
#include <curl/curl.h>
#include <thread>
#include <string>
#include <cstdio>
#include <fstream>
#include <chrono>
#include <thread>
using namespace std;

std::string CurrentDirectory()
{
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");

	return std::string(buffer).substr(0, pos);
}
 bool CheckForFile(const std::string& name) {
	std::ifstream f(name.c_str());
	return f.good();
}

 size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream) {
	 size_t written = fwrite(ptr, size, nmemb, stream);
	 return written;
 }


 int DownloadFiles() {
	 CURL* curl;
	 FILE* fp;
	 CURLcode res;

	 const char* url = "";
	 const char fileName[FILENAME_MAX] = "dl.zip";
	 curl = curl_easy_init();
	 if (curl) {
		 fp = fopen(fileName, "wb");
		 curl_easy_setopt(curl, CURLOPT_URL, url);
		 curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		 curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		 res = curl_easy_perform(curl);
		 curl_easy_cleanup(curl);
	 }
	 return 1;
}
int main() {
	const char* fileName = "TerminalCore.exe";
	if (CheckForFile(fileName)) {
		if (remove(fileName) != 0) {
			perror("Error deleting file");
			system("pause");
			return 1;

		}
	}

	DownloadFiles();
	std::this_thread::sleep_for(100ms);
	//TODO: unzip dl.zip files
	remove("dl.zip");
	return 1;

}