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
#include "Maths.h"
#include "UtilityClass.h"

/* These are needed for PLATFORM_* crap */
#if defined(_WIN32)
#include <windows.h>
#include <shlobj.h>
int mkdir(char* path, int mode)
{
	WCHAR utf16_path[MAX_PATH];
	MultiByteToWideChar(CP_UTF8, 0, path, -1, utf16_path, MAX_PATH);
	return CreateDirectoryW(utf16_path, NULL);
}
#define VNEEDS_MIGRATION (mkdirResult != 0)
#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__HAIKU__) || defined(__DragonFly__)
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>
#define VNEEDS_MIGRATION (mkdirResult == 0)
#define MAX_PATH PATH_MAX
#endif

static char saveDir[MAX_PATH] = {'\0'};
static char levelDir[MAX_PATH] = {'\0'};

static char assetDir[MAX_PATH] = {'\0'};
static char virtualMountPath[MAX_PATH] = {'\0'};

static void PLATFORM_getOSDirectory(char* output);
static void PLATFORM_migrateSaveData(char* output);
static void PLATFORM_copyFile(const char *oldLocation, const char *newLocation);

static void* bridged_malloc(PHYSFS_uint64 size)
{
	return SDL_malloc(size);
}

static void* bridged_realloc(void* ptr, PHYSFS_uint64 size)
{
	return SDL_realloc(ptr, size);
}

static const PHYSFS_Allocator allocator = {
	NULL,
	NULL,
	bridged_malloc,
	bridged_realloc,
	SDL_free
};

int FILESYSTEM_init(char *argvZero, char* baseDir, char *assetsPath)
{
	char output[MAX_PATH];
	int mkdirResult;
	int retval;
	const char* pathSep = PHYSFS_getDirSeparator();
	char* basePath;

	PHYSFS_setAllocator(&allocator);
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

	/* Mount our base user directory */
	PHYSFS_mount(output, NULL, 0);
	PHYSFS_setWriteDir(output);
	printf("Base directory: %s\n", output);

	/* Store full save directory */
	SDL_snprintf(saveDir, sizeof(saveDir), "%s%s%s",
		output,
		"saves",
		pathSep
	);
	mkdir(saveDir, 0777); /* FIXME: Why did I not | this? -flibit */
	printf("Save directory: %s\n", saveDir);

	/* Store full level directory */
	SDL_snprintf(levelDir, sizeof(levelDir), "%s%s%s",
		output,
		"levels",
		pathSep
	);
	mkdirResult = mkdir(levelDir, 0777);
	printf("Level directory: %s\n", levelDir);

	/* We didn't exist until now, migrate files! */
	if (VNEEDS_MIGRATION)
	{
		PLATFORM_migrateSaveData(output);
	}

	basePath = SDL_GetBasePath();

	if (basePath == NULL)
	{
		puts("Unable to get base path!");
		return 0;
	}

	/* Mount the stock content last */
	if (assetsPath)
	{
		SDL_strlcpy(output, assetsPath, sizeof(output));
	}
	else
	{
		SDL_snprintf(output, sizeof(output), "%s%s",
			basePath,
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
		retval = 0;
		goto end;
	}

	SDL_snprintf(output, sizeof(output), "%s%s", basePath, "gamecontrollerdb.txt");
	if (SDL_GameControllerAddMappingsFromFile(output) < 0)
	{
		printf("gamecontrollerdb.txt not found!\n");
	}
	retval = 1;

end:
	SDL_free(basePath);
	return retval;
}

void FILESYSTEM_deinit(void)
{
	if (PHYSFS_isInit())
	{
		PHYSFS_deinit();
	}
}

char *FILESYSTEM_getUserSaveDirectory(void)
{
	return saveDir;
}

char *FILESYSTEM_getUserLevelDirectory(void)
{
	return levelDir;
}

bool FILESYSTEM_isFile(const char* filename)
{
	PHYSFS_Stat stat;

	bool success = PHYSFS_stat(filename, &stat);

	if (!success)
	{
		printf(
			"Could not stat file: %s\n",
			PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())
		);
		return false;
	}

	/* We unfortunately cannot follow symlinks (PhysFS limitation).
	 * Let the caller deal with them.
	 */
	return stat.filetype == PHYSFS_FILETYPE_REGULAR
	|| stat.filetype == PHYSFS_FILETYPE_SYMLINK;
}

bool FILESYSTEM_isMounted(const char* filename)
{
	return PHYSFS_getMountPoint(filename) != NULL;
}

static bool FILESYSTEM_exists(const char *fname)
{
	return PHYSFS_exists(fname);
}

static void generateVirtualMountPath(char* path, const size_t path_size)
{
	char random[6 + 1] = {'\0'};
	size_t i;
	for (i = 0; i < SDL_arraysize(random) - 1; ++i)
	{
		/* Generate a-z0-9 (base 36) */
		char randchar = fRandom() * 36;
		if (randchar <= 26)
		{
			randchar += 'a';
		}
		else
		{
			randchar -= 26;
			randchar += '0';
		}
		random[i] = randchar;
	}
	SDL_snprintf(
		path,
		path_size,
		".vvv-mnt-virtual-%s/custom-assets/",
		random
	);
}

