#include "BinaryBlob.h"

#include <physfs.h> /* FIXME: Abstract to FileSystemUtils! */
#include <SDL.h>
#include <stdio.h>

#include "Exit.h"
#include "UtilityClass.h"

binaryBlob::binaryBlob()
{
	numberofHeaders = 0;
	SDL_zeroa(m_headers);
	SDL_zeroa(m_memblocks);
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

		memblock = (char*) SDL_malloc(size);
		if (memblock == NULL)
		{
			VVV_exit(1);
		}
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
		fwrite((char*) &m_headers, 1, sizeof(m_headers), file);

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

	PHYSFS_readBytes(handle, &m_headers, sizeof(m_headers));

	int offset = 0 + (sizeof(m_headers));

	for (size_t i = 0; i < SDL_arraysize(m_headers); i += 1)
	{
		/* Name can be stupid, just needs to be terminated */
		m_headers[i].name[47] = '\0';

		if (m_headers[i].valid & ~0x1 || !m_headers[i].valid)
		{
			m_headers[i].valid = false;
			continue; /* Must be EXACTLY 1 or 0 */
		}
		if (m_headers[i].size < 1)
		{
			m_headers[i].valid = false;
			continue; /* Must be nonzero and positive */
		}
		if ((offset + m_headers[i].size) > size)
		{
			m_headers[i].valid = false;
			continue; /* Bogus size value */
		}

		PHYSFS_seek(handle, offset);
		m_memblocks[i] = (char*) SDL_malloc(m_headers[i].size);
		if (m_memblocks[i] == NULL)
		{
			VVV_exit(1); /* Oh god we're out of memory, just bail */
		}
		PHYSFS_readBytes(handle, m_memblocks[i], m_headers[i].size);
		offset += m_headers[i].size;
	}
	PHYSFS_close(handle);

	printf("The complete reloaded file size: %lli\n", size);

	for (size_t i = 0; i < SDL_arraysize(m_headers); i += 1)
	{
		if (m_headers[i].valid == false)
		{
			break;
		}

		printf("%s unpacked\n", m_headers[i].name);
	}

	return true;
}

void binaryBlob::clear()
{
	for (size_t i = 0; i < SDL_arraysize(m_headers); i += 1)
	{
		SDL_free(m_memblocks[i]);
	}
	SDL_zeroa(m_memblocks);
	SDL_zeroa(m_headers);
}

int binaryBlob::getIndex(const char* _name)
{
	for (size_t i = 0; i < SDL_arraysize(m_headers); i += 1)
	{
		if (SDL_strcmp(_name, m_headers[i].name) == 0 && m_headers[i].valid)
		{
			return i;
		}
	}
	return -1;
}

int binaryBlob::getSize(int _index)
{
	if (!INBOUNDS_ARR(_index, m_headers))
	{
		puts("getSize() out-of-bounds!");
		return 0;
	}
	return m_headers[_index].size;
}

char* binaryBlob::getAddress(int _index)
{
	if (!INBOUNDS_ARR(_index, m_memblocks))
	{
		puts("getAddress() out-of-bounds!");
		return NULL;
	}
	return m_memblocks[_index];
}

std::vector<int> binaryBlob::getExtra()
{
	std::vector<int> result;
	for (size_t i = 0; i < SDL_arraysize(m_headers); i += 1)
	{
		if (m_headers[i].valid
#define FOREACH_TRACK(_, track_name) && SDL_strcmp(m_headers[i].name, track_name) != 0
		TRACK_NAMES(_)
#undef FOREACH_TRACK
		) {
			result.push_back(i);
		}
	}
	return result;
}
