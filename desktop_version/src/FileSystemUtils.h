#ifndef FILESYSTEMUTILS_H
#define FILESYSTEMUTILS_H

/* Forward declaration */
class binaryBlob;

#include <stddef.h>

// Forward declaration, including the entirety of tinyxml2.h across all files this file is included in is unnecessary
namespace tinyxml2 { class XMLDocument; }

int FILESYSTEM_init(char *argvZero, char* baseDir, char* assetsPath, char* langDir, char* fontsDir);
bool FILESYSTEM_isInit(void);
void FILESYSTEM_deinit(void);

char *FILESYSTEM_getUserSaveDirectory(void);
char *FILESYSTEM_getUserLevelDirectory(void);
char *FILESYSTEM_getUserMainLangDirectory(void);
bool FILESYSTEM_isMainLangDirFromRepo(void);
bool FILESYSTEM_doesLangDirExist(void);
bool FILESYSTEM_doesFontsDirExist(void);

bool FILESYSTEM_setLangWriteDir(void);
bool FILESYSTEM_restoreWriteDir(void);

bool FILESYSTEM_isFile(const char* filename);
bool FILESYSTEM_isMounted(const char* filename);

void FILESYSTEM_loadZip(const char* filename);
bool FILESYSTEM_mountAssets(const char *path);
void FILESYSTEM_unmountAssets(void);
bool FILESYSTEM_isAssetMounted(const char* filename);
bool FILESYSTEM_areAssetsInSameRealDir(const char* filenameA, const char* filenameB);

void FILESYSTEM_loadFileToMemory(const char *name, unsigned char **mem,
                                 size_t *len);
void FILESYSTEM_loadAssetToMemory(
    const char* name,
    unsigned char** mem,
    size_t* len
);

bool FILESYSTEM_loadBinaryBlob(binaryBlob* blob, const char* filename);

bool FILESYSTEM_saveTiXml2Document(const char *name, tinyxml2::XMLDocument& doc, bool sync = true);
bool FILESYSTEM_loadTiXml2Document(const char *name, tinyxml2::XMLDocument& doc);
bool FILESYSTEM_loadAssetTiXml2Document(const char *name, tinyxml2::XMLDocument& doc);

void FILESYSTEM_enumerateLevelDirFileNames(void (*callback)(const char* filename));

struct EnumHandle
{
    char** physfs_list;
    char** _item;
};

const char* FILESYSTEM_enumerate(const char* folder, EnumHandle* handle);
const char* FILESYSTEM_enumerateAssets(const char* folder, EnumHandle* handle);
const char* FILESYSTEM_enumerateLanguageCodes(EnumHandle* handle);
void FILESYSTEM_freeEnumerate(EnumHandle* handle);

bool FILESYSTEM_levelDirHasError(void);
void FILESYSTEM_clearLevelDirError(void);
const char* FILESYSTEM_getLevelDirError(void);
void FILESYSTEM_setLevelDirError(const char* text, const char* args_index, ...);

bool FILESYSTEM_openDirectoryEnabled(void);
bool FILESYSTEM_openDirectory(const char *dname);

bool FILESYSTEM_delete(const char *name);
void FILESYSTEM_deleteLevelSaves(void);

#endif /* FILESYSTEMUTILS_H */
