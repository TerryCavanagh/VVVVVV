#ifndef FILESYSTEMUTILS_H
#define FILESYSTEMUTILS_H

#include <string>
#include <vector>

#include "tinyxml.h"

int FILESYSTEM_init(char *argvZero, char* baseDir, char* assetsPath);
void FILESYSTEM_deinit();

char *FILESYSTEM_getUserSaveDirectory();
char *FILESYSTEM_getUserLevelDirectory();

void FILESYSTEM_loadFileToMemory(const char *name, unsigned char **mem,
                                 size_t *len, bool addnull = false);
void FILESYSTEM_freeMemory(unsigned char **mem);
bool FILESYSTEM_saveTiXmlDocument(const char *name, TiXmlDocument *doc);
bool FILESYSTEM_loadTiXmlDocument(const char *name, TiXmlDocument *doc);

std::vector<std::string> FILESYSTEM_getLevelDirFileNames();

bool FILESYSTEM_openDirectoryEnabled();
bool FILESYSTEM_openDirectory(const char *dname);

bool FILESYSTEM_delete(const char *name);

#endif /* FILESYSTEMUTILS_H */
