#include <physfs.h>
#include <SDL.h>
#include <stdarg.h>
#include <stdio.h>
#include <tinyxml2.h>

#include "Alloc.h"
#include "BinaryBlob.h"
#include "Constants.h"
#include "Exit.h"
#include "Graphics.h"
#include "Localization.h"
#include "Maths.h"
#include "Screen.h"
#include "Unused.h"
#include "UtilityClass.h"
#include "VFormat.h"
#include "Vlogging.h"

/* These are needed for PLATFORM_* crap */
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlobj.h>
static int mkdir(char* path, int mode)
{
    WCHAR utf16_path[MAX_PATH];
    MultiByteToWideChar(CP_UTF8, 0, path, -1, utf16_path, MAX_PATH);
    return CreateDirectoryW(utf16_path, NULL);
}
#elif defined(__EMSCRIPTEN__)
#include <limits.h>
#include <sys/stat.h>
#include <emscripten.h>
#define MAX_PATH PATH_MAX
#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__HAIKU__) || defined(__DragonFly__) || defined(__unix__)
#include <limits.h>
#include <sys/stat.h>
#define MAX_PATH PATH_MAX
#endif

static bool isInit = false;

static const char* pathSep = NULL;
static char* basePath = NULL;
static char writeDir[MAX_PATH] = {'\0'};
static char saveDir[MAX_PATH] = {'\0'};
static char levelDir[MAX_PATH] = {'\0'};
static char mainLangDir[MAX_PATH] = {'\0'};
static bool isMainLangDirFromRepo = false;

static char assetDir[MAX_PATH] = {'\0'};
static char virtualMountPath[MAX_PATH] = {'\0'};

static int PLATFORM_getOSDirectory(char* output, const size_t output_size);

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

void mount_pre_datazip(
    char* out_path,
    const char* real_dirname,
    const char* mount_point,
    const char* user_path
)
{
    /* Find and mount a directory (like the main language directory) in front of data.zip.
     * This directory, if not user-supplied, can be either next to data.zip,
     * or otherwise in desktop_version/ if that's found in the base path.
     *
     * out_path is assumed to be either NULL, or MAX_PATH long. If it isn't, boom */

    if (user_path != NULL)
    {
        if (PHYSFS_mount(user_path, mount_point, 1))
        {
            if (out_path != NULL)
            {
                SDL_strlcpy(out_path, user_path, MAX_PATH);
            }
        }
        else
        {
            vlog_warn("User-supplied %s directory is invalid!", real_dirname);
        }
        return;
    }

    /* Try to detect the directory, it's next to data.zip in distributed builds */
    bool dir_found = false;
    char buffer[MAX_PATH];

    SDL_snprintf(buffer, sizeof(buffer), "%s%s%s",
        basePath,
        real_dirname,
        pathSep
    );
    if (PHYSFS_mount(buffer, mount_point, 1))
    {
        dir_found = true;
    }
    else
    {
        /* If you're a developer, you probably want to use the language files/fonts
         * from the repo, otherwise it's a pain to keep everything in sync.
         * And who knows how deep in build folders our binary is. */
        size_t buf_reserve = SDL_strlen(real_dirname)+1;
        SDL_strlcpy(buffer, basePath, sizeof(buffer)-buf_reserve);

        char needle[32];
        SDL_snprintf(needle, sizeof(needle), "%sdesktop_version%s",
            pathSep,
            pathSep
        );

        /* We want the last match */
        char* match_last = NULL;
        char* match = buffer;
        while ((match = SDL_strstr(match, needle)))
        {
            match_last = match;
            match = &match[1];
        }

        if (match_last != NULL)
        {
            /* strstr only gives us a pointer and not a remaining buffer length, but that's
             * why we pretended the buffer was `buf_reserve` chars shorter than it was! */
            SDL_strlcpy(&match_last[SDL_strlen(needle)], real_dirname, buf_reserve);
            SDL_strlcat(buffer, pathSep, sizeof(buffer));

            if (PHYSFS_mount(buffer, mount_point, 1))
            {
                dir_found = true;

                if (SDL_strcmp(real_dirname, "lang") == 0)
                {
                    loc::show_translator_menu = true;
                    isMainLangDirFromRepo = true;
                }
            }
        }
    }

    if (dir_found)
    {
        if (out_path != NULL)
        {
            SDL_strlcpy(out_path, buffer, MAX_PATH);
        }
    }
    else
    {
        vlog_warn("Cannot find the %s directory anywhere!", real_dirname);
    }
}

