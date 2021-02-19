#include <algorithm>
#include <iostream>
#include <iterator>
#include <physfs.h>
#include <SDL.h>
#include <stdio.h>
#include <string>
#include <tinyxml2.h>
#include <vector>

#include "Exit.h"
#include "Graphics.h"
#include "UtilityClass.h"

/* These are needed for PLATFORM_* crap */
#if defined(_WIN32)
#include <windows.h>
#include <shlobj.h>
#include <shellapi.h>
#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__HAIKU__) || defined(__DragonFly__)
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <spawn.h>
#define MAX_PATH PATH_MAX
#endif

static char saveDir[MAX_PATH] = {'\0'};
static char levelDir[MAX_PATH] = {'\0'};

static void PLATFORM_getOSDirectory(char* output);
static void PLATFORM_migrateSaveData(char* output);
static void PLATFORM_copyFile(const char *oldLocation, const char *newLocation);

int FILESYSTEM_init(char *argvZero, char* baseDir, char *assetsPath)
{
	char output[MAX_PATH];
	int mkdirResult;
	const char* pathSep = PHYSFS_getDirSeparator();

	PHYSFS_init(argvZero);
	PHYSFS_permitSymbolicLinks(1);

	/* Determine the OS user directory */
	if (baseDir && baseDir[0] != '\0')
	{
		/* We later append to this path and assume it ends in a slash */
		bool trailing_pathsep = SDL_strcmp(baseDir + SDL_strlen(baseDir) - SDL_strlen(pathSep), pathSep) == 0;

		SDL_snprintf(output, sizeof(output), "%s%s",
			baseDir,
			!trailing_pathsep ? pathSep : ""
		);
	}
	else
	{
		PLATFORM_getOSDirectory(output);
	}

	/* Create base user directory, mount */
	mkdirResult = PHYSFS_mkdir(output);

	/* Mount our base user directory */
	PHYSFS_mount(output, NULL, 0);
	PHYSFS_setWriteDir(output);
	printf("Base directory: %s\n", output);

	/* Create the save/level folders */
	mkdirResult |= PHYSFS_mkdir("saves");
	mkdirResult |= PHYSFS_mkdir("levels");

	/* Store full save directory */
	SDL_snprintf(saveDir, sizeof(saveDir), "%s%s%s",
		output,
		"saves",
		PHYSFS_getDirSeparator()
	);
	printf("Save directory: %s\n", saveDir);

	/* Store full level directory */
	SDL_snprintf(levelDir, sizeof(levelDir), "%s%s%s",
		output,
		"levels",
		PHYSFS_getDirSeparator()
	);
	printf("Level directory: %s\n", levelDir);

	/* We didn't exist until now, migrate files! */
	if (mkdirResult == 0)
	{
		PLATFORM_migrateSaveData(output);
	}

	/* Mount the stock content last */
	if (assetsPath)
	{
		SDL_strlcpy(output, assetsPath, sizeof(output));
	}
	else
	{
		SDL_snprintf(output, sizeof(output), "%s%s",
			PHYSFS_getBaseDir(),
			"data.zip"
		);
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

	SDL_snprintf(output, sizeof(output), "%s%s", PHYSFS_getBaseDir(), "gamecontrollerdb.txt");
	if (SDL_GameControllerAddMappingsFromFile(output) < 0)
	{
		printf("gamecontrollerdb.txt not found!\n");
	}
	return 1;
}

void FILESYSTEM_deinit()
{
	if (PHYSFS_isInit())
	{
		PHYSFS_deinit();
	}
}

char *FILESYSTEM_getUserSaveDirectory()
{
	return saveDir;
}

char *FILESYSTEM_getUserLevelDirectory()
{
	return levelDir;
}

bool FILESYSTEM_directoryExists(const char *fname)
{
	return PHYSFS_exists(fname);
}

void FILESYSTEM_mount(const char *fname)
{
	std::string path(PHYSFS_getRealDir(fname));
	path += PHYSFS_getDirSeparator();
	path += fname;
	if (!PHYSFS_mount(path.c_str(), NULL, 0))
	{
		printf("Error mounting: %s\n", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
	}
	else
	{
		graphics.assetdir = path.c_str();
	}
}

bool FILESYSTEM_assetsmounted = false;

void FILESYSTEM_mountassets(const char* path)
{
	const std::string _path(path);

	std::string zippath = "levels/" + _path.substr(7,_path.size()-14) + ".data.zip";
	std::string dirpath = "levels/" + _path.substr(7,_path.size()-14) + "/";
	std::string zip_path;
	const char* cstr = PHYSFS_getRealDir(_path.c_str());

	if (cstr) {
		zip_path = cstr;
	}

	if (cstr && FILESYSTEM_directoryExists(zippath.c_str())) {
		printf("Custom asset directory exists at %s\n", zippath.c_str());
		FILESYSTEM_mount(zippath.c_str());
		graphics.reloadresources();
		FILESYSTEM_assetsmounted = true;
	} else if (zip_path != "data.zip" && !endsWith(zip_path, "/data.zip") && endsWith(zip_path, ".zip")) {
		printf("Custom asset directory is .zip at %s\n", zip_path.c_str());
		PHYSFS_File* zip = PHYSFS_openRead(zip_path.c_str());
		zip_path += ".data.zip";
		if (zip == NULL) {
			printf("error loading .zip: %s\n", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		} else if (PHYSFS_mountHandle(zip, zip_path.c_str(), "/", 0) == 0) {
			printf("error mounting .zip: %s\n", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		} else {
			graphics.assetdir = zip_path;
		}
		FILESYSTEM_assetsmounted = true;
		graphics.reloadresources();
	} else if (FILESYSTEM_directoryExists(dirpath.c_str())) {
		printf("Custom asset directory exists at %s\n",dirpath.c_str());
		FILESYSTEM_mount(dirpath.c_str());
		graphics.reloadresources();
		FILESYSTEM_assetsmounted = true;
	} else {
		printf("Custom asset directory does not exist\n");
		FILESYSTEM_assetsmounted = false;
	}
}

void FILESYSTEM_unmountassets()
{
	if (graphics.assetdir != "")
	{
		printf("Unmounting %s\n", graphics.assetdir.c_str());
		PHYSFS_unmount(graphics.assetdir.c_str());
		graphics.assetdir = "";
		graphics.reloadresources();
	}
	else
	{
		printf("Cannot unmount when no asset directory is mounted\n");
	}
	FILESYSTEM_assetsmounted = false;
}

void FILESYSTEM_freeMemory(unsigned char **mem);

void FILESYSTEM_loadFileToMemory(
	const char *name,
	unsigned char **mem,
	size_t *len,
	bool addnull
) {
	if (SDL_strcmp(name, "levels/special/stdin.vvvvvv") == 0)
	{
		// this isn't *technically* necessary when piping directly from a file, but checking for that is annoying
		static std::vector<char> STDIN_BUFFER;
		static bool STDIN_LOADED = false;
		if (!STDIN_LOADED)
		{
			std::istreambuf_iterator<char> begin(std::cin), end;
			STDIN_BUFFER.assign(begin, end);
			STDIN_BUFFER.push_back(0); // there's no observable change in behavior if addnull is always true, but not vice versa
			STDIN_LOADED = true;
		}

		size_t length = STDIN_BUFFER.size() - 1;
		if (len != NULL)
		{
			*len = length;
		}

		++length;
		*mem = static_cast<unsigned char*>(SDL_malloc(length)); // STDIN_BUFFER.data() causes double-free
		if (*mem == NULL)
		{
			VVV_exit(1);
		}
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
		*mem = (unsigned char *) SDL_malloc(length + 1);
		if (*mem == NULL)
		{
			VVV_exit(1);
		}
		(*mem)[length] = 0;
	}
	else
	{
		*mem = (unsigned char*) SDL_malloc(length);
		if (*mem == NULL)
		{
			VVV_exit(1);
		}
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
	SDL_free(*mem);
	*mem = NULL;
}

bool FILESYSTEM_saveTiXml2Document(const char *name, tinyxml2::XMLDocument& doc)
{
	/* XMLDocument.SaveFile doesn't account for Unicode paths, PHYSFS does */
	tinyxml2::XMLPrinter printer;
	doc.Print(&printer);
	PHYSFS_File* handle = PHYSFS_openWrite(name);
	if (handle == NULL)
	{
		return false;
	}
	PHYSFS_writeBytes(handle, printer.CStr(), printer.CStrSize() - 1); // subtract one because CStrSize includes terminating null
	PHYSFS_close(handle);
	return true;
}

bool FILESYSTEM_loadTiXml2Document(const char *name, tinyxml2::XMLDocument& doc)
{
	/* XMLDocument.LoadFile doesn't account for Unicode paths, PHYSFS does */
	unsigned char *mem = NULL;
	FILESYSTEM_loadFileToMemory(name, &mem, NULL, true);
	if (mem == NULL)
	{
		return false;
	}
	doc.Parse((const char*) mem);
	FILESYSTEM_freeMemory(&mem);
	return true;
}

static PHYSFS_EnumerateCallbackResult enumerateCallback(
	void* data,
	const char* origdir,
	const char* filename
) {
	void (*callback)(const char*) = (void (*)(const char*)) data;
	char builtLocation[MAX_PATH];

	SDL_snprintf(
		builtLocation,
		sizeof(builtLocation),
		"%s/%s",
		origdir,
		filename
	);

	callback(builtLocation);

	return PHYSFS_ENUM_OK;
}

void FILESYSTEM_enumerateLevelDirFileNames(
	void (*callback)(const char* filename)
) {
	int success = PHYSFS_enumerate("levels", enumerateCallback, (void*) callback);

	if (success == 0)
	{
		printf(
			"Could not get list of levels: %s\n",
			PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())
		);
	}
}

static void PLATFORM_getOSDirectory(char* output)
{
#ifdef _WIN32
	/* This block is here for compatibility, do not touch it! */
	WCHAR utf16_path[MAX_PATH];
	SHGetFolderPathW(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, utf16_path);
	WideCharToMultiByte(CP_UTF8, 0, utf16_path, -1, output, MAX_PATH, NULL, NULL);
	SDL_strlcat(output, "\\VVVVVV\\", MAX_PATH);
#else
	SDL_strlcpy(output, PHYSFS_getPrefDir("distractionware", "VVVVVV"), MAX_PATH);
#endif
}

static void PLATFORM_migrateSaveData(char* output)
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
	const char *homeDir = SDL_getenv("HOME");
	if (homeDir == NULL)
	{
		/* Uhh, I don't want to get near this. -flibit */
		return;
	}
	const char oldPath[] =
 #if defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__HAIKU__) || defined(__DragonFly__)
		"/.vvvvvv/";
 #elif defined(__APPLE__)
		"/Documents/VVVVVV/";
 #endif
	SDL_snprintf(oldDirectory, sizeof(oldDirectory), "%s%s", homeDir, oldPath);
	dir = opendir(oldDirectory);
	if (!dir)
	{
		printf("Could not find directory %s\n", oldDirectory);
		return;
	}

	printf("Migrating old savedata to new location...\n");
	for (de = readdir(dir); de != NULL; de = readdir(dir))
	{
		if (	SDL_strcmp(de->d_name, "..") == 0 ||
			SDL_strcmp(de->d_name, ".") == 0	)
		{
			continue;
		}
		#define COPY_SAVEFILE(name) \
			else if (SDL_strcmp(de->d_name, name) == 0) \
			{ \
				SDL_snprintf(oldLocation, sizeof(oldLocation), "%s%s", oldDirectory, name); \
				SDL_snprintf(newLocation, sizeof(newLocation), "%ssaves/%s", output, name); \
				PLATFORM_copyFile(oldLocation, newLocation); \
			}
		COPY_SAVEFILE("unlock.vvv")
		COPY_SAVEFILE("tsave.vvv")
		COPY_SAVEFILE("qsave.vvv")
		#undef COPY_SAVEFILE
		else if (SDL_strstr(de->d_name, ".vvvvvv.vvv") != NULL)
		{
			SDL_snprintf(oldLocation, sizeof(oldLocation), "%s%s", oldDirectory, de->d_name);
			SDL_snprintf(newLocation, sizeof(newLocation), "%ssaves/%s", output, de->d_name);
			PLATFORM_copyFile(oldLocation, newLocation);
		}
		else if (SDL_strstr(de->d_name, ".vvvvvv") != NULL)
		{
			SDL_snprintf(oldLocation, sizeof(oldLocation), "%s%s", oldDirectory, de->d_name);
			SDL_snprintf(newLocation, sizeof(newLocation), "%slevels/%s", output, de->d_name);
			PLATFORM_copyFile(oldLocation, newLocation);
		}
		else if (SDL_strcmp(de->d_name, "Saves") == 0)
		{
			SDL_snprintf(subDirLocation, sizeof(subDirLocation), "%sSaves/", oldDirectory);
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
					(SDL_strcmp(subDe->d_name, name) == 0) \
					{ \
						SDL_snprintf(oldLocation, sizeof(oldLocation), "%s%s", subDirLocation, name); \
						SDL_snprintf(newLocation, sizeof(newLocation), "%ssaves/%s", output, name); \
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
	SDL_strlcpy(oldDirectory, output, sizeof(oldDirectory));

	SDL_snprintf(fileSearch, sizeof(fileSearch), "%s\\*.vvvvvv", oldDirectory);
	hFind = FindFirstFile(fileSearch, &findHandle);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		printf("Could not find directory %s\n", oldDirectory);
	}
	else do
	{
		if ((findHandle.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			SDL_snprintf(oldLocation, sizeof(oldLocation), "%s%s", oldDirectory, findHandle.cFileName);
			SDL_snprintf(newLocation, sizeof(newLocation), "%slevels\\%s", output, findHandle.cFileName);
			PLATFORM_copyFile(oldLocation, newLocation);
		}
	} while (FindNextFile(hFind, &findHandle));
#else
#error See PLATFORM_migrateSaveData
#endif
}

static void PLATFORM_copyFile(const char *oldLocation, const char *newLocation)
{
	char *data;
	size_t length, bytes_read, bytes_written;

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
	data = (char*) SDL_malloc(length);
	if (data == NULL)
	{
		VVV_exit(1);
	}
	bytes_read = fread(data, 1, length, file);
	fclose(file);
	if (bytes_read != length)
	{
		printf("An error occurred when reading from %s\n", oldLocation);
		SDL_free(data);
		return;
	}

	/* Write data */
	file = fopen(newLocation, "wb");
	if (!file)
	{
		printf("Could not write to %s\n", newLocation);
		SDL_free(data);
		return;
	}
	bytes_written = fwrite(data, 1, length, file);
	fclose(file);
	SDL_free(data);

	/* WTF did we just do */
	printf("Copied:\n\tOld: %s\n\tNew: %s\n", oldLocation, newLocation);
	if (bytes_written != length)
	{
		printf("Warning: an error occurred when writing to %s\n", newLocation);
	}
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
	char* argv[3] =
	{
		const_cast<char*>(open_cmd),
		const_cast<char*>(dname),
		NULL
	};
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
