#include "BinaryBlob.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FIXME: Abstract to FileSystemUtils! */
#include <physfs.h>

binaryBlob::binaryBlob()
{
	numberofHeaders = 0;
	for (int i = 0; i < 128; i += 1)
	{
		m_headers[i].valid = false;

		for (int j = 0; j < 48; j += 1)
		{
			m_headers[i].name[j] = '\0';
		}
	}
	::memset(m_headers, 0, 128 * sizeof(resourceheader));
}

#ifdef VVV_COMPILEMUSIC
void binaryBlob::AddFileToBinaryBlob(const char* _path)
{
	long size;
	char * memblock;

	FILE *file = fopen(_path, "rb");
	if (file != NULL)
	{
		fseek(file, 0, SEEK_END);
		size = ftell(file);
		fseek(file, 0, SEEK_SET);

		memblock = (char*) malloc(size);
		fread(memblock, 1, size, file);

		fclose(file);

		printf("The complete file size: %li\n", size);

		m_memblocks[numberofHeaders] = memblock;
		for (int i = 0; _path[i]; i += 1)
		{
			m_headers[numberofHeaders].name[i] = _path[i];
		}

		m_headers[numberofHeaders].valid = true;
		m_headers[numberofHeaders].size = size;
		numberofHeaders += 1;
	}
	else
	{
		printf("Unable to open file\n");
	}
}

void binaryBlob::writeBinaryBlob(const char* _name)
{
	FILE *file = fopen(_name, "wb");
	if (file != NULL)
	{
		fwrite((char*) &m_headers, 1, sizeof(resourceheader) * 128, file);

		for (int i = 0; i < numberofHeaders; i += 1)
		{
			fwrite(m_memblocks[i], 1, m_headers[i].size, file);
		}

		fclose(file);
	}
	else
	{
		printf("Unable to open new file for writing. Feels bad.\n");
	}
}
#endif

bool binaryBlob::unPackBinary(const char* name)
{
	PHYSFS_sint64 size;

	PHYSFS_File *handle = PHYSFS_openRead(name);
	if (handle == NULL)
	{
		printf("Unable to open file %s\n", name);
		return false;
	}

	size = PHYSFS_fileLength(handle);

	PHYSFS_readBytes(handle, &m_headers, sizeof(resourceheader) * 128);

	int offset = 0 + (sizeof(resourceheader) * 128);

	for (int i = 0; i < 128; i += 1)
	{
		if (m_headers[i].valid)
		{
			PHYSFS_seek(handle, offset);
			m_memblocks[i] = (char*) malloc(m_headers[i].size);
			PHYSFS_readBytes(handle, m_memblocks[i], m_headers[i].size);
			offset += m_headers[i].size;
		}
	}
	PHYSFS_close(handle);

	printf("The complete reloaded file size: %lli\n", size);

	for (int i = 0; i < 128; i += 1)
	{
		if (m_headers[i].valid == false)
		{
			break;
		}

		printf("%s unpacked\n", m_headers[i].name);
	}

	return true;
}

int binaryBlob::getIndex(const char* _name)
{
	for (int i = 0; i < 128; i += 1)
	{
		if (strcmp(_name, m_headers[i].name) == 0)
		{
			return i;
		}
	}
	return -1;
}

int binaryBlob::getSize(int _index)
{
	return m_headers[_index].size;
}

char* binaryBlob::getAddress(int _index)
{
	return m_memblocks[_index];
}