int FILESYSTEM_init(char *argvZero, char* baseDir, char *assetsPath, char* langDir, char* fontsDir)
{
    char output[MAX_PATH];

    pathSep = PHYSFS_getDirSeparator();

    PHYSFS_setAllocator(&allocator);

    if (!PHYSFS_init(argvZero))
    {
        vlog_error(
            "Unable to initialize PhysFS: %s",
            PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())
        );
        return 0;
    }

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
    else if (!PLATFORM_getOSDirectory(output, sizeof(output)))
    {
        return 0;
    }

    /* Mount our base user directory */
    SDL_strlcpy(writeDir, output, sizeof(writeDir));
    if (!PHYSFS_mount(writeDir, NULL, 0))
    {
        vlog_error(
            "Could not mount %s: %s",
            writeDir,
            PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())
        );
        return 0;
    }
    if (!PHYSFS_setWriteDir(writeDir))
    {
        vlog_error(
            "Could not set write dir to %s: %s",
            writeDir,
            PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())
        );
        return 0;
    }
    vlog_info("Base directory: %s", writeDir);

    /* Store full save directory */
    SDL_snprintf(saveDir, sizeof(saveDir), "%s%s%s",
        writeDir,
        "saves",
        pathSep
    );
    mkdir(saveDir, 0777);
    vlog_info("Save directory: %s", saveDir);

    /* Store full level directory */
    SDL_snprintf(levelDir, sizeof(levelDir), "%s%s%s",
        writeDir,
        "levels",
        pathSep
    );
    mkdir(levelDir, 0777);
    vlog_info("Level directory: %s", levelDir);

    basePath = SDL_GetBasePath();

    if (basePath == NULL)
    {
        vlog_warn("Unable to determine base path, falling back to current directory");
        basePath = SDL_strdup("./");
    }

    mount_pre_datazip(mainLangDir, "lang", "lang/", langDir);
    vlog_info("Languages directory: %s", mainLangDir);

    mount_pre_datazip(NULL, "fonts", "graphics/", fontsDir);

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
        vlog_error("Error: data.zip missing!");
        vlog_error("You do not have data.zip!");
        vlog_error("Grab it from your purchased copy of the game,");
        vlog_error("or get it from the free Make and Play Edition.");

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

    SDL_snprintf(output, sizeof(output), "%s%s", basePath, "gamecontrollerdb.txt");
    if (SDL_GameControllerAddMappingsFromFile(output) < 0)
    {
        vlog_info("gamecontrollerdb.txt not found!");
    }

    isInit = true;
    return 1;
}

bool FILESYSTEM_isInit(void)
{
    return isInit;
}

static unsigned char* stdin_buffer = NULL;
static size_t stdin_length = 0;

void FILESYSTEM_deinit(void)
{
    if (PHYSFS_isInit())
    {
        PHYSFS_deinit();
    }
    VVV_free(stdin_buffer);
    VVV_free(basePath);
    isInit = false;
}

char *FILESYSTEM_getUserSaveDirectory(void)
{
    return saveDir;
}

char *FILESYSTEM_getUserLevelDirectory(void)
{
    return levelDir;
}

char *FILESYSTEM_getUserMainLangDirectory(void)
{
    return mainLangDir;
}

bool FILESYSTEM_isMainLangDirFromRepo(void)
{
    return isMainLangDirFromRepo;
}

bool FILESYSTEM_restoreWriteDir(void)
{
    return PHYSFS_setWriteDir(writeDir);
}

bool FILESYSTEM_setLangWriteDir(void)
{
    const char* realLangDir = PHYSFS_getRealDir("lang");
    if (realLangDir == NULL || SDL_strcmp(mainLangDir, realLangDir) != 0)
    {
        vlog_error("Not setting language write dir: %s overrules %s when loading",
            realLangDir, mainLangDir
        );
        return false;
    }
    if (!PHYSFS_setWriteDir(mainLangDir))
    {
        FILESYSTEM_restoreWriteDir();
        return false;
    }
    return true;
}

