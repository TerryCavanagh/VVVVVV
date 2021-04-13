#include "BinaryBlob.h"

#include <SDL.h>
#include <stdio.h>

#include "Exit.h"
#include "FileSystemUtils.h"
#include "UtilityClass.h"

binaryBlob::binaryBlob(void)
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
	return FILESYSTEM_loadBinaryBlob(this, name);
}

void binaryBlob::clear(void)
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

bool binaryBlob::nextExtra(size_t* start)
{
	size_t* idx;

	if (start == NULL)
	{
		return false;
	}

	for (idx = start; *idx < SDL_arraysize(m_headers); *idx += 1)
	{
		if (m_headers[*idx].valid
#define FOREACH_TRACK(_, track_name) && SDL_strcmp(m_headers[*idx].name, track_name) != 0
		TRACK_NAMES(_)
#undef FOREACH_TRACK
		) {
			return true;
		}
	}
	return false;
}
