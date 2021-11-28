#define _CRT_SECURE_NO_WARNINGS
#include <curl/curl.h>
#include <thread>
#include <string>
#include <cstdio>
#include <fstream>

#include <ShlDisp.h>
#include <chrono>
#include <thread>
using namespace std;
bool Unzip2Folder(BSTR lpZipFile, BSTR lpFolder)
{
	IShellDispatch* pISD;

	Folder* pZippedFile = 0L;
	Folder* pDestination = 0L;

	long FilesCount = 0;
	IDispatch* pItem = 0L;
	FolderItems* pFilesInside = 0L;

	VARIANT Options, OutFolder, InZipFile, Item;
	CoInitialize(NULL);
	__try {
		if (CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER, IID_IShellDispatch, (void**)&pISD) != S_OK)
			return 1;

		InZipFile.vt = VT_BSTR;
		InZipFile.bstrVal = lpZipFile;
		pISD->NameSpace(InZipFile, &pZippedFile);
		if (!pZippedFile)
		{
			pISD->Release();
			return 1;
		}

		OutFolder.vt = VT_BSTR;
		OutFolder.bstrVal = lpFolder;
		pISD->NameSpace(OutFolder, &pDestination);
		if (!pDestination)
		{
			pZippedFile->Release();
			pISD->Release();
			return 1;
		}

		pZippedFile->Items(&pFilesInside);
		if (!pFilesInside)
		{
			pDestination->Release();
			pZippedFile->Release();
			pISD->Release();
			return 1;
		}

		pFilesInside->get_Count(&FilesCount);
		if (FilesCount < 1)
		{
			pFilesInside->Release();
			pDestination->Release();
			pZippedFile->Release();
			pISD->Release();
			return 0;
		}

		pFilesInside->QueryInterface(IID_IDispatch, (void**)&pItem);

		Item.vt = VT_DISPATCH;
		Item.pdispVal = pItem;

		Options.vt = VT_I4;
		Options.lVal = 1024 | 512 | 16 | 4;//http://msdn.microsoft.com/en-us/library/bb787866(VS.85).aspx

		bool retval = pDestination->CopyHere(Item, Options) == S_OK;

		pItem->Release(); pItem = 0L;
		pFilesInside->Release(); pFilesInside = 0L;
		pDestination->Release(); pDestination = 0L;
		pZippedFile->Release(); pZippedFile = 0L;
		pISD->Release(); pISD = 0L;

		return retval;

	}
	__finally
	{
		CoUninitialize();
	}
}

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
	 const char* url = "https://drive.google.com/file/d/11t3lfy6ut7lv4syr8gZRL4ptQ5eyQ9Ud/view?usp=sharing";
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
	Unzip2Folder((BSTR)L"dl.zip", (BSTR)L"./");
	remove("dl.zip");
	return 1;

}