bool FILESYSTEM_isFileType(const char* filename, PHYSFS_FileType filetype)
{
    PHYSFS_Stat stat;

    bool success = PHYSFS_stat(filename, &stat);

    if (!success)
    {
        vlog_error(
            "Could not stat file: %s",
            PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())
        );
        return false;
    }

    /* We unfortunately cannot follow symlinks (PhysFS limitation).
     * Let the caller deal with them.
     */
    return stat.filetype == filetype
    || stat.filetype == PHYSFS_FILETYPE_SYMLINK;
}

bool FILESYSTEM_isFile(const char* filename)
{
    return FILESYSTEM_isFileType(filename, PHYSFS_FILETYPE_REGULAR);
}

bool FILESYSTEM_isDirectory(const char* filename)
{
    return FILESYSTEM_isFileType(filename, PHYSFS_FILETYPE_DIRECTORY);
}

bool FILESYSTEM_isMounted(const char* filename)
{
    return PHYSFS_getMountPoint(filename) != NULL;
}

static bool FILESYSTEM_exists(const char *fname)
{
    return PHYSFS_exists(fname);
}

static void generateBase36(char* string, const size_t string_size)
{
    size_t i;
    for (i = 0; i < string_size - 1; ++i)
    {
        /* a-z0-9 */
        char randchar = fRandom() * 35;
        if (randchar < 26)
        {
            randchar += 'a';
        }
        else
        {
            randchar -= 26;
            randchar += '0';
        }
        string[i] = randchar;
    }
    string[string_size - 1] = '\0';
}

static void generateVirtualMountPath(char* path, const size_t path_size)
{
    char random_str[6 + 1];
    generateBase36(random_str, sizeof(random_str));
    SDL_snprintf(
        path,
        path_size,
        ".vvv-mnt-virtual-%s/custom-assets/",
        random_str
    );
}

static char levelDirError[6*SCREEN_WIDTH_CHARS + 1] = {'\0'};

static bool levelDirHasError = false;

bool FILESYSTEM_levelDirHasError(void)
{
    return levelDirHasError;
}

void FILESYSTEM_clearLevelDirError(void)
{
    levelDirHasError = false;
}

const char* FILESYSTEM_getLevelDirError(void)
{
    return levelDirError;
}

static void setLevelDirError(const char* text, const char* args_index, ...)
{
    levelDirHasError = true;

    va_list list;
    va_start(list, args_index);
    vformat_buf_valist(levelDirError, sizeof(levelDirError), text, args_index, list);
    va_end(list);

    vlog_error("%s", levelDirError);
}

static bool FILESYSTEM_mountAssetsFrom(const char *fname)
{
    const char* real_dir = PHYSFS_getRealDir(fname);
    char path[MAX_PATH];

    if (real_dir == NULL)
    {
        setLevelDirError(
            loc::gettext("Could not mount {path}: real directory doesn't exist"),
            "path:str",
            fname
        );
        return false;
    }

    SDL_snprintf(path, sizeof(path), "%s/%s", real_dir, fname);

    generateVirtualMountPath(virtualMountPath, sizeof(virtualMountPath));

    if (!PHYSFS_mount(path, virtualMountPath, 0))
    {
        vlog_error(
            "Error mounting %s: %s",
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
    if (zip == NULL)
    {
        vlog_error(
            "Could not read zip %s: %s",
            filename,
            PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())
        );
    }

    if (!PHYSFS_mountHandle(zip, filename, "levels", 1))
    {
        vlog_error(
            "Could not mount %s: %s",
            filename,
            PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())
        );
    }
}

void FILESYSTEM_unmountAssets(void);

