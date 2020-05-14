#include "FileSystemUtils.h"

#include <vector>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iterator>
#include <algorithm>
#include <iostream>

#include <SDL.h>
#include <physfs.h>

#include "tinyxml.h"

#if defined(_WIN32)
#include <windows.h>
#include <shlobj.h>
#include <shellapi.h>
int mkdir(char* path, int mode)
{
	WCHAR utf16_path[MAX_PATH];
	MultiByteToWideChar(CP_UTF8, 0, path, -1, utf16_path, MAX_PATH);
	return CreateDirectoryW(utf16_path, NULL);
}
#define VNEEDS_MIGRATION (mkdirResult != 0)
#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__HAIKU__) || defined(__DragonFly__)
#include <sys/stat.h>
#include <limits.h>
#define VNEEDS_MIGRATION (mkdirResult == 0)
/* These are needed for PLATFORM_* crap */
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <spawn.h>
#define MAX_PATH PATH_MAX
#endif

char saveDir[MAX_PATH];
char levelDir[MAX_PATH];

void PLATFORM_getOSDirectory(char* output);
void PLATFORM_migrateSaveData(char* output);
void PLATFORM_copyFile(const char *oldLocation, const char *newLocation);

int FILESYSTEM_init(char *argvZero, char* baseDir, char *assetsPath)
{
	char output[MAX_PATH];
	int mkdirResult;
	const char* pathSep = PHYSFS_getDirSeparator();

	PHYSFS_init(argvZero);
	PHYSFS_permitSymbolicLinks(1);

	/* Determine the OS user directory */
	if (baseDir && strlen(baseDir) > 0)
	{
		strcpy(output, baseDir);

		/* We later append to this path and assume it ends in a slash */
		if (strcmp(std::string(1, output[strlen(output) - 1]).c_str(), pathSep) != 0)
		{
			strcat(output, pathSep);
		}
	}
	else
	{
		PLATFORM_getOSDirectory(output);
	}

	/* Create base user directory, mount */
	mkdirResult = mkdir(output, 0777);

	/* Mount our base user directory */
	PHYSFS_mount(output, NULL, 1);
	PHYSFS_setWriteDir(output);
	printf("Base directory: %s\n", output);

	/* Create save directory */
	strcpy(saveDir, output);
	strcat(saveDir, "saves");
	strcat(saveDir, PHYSFS_getDirSeparator());
	mkdir(saveDir, 0777);
	printf("Save directory: %s\n", saveDir);

	/* Create level directory */
	strcpy(levelDir, output);
	strcat(levelDir, "levels");
	strcat(levelDir, PHYSFS_getDirSeparator());
	mkdirResult |= mkdir(levelDir, 0777);
	printf("Level directory: %s\n", levelDir);

	/* We didn't exist until now, migrate files! */
	if (VNEEDS_MIGRATION)
	{
		PLATFORM_migrateSaveData(output);
	}

	/* Mount the stock content last */
	if (assetsPath) {
		strcpy(output, assetsPath);
	} else {
		strcpy(output, PHYSFS_getBaseDir());
		strcat(output, "data.zip");
	}
	if (!PHYSFS_mount(output, NULL, 1))
	{
		puts("Error: data.zip missing!");
		puts("You do not have data.zip!");
		puts("Grab it from your purchased copy of the game,");
		puts("or get it from the free Make and Play Edition.");

		SDL_ShowSimpleMessageBox(
			SDL_MESSAGEBOX_ERROR,
			"data.zip missing!",
			"You do not have data.zip!"
			"\n\nGrab it from your purchased copy of the game,"
			"\nor get it from the free Make and Play Edition.",
			NULL
		);
		return 0;
	}

	strcpy(output, PHYSFS_getBaseDir());
	strcpy(output, "gamecontrollerdb.txt");
	if (SDL_GameControllerAddMappingsFromFile(output) < 0)
	{
		printf("gamecontrollerdb.txt not found!\n");
	}
	return 1;
}

void FILESYSTEM_deinit()
{
	PHYSFS_deinit();
}

char *FILESYSTEM_getUserSaveDirectory()
{
	return saveDir;
}

