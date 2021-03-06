// runapp.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <stdint.h>
#define DST_PREFIX "C:\\dst\\"     //"C:\\"
#define SRC_PREFIX "C:\\src\\"     //"D:\\"
#define SRC_PATTERN "C:\\src\\*"     //"D:\\*"
#define CONFIG_FileName "C:\\dst\\RunCommand.txt"    //"C:\\RunCommand.txt"
char** read_config(char* config_path) {
	char* *ptr2=(char**)malloc(sizeof(int*)*2);
	memset(ptr2, 0, sizeof(int*) * 2);

	FILE* config_file = 0;
	fopen_s(&config_file, config_path, "rb");
	if (!config_file) {
		return ptr2;//file open error
	}
	fseek(config_file, 0, SEEK_END);
	long size=ftell(config_file);
	char *buffer = (char*)malloc(size+1);
	memset(buffer, 0, size + 1);
	fseek(config_file, 0, SEEK_SET);

	size_t ret_size=fread(buffer, 1, size, config_file);
	if (ret_size != size) {
		fclose(config_file);
		free(buffer);
		return ptr2;//if read error
	}

	for (int i = 0; i < size; i++) {
		if (buffer[i] == '\xd' && buffer[i + 1] == '\xa') {
			buffer[i] = 0;
			buffer[i + 1] = 0;
			ptr2[0] = buffer;
			ptr2[1] = buffer + 2+i;
			return ptr2;//return success
		}
	}

	ptr2[0] = buffer;
	return ptr2;// half success
}
bool copy(WCHAR* src, WCHAR* dst) {
	WCHAR dest_path[1024];
	WCHAR src_path[1024];
	memset(dest_path, 0, sizeof(dest_path));
	memset(src_path, 0, sizeof(src_path));
	wcscat_s(dest_path, _T(DST_PREFIX));
	wcscat_s(src_path, _T(SRC_PREFIX));
	wcscat_s(dest_path, dst);
	wcscat_s(src_path, src);
	bool ret_code = CopyFile(src_path, dest_path, FALSE);
	return ret_code;
}
void copy_file() {
	WIN32_FIND_DATA findFileData;
	HANDLE hFind;
	hFind = FindFirstFile(_T(SRC_PATTERN),&findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return;
	}
	else
	{
		copy(findFileData.cFileName, findFileData.cFileName);
		while (FindNextFile(hFind, &findFileData) == TRUE)
		{
			copy(findFileData.cFileName, findFileData.cFileName);
		}
		FindClose(hFind);
	}
}
void runcmd(char** ptr2) {
	char cmd_buffer[1024] = { 0 };
	strcat_s(cmd_buffer, DST_PREFIX);
	strcat_s(cmd_buffer, ptr2[0]);
	if (ptr2[1]) {
		strcat_s(cmd_buffer, " ");
		strcat_s(cmd_buffer, ptr2[1]);
	}
	STARTUPINFOA startup_info;
	memset(&startup_info, 0, sizeof(startup_info));
	startup_info.cb = sizeof(startup_info);
	startup_info.dwFlags = STARTF_USESHOWWINDOW;
	startup_info.wShowWindow = 1;
	PROCESS_INFORMATION process_information;
	CreateProcessA(0, (LPSTR)cmd_buffer, 0, 0, 0, 0, 0, 0, &startup_info, &process_information);

}
int main()
{
	copy_file();
	/*char config_path[1024] = { 0 };
	strcat_s(config_path, DST_PREFIX);
	strcat_s(config_path, CONFIG_FileName);*/
	char** ptr2 = read_config(CONFIG_FileName);
	if (!ptr2[0]) {
		printf("config format error!");
		return 0;
	}
	else {
		runcmd(ptr2);
		free(ptr2[0]);
	}
	free(ptr2);
    return 0;
}