bool FILESYSTEM_mountAssets(const char* path)
{
    const char* real_dir = PHYSFS_getRealDir(path);

    if (real_dir != NULL &&
    SDL_strncmp(real_dir, "levels/", sizeof("levels/") - 1) == 0 &&
    endsWith(real_dir, ".zip"))
    {
        /* This is a level zip */
        vlog_info("Asset directory is .zip at %s", real_dir);

        if (!FILESYSTEM_mountAssetsFrom(real_dir))
        {
            return false;
        }

        MAYBE_FAIL(graphics.reloadresources());
    }
    else
    {
        /* If it's not a zip, look for a level folder */
        char filename[MAX_PATH];
        char virtual_path[MAX_PATH];

        VVV_between(path, "levels/", filename, ".vvvvvv");

        SDL_snprintf(
            virtual_path,
            sizeof(virtual_path),
            "levels/%s/",
            filename
        );

        if (FILESYSTEM_exists(virtual_path))
        {
            vlog_info("Asset directory exists at %s", virtual_path);

            if (!FILESYSTEM_mountAssetsFrom(virtual_path))
            {
                return false;
            }

            MAYBE_FAIL(graphics.reloadresources());
        }
        else
        {
            /* Wasn't a level zip or folder! */
            vlog_debug("Asset directory does not exist");
        }
    }

    return true;

fail:
    FILESYSTEM_unmountAssets();
    return false;
}

void FILESYSTEM_unmountAssets(void)
{
    if (assetDir[0] != '\0')
    {
        vlog_info("Unmounting %s", assetDir);
        PHYSFS_unmount(assetDir);
        assetDir[0] = '\0';
        graphics.reloadresources();
    }
    else
    {
        vlog_debug("Cannot unmount when no asset directory is mounted");
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
    char path[MAX_PATH];

    /* Fast path */
    if (assetDir[0] == '\0')
    {
        return false;
    }

    getMountedPath(path, sizeof(path), filename);

    realDir = PHYSFS_getRealDir(path);

    if (realDir == NULL)
    {
        return false;
    }

    return SDL_strcmp(assetDir, realDir) == 0;
}

bool FILESYSTEM_areAssetsInSameRealDir(const char* filenameA, const char* filenameB)
{
    char pathA[MAX_PATH];
    char pathB[MAX_PATH];

    getMountedPath(pathA, sizeof(pathA), filenameA);
    getMountedPath(pathB, sizeof(pathB), filenameB);

    const char* realDirA = PHYSFS_getRealDir(pathA);
    const char* realDirB = PHYSFS_getRealDir(pathB);

    /* Both NULL, or both the same pointer? */
    if (realDirA == realDirB)
    {
        return true;
    }

    if (realDirA == NULL || realDirB == NULL)
    {
        return false;
    }

    return SDL_strcmp(realDirA, realDirB) == 0;
}

static void load_stdin(void)
{
    size_t pos = 0;
    /* A .vvvvvv file with nothing is at least 140K...
     * initial size of 1K shouldn't hurt. */
#define INITIAL_SIZE 1024
    size_t alloc_size = INITIAL_SIZE;
    stdin_buffer = (unsigned char*) SDL_malloc(INITIAL_SIZE);
#undef INITIAL_SIZE

    if (stdin_buffer == NULL)
    {
        VVV_exit(1);
    }

    while (true)
    {
        int ch = fgetc(stdin);
        bool end = ch == EOF;
        if (end)
        {
            /* Add null terminator. There's no observable change in
             * behavior if addnull is always true, but not vice versa. */
            ch = '\0';
        }

        if (pos == alloc_size)
        {
            unsigned char *tmp;
            alloc_size *= 2;
            tmp = (unsigned char*) SDL_realloc((void*) stdin_buffer, alloc_size);
            if (tmp == NULL)
            {
                VVV_exit(1);
            }
            stdin_buffer = tmp;
        }

        stdin_buffer[pos] = ch;
        ++pos;

        if (end)
        {
            break;
        }
    }

    stdin_length = pos - 1;
}

void FILESYSTEM_loadFileToMemory(
    const char *name,
    unsigned char **mem,
    size_t *len,
    bool addnull
) {
    PHYSFS_File *handle;
    PHYSFS_sint64 length;
    PHYSFS_sint64 success;

    if (name == NULL || mem == NULL)
    {
        goto fail;
    }

    /* FIXME: Dumb hack to use `special/stdin.vvvvvv` here...
     * This is also checked elsewhere... grep for `special/stdin`! */
    if (SDL_strcmp(name, "levels/special/stdin.vvvvvv") == 0)
    {
        // this isn't *technically* necessary when piping directly from a file, but checking for that is annoying
        if (stdin_buffer == NULL)
        {
            load_stdin();
        }

        *mem = (unsigned char*) SDL_malloc(stdin_length + 1); /* + 1 for null */
        if (*mem == NULL)
        {
            VVV_exit(1);
        }

        if (len != NULL)
        {
            *len = stdin_length;
        }

        SDL_memcpy((void*) *mem, (void*) stdin_buffer, stdin_length + 1);
        return;
    }

    handle = PHYSFS_openRead(name);
    if (handle == NULL)
    {
        vlog_debug(
            "Could not read file %s: %s",
            name,
            PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())
        );
        goto fail;
    }
    length = PHYSFS_fileLength(handle);
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
    success = PHYSFS_readBytes(handle, *mem, length);
    if (success == -1)
    {
        VVV_free(*mem);
    }
    PHYSFS_close(handle);
    return;

fail:
    if (mem != NULL)
    {
        *mem = NULL;
    }
    if (len != NULL)
    {
        *len = 0;
    }
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

bool FILESYSTEM_loadBinaryBlob(binaryBlob* blob, const char* filename)
{
    PHYSFS_sint64 size;
    PHYSFS_File* handle;
    int valid, offset;
    size_t i;
    char path[MAX_PATH];

    if (blob == NULL || filename == NULL)
    {
        return false;
    }

    getMountedPath(path, sizeof(path), filename);

    handle = PHYSFS_openRead(path);
    if (handle == NULL)
    {
        vlog_debug(
            "Could not read binary blob %s: %s",
            filename,
            PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())
        );
        return false;
    }

    size = PHYSFS_fileLength(handle);

    PHYSFS_readBytes(
        handle,
        &blob->m_headers,
        sizeof(blob->m_headers)
    );

    valid = 0;
    offset = sizeof(blob->m_headers);

    for (i = 0; i < SDL_arraysize(blob->m_headers); ++i)
    {
        resourceheader* header = &blob->m_headers[i];
        char** memblock = &blob->m_memblocks[i];

        /* Name can be stupid, just needs to be terminated */
        static const size_t last_char = sizeof(header->name) - 1;
        header->name[last_char] = '\0';

        if (header->valid & ~0x1 || !header->valid)
        {
            goto fail; /* Must be EXACTLY 1 or 0 */
        }
        if (header->size < 1)
        {
            goto fail; /* Must be nonzero and positive */
        }
        if (offset + header->size > size)
        {
            goto fail; /* Bogus size value */
        }

        PHYSFS_seek(handle, offset);
        *memblock = (char*) SDL_malloc(header->size);
        if (*memblock == NULL)
        {
            VVV_exit(1); /* Oh god we're out of memory, just bail */
        }
        PHYSFS_readBytes(handle, *memblock, header->size);
        offset += header->size;
        valid += 1;

        continue;
fail:
        header->valid = false;
    }

    PHYSFS_close(handle);

    if (valid == 0)
    {
        return false;
    }

    vlog_debug("The complete reloaded file size: %lli", size);

    for (i = 0; i < SDL_arraysize(blob->m_headers); ++i)
    {
        const resourceheader* header = &blob->m_headers[i];

        if (!header->valid)
        {
            continue;
        }

        vlog_debug("%s unpacked", header->name);
    }

    return true;
}

