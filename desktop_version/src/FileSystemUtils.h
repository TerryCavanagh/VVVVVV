#ifndef FILESYSTEMUTILS_H
#define FILESYSTEMUTILS_H

/* Forward declaration */
class binaryBlob;

#include <stddef.h>

// Forward declaration, including the entirety of tinyxml2.h across all files this file is included in is unnecessary
namespace tinyxml2 { class XMLDocument; }

int FILESYSTEM_init(char *argvZero, char* baseDir, char* assetsPath);
void FILESYSTEM_deinit(void);

char *FILESYSTEM_getUserSaveDirectory(void);
char *FILESYSTEM_getUserLevelDirectory(void);

bool FILESYSTEM_isFile(const char* filename);
bool FILESYSTEM_isMounted(const char* filename);

void FILESYSTEM_loadZip(const char* filename);
bool FILESYSTEM_mountAssets(const char *path);
void FILESYSTEM_unmountAssets(void);
bool FILESYSTEM_isAssetMounted(const char* filename);

void FILESYSTEM_loadFileToMemory(const char *name, unsigned char **mem,
                                 size_t *len, bool addnull);
void FILESYSTEM_loadAssetToMemory(
    const char* name,
    unsigned char** mem,
    size_t* len,
    const bool addnull
);
void FILESYSTEM_freeMemory(unsigned char **mem);

bool FILESYSTEM_loadBinaryBlob(binaryBlob* blob, const char* filename);

bool FILESYSTEM_saveTiXml2Document(const char *name, tinyxml2::XMLDocument& doc);
bool FILESYSTEM_loadTiXml2Document(const char *name, tinyxml2::XMLDocument& doc);

void FILESYSTEM_enumerateLevelDirFileNames(void (*callback)(const char* filename));

bool FILESYSTEM_levelDirHasError(void);
void FILESYSTEM_clearLevelDirError(void);
const char* FILESYSTEM_getLevelDirError(void);

bool FILESYSTEM_openDirectoryEnabled(void);
bool FILESYSTEM_openDirectory(const char *dname);

bool FILESYSTEM_delete(const char *name);

#endif /* FILESYSTEMUTILS_H */