char *FILESYSTEM_getUserLevelDirectory()
{
	return levelDir;
}

void FILESYSTEM_loadFileToMemory(const char *name, unsigned char **mem,
                                 size_t *len, bool addnull)
{
	if (strcmp(name, "levels/special/stdin.vvvvvv") == 0) {
		// this isn't *technically* necessary when piping directly from a file, but checking for that is annoying
		static std::vector<char> STDIN_BUFFER;
		static bool STDIN_LOADED = false;
		if (!STDIN_LOADED) {
			std::istreambuf_iterator<char> begin(std::cin), end;
			STDIN_BUFFER.assign(begin, end);
			STDIN_BUFFER.push_back(0); // there's no observable change in behavior if addnull is always true, but not vice versa
			STDIN_LOADED = true;
		}

		size_t length = STDIN_BUFFER.size() - 1;
		if (len != NULL) {
			*len = length;
		}

		++length;
		*mem = static_cast<unsigned char*>(malloc(length)); // STDIN_BUFFER.data() causes double-free
		std::copy(STDIN_BUFFER.begin(), STDIN_BUFFER.end(), reinterpret_cast<char*>(*mem));
		return;
	}

	PHYSFS_File *handle = PHYSFS_openRead(name);
	if (handle == NULL)
	{
		return;
	}
	PHYSFS_uint32 length = PHYSFS_fileLength(handle);
	if (len != NULL)
	{
		*len = length;
	}
	if (addnull)
	{
		*mem = (unsigned char *) malloc(length + 1);
		(*mem)[length] = 0;
	}
	else
	{
		*mem = (unsigned char*) malloc(length);
	}
	int success = PHYSFS_readBytes(handle, *mem, length);
	if (success == -1)
	{
		FILESYSTEM_freeMemory(mem);
	}
	PHYSFS_close(handle);
}

void FILESYSTEM_freeMemory(unsigned char **mem)
{
	free(*mem);
	*mem = NULL;
}

bool FILESYSTEM_saveTiXmlDocument(const char *name, TiXmlDocument *doc)
{
	/* TiXmlDocument.SaveFile doesn't account for Unicode paths, PHYSFS does */
	TiXmlPrinter printer;
	doc->Accept(&printer);
	PHYSFS_File* handle = PHYSFS_openWrite(name);
	if (handle == NULL)
	{
		return false;
	}
	PHYSFS_writeBytes(handle, printer.CStr(), printer.Size());
	PHYSFS_close(handle);
	return true;
}

bool FILESYSTEM_loadTiXmlDocument(const char *name, TiXmlDocument *doc)
{
	/* TiXmlDocument.SaveFile doesn't account for Unicode paths, PHYSFS does */
	unsigned char *mem = NULL;
	FILESYSTEM_loadFileToMemory(name, &mem, NULL, true);
	if (mem == NULL)
	{
		return false;
	}
	doc->Parse((const char*)mem, NULL, TIXML_ENCODING_UTF8);
	FILESYSTEM_freeMemory(&mem);
	return true;
}

std::vector<std::string> FILESYSTEM_getLevelDirFileNames()
{
	std::vector<std::string> list;
	char **fileList = PHYSFS_enumerateFiles("/levels");
	char **i;
	std::string builtLocation;

	for (i = fileList; *i != NULL; i++)
	{
		if (strcmp(*i, "data") == 0)
		{
			continue; /* FIXME: lolwut -flibit */
		}
		builtLocation = "levels/";
		builtLocation += *i;
		list.push_back(builtLocation);
	}

	PHYSFS_freeList(fileList);

	return list;
}

void PLATFORM_getOSDirectory(char* output)
{
#ifdef _WIN32
	/* This block is here for compatibility, do not touch it! */
	WCHAR utf16_path[MAX_PATH];
	SHGetFolderPathW(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, utf16_path);
	WideCharToMultiByte(CP_UTF8, 0, utf16_path, -1, output, MAX_PATH, NULL, NULL);
	strcat(output, "\\VVVVVV\\");
#else
	strcpy(output, PHYSFS_getPrefDir("distractionware", "VVVVVV"));
#endif
}