bool FILESYSTEM_saveTiXml2Document(const char *name, tinyxml2::XMLDocument& doc, bool sync /*= true*/)
{
    if (!isInit)
    {
        vlog_warn("Filesystem not initialized! Not writing just to be safe.");
        return false;
    }

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

#ifdef __EMSCRIPTEN__
    if (sync)
    {
        EM_ASM(FS.syncfs(false, function(err)
        {
            if (err)
            {
                console.warn("Error saving:", err);
                alert("Error saving. Check console for more information.");
            }
        }));
    }
#else
    UNUSED(sync);
#endif

    return true;
}

bool FILESYSTEM_loadTiXml2Document(const char *name, tinyxml2::XMLDocument& doc)
{
    /* XMLDocument.LoadFile doesn't account for Unicode paths, PHYSFS does */
    unsigned char *mem;
    FILESYSTEM_loadFileToMemory(name, &mem, NULL, true);
    if (mem == NULL)
    {
        return false;
    }
    doc.Parse((const char*) mem);
    VVV_free(mem);
    return true;
}

bool FILESYSTEM_loadAssetTiXml2Document(const char *name, tinyxml2::XMLDocument& doc)
{
    /* Same as FILESYSTEM_loadTiXml2Document except for possible custom assets */
    unsigned char *mem;
    FILESYSTEM_loadAssetToMemory(name, &mem, NULL, true);
    if (mem == NULL)
    {
        return false;
    }
    doc.Parse((const char*) mem);
    VVV_free(mem);
    return true;
}

