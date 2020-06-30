#ifndef FILESYSTEMUTILS_H
#define FILESYSTEMUTILS_H

#include <string>
#include <vector>

// Forward declaration, including the entirety of tinyxml2.h across all files this file is included in is unnecessary
namespace tinyxml2 { class XMLDocument; }

int FILESYSTEM_init(char *argvZero, char* baseDir, char* assetsPath);
void FILESYSTEM_deinit();

char *FILESYSTEM_getUserSaveDirectory();
char *FILESYSTEM_getUserLevelDirectory();

bool FILESYSTEM_directoryExists(const char *fname);
void FILESYSTEM_mount(const char *fname);
extern bool FILESYSTEM_assetsmounted;
void FILESYSTEM_mountassets(const char *path);
void FILESYSTEM_unmountassets();

void FILESYSTEM_loadFileToMemory(const char *name, unsigned char **mem,
                                 size_t *len, bool addnull = false);
void FILESYSTEM_freeMemory(unsigned char **mem);
bool FILESYSTEM_saveTiXml2Document(const char *name, tinyxml2::XMLDocument& doc);
bool FILESYSTEM_loadTiXml2Document(const char *name, tinyxml2::XMLDocument& doc);

std::vector<std::string> FILESYSTEM_getLevelDirFileNames();

bool FILESYSTEM_openDirectoryEnabled();
bool FILESYSTEM_openDirectory(const char *dname);

bool FILESYSTEM_delete(const char *name);

#endif /* FILESYSTEMUTILS_H */
