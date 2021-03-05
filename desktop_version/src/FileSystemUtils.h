#ifndef FILESYSTEMUTILS_H
#define FILESYSTEMUTILS_H

#include <stddef.h>

// Forward declaration, including the entirety of tinyxml2.h across all files this file is included in is unnecessary
namespace tinyxml2 { class XMLDocument; }

int FILESYSTEM_init(char *argvZero, char* baseDir, char* assetsPath);
void FILESYSTEM_deinit(void);

char *FILESYSTEM_getUserSaveDirectory(void);
char *FILESYSTEM_getUserLevelDirectory(void);

void FILESYSTEM_mount(const char *fname);
void FILESYSTEM_loadZip(const char* filename);
extern bool FILESYSTEM_assetsmounted;
void FILESYSTEM_mountassets(const char *path);
void FILESYSTEM_unmountassets(void);

void FILESYSTEM_loadFileToMemory(const char *name, unsigned char **mem,
                                 size_t *len, bool addnull = false);
void FILESYSTEM_freeMemory(unsigned char **mem);
bool FILESYSTEM_saveTiXml2Document(const char *name, tinyxml2::XMLDocument& doc);
bool FILESYSTEM_loadTiXml2Document(const char *name, tinyxml2::XMLDocument& doc);

void FILESYSTEM_enumerateLevelDirFileNames(void (*callback)(const char* filename));

bool FILESYSTEM_openDirectoryEnabled(void);
bool FILESYSTEM_openDirectory(const char *dname);

bool FILESYSTEM_delete(const char *name);

#endif /* FILESYSTEMUTILS_H */