void PLATFORM_migrateSaveData(char* output)
{
	char oldLocation[MAX_PATH];
	char newLocation[MAX_PATH];
	char oldDirectory[MAX_PATH];
#if defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__HAIKU__) || defined(__DragonFly__)
	DIR *dir = NULL;
	struct dirent *de = NULL;
	DIR *subDir = NULL;
	struct dirent *subDe = NULL;
	char subDirLocation[MAX_PATH];
	const char *homeDir = getenv("HOME");
	if (homeDir == NULL)
	{
		/* Uhh, I don't want to get near this. -flibit */
		return;
	}
	strcpy(oldDirectory, homeDir);
#if defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__HAIKU__) || defined(__DragonFly__)
	strcat(oldDirectory, "/.vvvvvv/");
#elif defined(__APPLE__)
	strcat(oldDirectory, "/Documents/VVVVVV/");
#endif
	dir = opendir(oldDirectory);
	if (!dir)
	{
		printf("Could not find directory %s\n", oldDirectory);
		return;
	}

	printf("Migrating old savedata to new location...\n");
	for (de = readdir(dir); de != NULL; de = readdir(dir))
	{
		if (	strcmp(de->d_name, "..") == 0 ||
			strcmp(de->d_name, ".") == 0	)
		{
			continue;
		}
		#define COPY_SAVEFILE(name) \
			else if (strcmp(de->d_name, name) == 0) \
			{ \
				strcpy(oldLocation, oldDirectory); \
				strcat(oldLocation, name); \
				strcpy(newLocation, output); \
				strcat(newLocation, "saves/"); \
				strcat(newLocation, name); \
				PLATFORM_copyFile(oldLocation, newLocation); \
			}
		COPY_SAVEFILE("unlock.vvv")
		COPY_SAVEFILE("tsave.vvv")
		COPY_SAVEFILE("qsave.vvv")
		#undef COPY_SAVEFILE
		else if (strstr(de->d_name, ".vvvvvv.vvv") != NULL)
		{
			strcpy(oldLocation, oldDirectory);
			strcat(oldLocation, de->d_name);
			strcpy(newLocation, output);
			strcat(newLocation, "saves/");
			strcat(newLocation, de->d_name);
			PLATFORM_copyFile(oldLocation, newLocation);
		}
		else if (strstr(de->d_name, ".vvvvvv") != NULL)
		{
			strcpy(oldLocation, oldDirectory);
			strcat(oldLocation, de->d_name);
			strcpy(newLocation, output);
			strcat(newLocation, "levels/");
			strcat(newLocation, de->d_name);
			PLATFORM_copyFile(oldLocation, newLocation);
		}
		else if (strcmp(de->d_name, "Saves") == 0)
		{
			strcpy(subDirLocation, oldDirectory);
			strcat(subDirLocation, "Saves/");
			subDir = opendir(subDirLocation);
			if (!subDir)
			{
				printf("Could not open Saves/ subdir!\n");
				continue;
			}
			for (
				subDe = readdir(subDir);
				subDe != NULL;
				subDe = readdir(subDir)
			) {
				#define COPY_SAVEFILE(name) \
					(strcmp(subDe->d_name, name) == 0) \
					{ \
						strcpy(oldLocation, subDirLocation); \
						strcat(oldLocation, name); \
						strcpy(newLocation, output); \
						strcat(newLocation, "saves/"); \
						strcat(newLocation, name); \
						PLATFORM_copyFile(oldLocation, newLocation); \
					}
				if COPY_SAVEFILE("unlock.vvv")
				else if COPY_SAVEFILE("tsave.vvv")
				else if COPY_SAVEFILE("qsave.vvv")
				#undef COPY_SAVEFILE
			}
		}
	}
#elif defined(_WIN32)
	WIN32_FIND_DATA findHandle;
	HANDLE hFind = NULL;
	char fileSearch[MAX_PATH];

	/* Same place, different layout. */
	strcpy(oldDirectory, output);

	/* In theory we don't need to worry about this, thanks case insensitivity!
	sprintf(fileSearch, "%s\\Saves\\*.vvv", oldDirectory);
	hFind = FindFirstFile(fileSearch, &findHandle);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		printf("Could not find directory %s\\Saves\\\n", oldDirectory);
	}
	else do
	{
		if ((findHandle.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			#define COPY_SAVEFILE(name) \
				(strcmp(findHandle.cFileName, name) == 0) \
				{ \
					strcpy(oldLocation, oldDirectory); \
					strcat(oldLocation, "Saves\\"); \
					strcat(oldLocation, name); \
					strcpy(newLocation, output); \
					strcat(newLocation, "saves\\"); \
					strcat(newLocation, name); \
					PLATFORM_copyFile(oldLocation, newLocation); \
				}
			if COPY_SAVEFILE("unlock.vvv")
			else if COPY_SAVEFILE("tsave.vvv")
			else if COPY_SAVEFILE("qsave.vvv")
			#undef COPY_SAVEFILE
		}
	} while (FindNextFile(hFind, &findHandle));
	*/

	sprintf(fileSearch, "%s\\*.vvvvvv", oldDirectory);
	hFind = FindFirstFile(fileSearch, &findHandle);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		printf("Could not find directory %s\n", oldDirectory);
	}
	else do
	{
		if ((findHandle.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			strcpy(oldLocation, oldDirectory);
			strcat(oldLocation, findHandle.cFileName);
			strcpy(newLocation, output);
			strcat(newLocation, "levels\\");
			strcat(newLocation, findHandle.cFileName);
			PLATFORM_copyFile(oldLocation, newLocation);
		}
	} while (FindNextFile(hFind, &findHandle));
#else
#error See PLATFORM_migrateSaveData
#endif
}

