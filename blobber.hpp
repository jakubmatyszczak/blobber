#pragma once

#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
namespace blob
{
	constexpr long pathLen = 64;
	struct BlobEntryHeader
	{
		char path[pathLen];
		long datasize;
		long offset;
	};
	struct BlobHeader
	{
		long pathLen;
		long nElements;
	};
	class BlobReader
	{
	  private:
		long dataread()
		{
			long value = ((long*)data)[marker];
			marker += sizeof(long);
			return value;
		}
		void dataread(void* buffer, long size)
		{
			memcpy(buffer, &data[marker], size);
			marker += size;
		}
		bool init()
		{
			dataread(&bh, sizeof(bh));
			for (int i = 0; i < bh.nElements; i++)
			{
				BlobEntryHeader h;
				dataread(&h, sizeof(h));
				headers.emplace_back(h);
			}
			return true;
		}

	  public:
		char* data;
		long marker = 0;
		bool shouldFree = false;
		BlobHeader bh;
		std::vector<BlobEntryHeader> headers;
		~BlobReader() { unload(); }
		bool readFile(const std::string& filepath, void* mem = nullptr)
		{
			std::ifstream ifile;
			ifile.open(filepath.c_str(), std::ios::binary | std::ios::ate);
			if (!ifile.is_open())
				return false;
			long fsize = ifile.tellg();
			if (mem)
				data = (char*)mem;
			else
			{
				data = (char*)calloc(fsize, 1);
				shouldFree = true;
			}
			ifile.seekg(0);
			ifile.read((char*)data, fsize);
			ifile.close();
			return init();
		}
		bool readMemory(char* blobPtr)
		{
			data = blobPtr;
			return init();
		}
		void unload()
		{
			if (shouldFree)
				free(data);
		}
		void* getAsset(const std::string& assetName, long& datasize)
		{
			for (BlobEntryHeader& h : headers)
				if (strcmp(h.path, assetName.c_str()) == 0)
				{
					datasize = h.datasize;
					return (void*)(&data[h.offset]);
				}
			return nullptr;
		}
	};
} // namespace blob
