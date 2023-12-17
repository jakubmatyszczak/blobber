#pragma once

#include <cstring>
#include <fstream>
#include <string>
#include <vector>
namespace blob
{
	constexpr long headerFilenameSize = 64;
	struct BlobEntryHeader
	{
		char name[64];
		char extension[16];
		long datasize;
		long offset;
	};
	struct BlobHeader
	{
		long nElements;
		std::vector<std::string> elements;
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

	  public:
		char* data;
		long marker = 0;
		BlobHeader bh;
		std::vector<BlobEntryHeader> headers;
		~BlobReader() { unload(); }
		bool read(const std::string& filepath)
		{
			std::ifstream ifile;
			ifile.open(filepath.c_str(), std::ios::binary | std::ios::ate);
			if (!ifile.is_open())
				return false;
			long fsize = ifile.tellg();
			data = (char*)calloc(ifile.tellg(), 1);
			ifile.seekg(0);
			ifile.read((char*)data, fsize);
			ifile.close();

			bh.nElements = dataread();
			for (int i = 0; i < bh.nElements; i++)
			{
				char entry[headerFilenameSize] = {};
				dataread(entry, headerFilenameSize);
				bh.elements.emplace_back(entry);
			}
			for (int i = 0; i < bh.nElements; i++)
			{
				BlobEntryHeader h;
				dataread(&h, sizeof(h));
				headers.emplace_back(h);
				marker += h.datasize;
			}
			return true;
		}
		void unload()
		{
			if (data != nullptr)
				free(data);
		}
        void* getAsset(const std::string& assetName)
        {
            for(BlobEntryHeader& h : headers)
                if(strcmp(h.name, assetName.c_str()) == 0)
                    return (void*)(&data[h.offset]);
            return nullptr;
        }

	};
} // namespace blob