void PLATFORM_copyFile(const char *oldLocation, const char *newLocation)
{
	char *data;
	long int length;

	/* Read data */
	FILE *file = fopen(oldLocation, "rb");
	if (!file)
	{
		printf("Cannot open/copy %s\n", oldLocation);
		return;
	}
	fseek(file, 0, SEEK_END);
	length = ftell(file);
	fseek(file, 0, SEEK_SET);
	data = (char*) malloc(length);
	fread(data, 1, length, file);
	fclose(file);

	/* Write data */
	file = fopen(newLocation, "wb");
	if (!file)
	{
		printf("Could not write to %s\n", newLocation);
		free(data);
		return;
	}
	fwrite(data, 1, length, file);
	fclose(file);
	free(data);

	/* WTF did we just do */
	printf("Copied:\n\tOld: %s\n\tNew: %s\n", oldLocation, newLocation);
}

bool FILESYSTEM_openDirectoryEnabled()
{
	/* This is just a check to see if we're on a desktop or tenfoot setup.
	 * If you're working on a tenfoot-only build, add a def that always
	 * returns false!
	 */
	return !SDL_GetHintBoolean("SteamTenfoot", SDL_FALSE);
}

#ifdef _WIN32
bool FILESYSTEM_openDirectory(const char *dname)
{
	ShellExecute(NULL, "open", dname, NULL, NULL, SW_SHOWMINIMIZED);
	return true;
}
#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__HAIKU__) || defined(__DragonFly__)
 #if defined(__APPLE__) || defined(__HAIKU__)
const char* open_cmd = "open";
 #else
const char* open_cmd = "xdg-open";
 #endif

extern "C" char** environ;

bool FILESYSTEM_openDirectory(const char *dname)
{
	pid_t child;
	// This const_cast is legal (ctrl-f "The statement" at https://pubs.opengroup.org/onlinepubs/9699919799/functions/exec.html
	char* argv[3] = {const_cast<char*>(open_cmd), const_cast<char*>(dname), NULL};
	posix_spawnp(&child, open_cmd, NULL, NULL, argv, environ);
	int status = 0;
	waitpid(child, &status, 0);
	return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}
#else
bool FILESYSTEM_openDirectory(const char *dname)
{
	return false;
}
#endif

bool FILESYSTEM_delete(const char *name)
{
    return PHYSFS_delete(name) != 0;
}