struct CallbackWrapper
{
    void (*callback)(const char* filename);
};

static PHYSFS_EnumerateCallbackResult enumerateCallback(
    void* data,
    const char* origdir,
    const char* filename
) {
    struct CallbackWrapper* wrapper = (struct CallbackWrapper*) data;
    void (*callback)(const char*) = wrapper->callback;
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
    int success;
    struct CallbackWrapper wrapper = {callback};

    success = PHYSFS_enumerate("levels", enumerateCallback, (void*) &wrapper);

    if (success == 0)
    {
        vlog_error(
            "Could not get list of levels: %s",
            PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())
        );
    }
}

std::vector<std::string> FILESYSTEM_getLanguageCodes(void)
{
    std::vector<std::string> list;
    char** fileList = PHYSFS_enumerateFiles("lang");
    char** item;

    for (item = fileList; *item != NULL; item++)
    {
        char fullName[128];
        SDL_snprintf(fullName, sizeof(fullName), "lang/%s", *item);

        if (FILESYSTEM_isDirectory(fullName) && *item[0] != '.')
        {
            list.push_back(*item);
        }
    }

    PHYSFS_freeList(fileList);

    return list;
}

static int PLATFORM_getOSDirectory(char* output, const size_t output_size)
{
#ifdef _WIN32
    /* This block is here for compatibility, do not touch it! */
    WCHAR utf16_path[MAX_PATH];
    HRESULT retcode = SHGetFolderPathW(
        NULL,
        CSIDL_PERSONAL,
        NULL,
        SHGFP_TYPE_CURRENT,
        utf16_path
    );
    int num_bytes;

    if (FAILED(retcode))
    {
        vlog_error(
            "Could not get OS directory: SHGetFolderPathW returned 0x%08x",
            retcode
        );
        return 0;
    }

    num_bytes = WideCharToMultiByte(
        CP_UTF8,
        0,
        utf16_path,
        -1,
        output,
        output_size,
        NULL,
        NULL
    );
    if (num_bytes == 0)
    {
        vlog_error(
            "Could not get OS directory: UTF-8 conversion failed with %d",
            GetLastError()
        );
        return 0;
    }

    SDL_strlcat(output, "\\VVVVVV\\", MAX_PATH);
    mkdir(output, 0777);
    return 1;
#else
    const char* prefDir = PHYSFS_getPrefDir("distractionware", "VVVVVV");
    if (prefDir == NULL)
    {
        vlog_error(
            "Could not get OS directory: %s",
            PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())
        );
        return 0;
    }
    SDL_strlcpy(output, prefDir, output_size);
    return 1;
#endif
}

bool FILESYSTEM_openDirectoryEnabled(void)
{
    return !gameScreen.isForcedFullscreen();
}

#if defined(__EMSCRIPTEN__)
bool FILESYSTEM_openDirectory(const char *dname)
{
    return false;
}
#else
bool FILESYSTEM_openDirectory(const char *dname)
{
    char url[MAX_PATH];
    SDL_snprintf(url, sizeof(url), "file://%s", dname);
    if (SDL_OpenURL(url) == -1)
    {
        vlog_error("Error opening directory: %s", SDL_GetError());
        return false;
    }
    return true;
}
#endif

bool FILESYSTEM_delete(const char *name)
{
    return PHYSFS_delete(name) != 0;
}

static void levelSaveCallback(const char* filename)
{
    if (endsWith(filename, ".vvvvvv.vvv"))
    {
        if (!FILESYSTEM_delete(filename))
        {
            vlog_error("Error deleting %s", filename);
        }
    }
}

void FILESYSTEM_deleteLevelSaves(void)
{
    int success;
    struct CallbackWrapper wrapper = {levelSaveCallback};

    success = PHYSFS_enumerate(
        "saves",
        enumerateCallback,
        (void*) &wrapper
    );

    if (success == 0)
    {
        vlog_error(
            "Could not enumerate saves/: %s",
            PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())
        );
    }
}