static bool FILESYSTEM_mountAssetsFrom(const char *fname)
{
	const char* real_dir = PHYSFS_getRealDir(fname);
	char path[MAX_PATH];

	if (real_dir == NULL)
	{
		printf(
			"Could not mount %s: real directory doesn't exist\n",
			fname
		);
		return false;
	}

	SDL_snprintf(path, sizeof(path), "%s/%s", real_dir, fname);

	generateVirtualMountPath(virtualMountPath, sizeof(virtualMountPath));

	if (!PHYSFS_mount(path, virtualMountPath, 0))
	{
		printf(
			"Error mounting %s: %s\n",
			fname,
			PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())
		);
		return false;
	}

	SDL_strlcpy(assetDir, path, sizeof(assetDir));
	return true;
}

void FILESYSTEM_loadZip(const char* filename)
{
	PHYSFS_File* zip = PHYSFS_openRead(filename);

	if (!PHYSFS_mountHandle(zip, filename, "levels", 1))
	{
		printf(
			"Could not mount %s: %s\n",
			filename,
			PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())
		);
	}
}

void FILESYSTEM_mountAssets(const char* path)
{
	const size_t path_size = SDL_strlen(path);
	char filename[MAX_PATH];
	char zip_data[MAX_PATH];
	const char* zip_normal;
	char dir[MAX_PATH];

	/* path is going to look like "levels/LEVELNAME.vvvvvv".
	 * We want LEVELNAME, which entails starting from index 7
	 * (which is how long "levels/" is)
	 * and then grabbing path_size-14 characters
	 * (14 chars because "levels/" and ".vvvvvv" are both 7 chars).
	 * We also add 1 when calculating the amount of bytes to grab
	 * to account for the null terminator.
	 */
	SDL_strlcpy(
		filename,
		&path[7],
		VVV_min((path_size - 14) + 1, sizeof(filename))
	);

	SDL_snprintf(
		zip_data,
		sizeof(zip_data),
		"levels/%s.data.zip",
		filename
	);

	zip_normal = PHYSFS_getRealDir(path);

	SDL_snprintf(
		dir,
		sizeof(dir),
		"levels/%s/",
		filename
	);

	if (FILESYSTEM_exists(zip_data))
	{
		printf("Custom asset directory is .data.zip at %s\n", zip_data);

		if (!FILESYSTEM_mountAssetsFrom(zip_data))
		{
			return;
		}

		graphics.reloadresources();
	}
	else if (zip_normal != NULL && endsWith(zip_normal, ".zip"))
	{
		printf("Custom asset directory is .zip at %s\n", zip_normal);

		if (!FILESYSTEM_mountAssetsFrom(zip_normal))
		{
			return;
		}

		graphics.reloadresources();
	}
	else if (FILESYSTEM_exists(dir))
	{
		printf("Custom asset directory exists at %s\n", dir);

		if (!FILESYSTEM_mountAssetsFrom(dir))
		{
			return;
		}

		graphics.reloadresources();
	}
	else
	{
		puts("Custom asset directory does not exist");
	}
}

void FILESYSTEM_unmountAssets(void)
{
	if (assetDir[0] != '\0')
	{
		printf("Unmounting %s\n", assetDir);
		PHYSFS_unmount(assetDir);
		assetDir[0] = '\0';
		graphics.reloadresources();
	}
	else
	{
		printf("Cannot unmount when no asset directory is mounted\n");
	}
}

static void getMountedPath(
	char* buffer,
	const size_t buffer_size,
	const char* filename
) {
	const char* path;
	const bool assets_mounted = assetDir[0] != '\0';
	char mounted_path[MAX_PATH];

	if (assets_mounted)
	{
		SDL_snprintf(
			mounted_path,
			sizeof(mounted_path),
			"%s%s",
			virtualMountPath,
			filename
		);
	}

	if (assets_mounted && PHYSFS_exists(mounted_path))
	{
		path = mounted_path;
	}
	else
	{
		path = filename;
	}

	SDL_strlcpy(buffer, path, buffer_size);
}

bool FILESYSTEM_isAssetMounted(const char* filename)
{
	const char* realDir;

	/* Fast path */
	if (assetDir[0] == '\0')
	{
		return false;
	}

	realDir = PHYSFS_getRealDir(filename);

	if (realDir == NULL)
	{
		return false;
	}

	return SDL_strcmp(assetDir, realDir) == 0;
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
	PHYSFS_sint64 length = PHYSFS_fileLength(handle);
	if (len != NULL)
	{
		if (length < 0)
		{
			length = 0;
		}
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
	PHYSFS_sint64 success = PHYSFS_readBytes(handle, *mem, length);
	if (success == -1)
	{
		FILESYSTEM_freeMemory(mem);
	}
	PHYSFS_close(handle);
}

void FILESYSTEM_loadAssetToMemory(
	const char* name,
	unsigned char** mem,
	size_t* len,
	const bool addnull
) {
	char path[MAX_PATH];

	getMountedPath(path, sizeof(path), name);

	FILESYSTEM_loadFileToMemory(path, mem, len, addnull);
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
	mkdir(output, 0777);
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

bool FILESYSTEM_openDirectoryEnabled(void)
{
	/* This is just a check to see if we're on a desktop or tenfoot setup.
	 * If you're working on a tenfoot-only build, add a def that always
	 * returns false!
	 */
	return !SDL_GetHintBoolean("SteamTenfoot", SDL_FALSE);
}

bool FILESYSTEM_openDirectory(const char *dname)
{
	char url[MAX_PATH];
	SDL_snprintf(url, sizeof(url), "file://%s", dname);
	if (SDL_OpenURL(url) == -1)
	{
		printf("Error opening directory: %s\n", SDL_GetError());
		return false;
	}
	return true;
}

bool FILESYSTEM_delete(const char *name)
{
	return PHYSFS_delete(name) != 0;
}
