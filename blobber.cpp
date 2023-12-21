#include <dirent.h>
#include <iostream>

#include "blobber.hpp"
using namespace blob;
struct file
{
	bool ok;
    char filepath[pathLen];
	long size;
	void* data;
};
void strip_ext(char* fname)
{
	char* end = fname + strlen(fname);
	while (end > fname && *end != '.')
		--end;
	if (end > fname)
		*end = '\0';
}
const char* get_filename_ext(const char* filename)
{
	const char* dot = strrchr(filename, '.');
	if (!dot || dot == filename)
		return "";
	return dot + 1;
}

file openFile(std::string filepath)
{
	std::ifstream ifile;
	ifile.open(filepath.c_str(), std::ios::binary | std::ios::ate);
	if (!ifile.good())
	{
		std::cerr << "Failed to open file!" << std::endl;
		return {};
	}
	file f;
    strcpy(f.filepath, filepath.c_str());
	f.size = ifile.tellg();
	f.data = calloc(f.size, 1);
	ifile.seekg(0);
	ifile.read((char*)f.data, f.size);
	ifile.close();
	f.ok = true;
	return f;
}
long writeBlobHeader(std::ofstream& of, const BlobHeader& bh)
{
    long bytes = 0;
	of.write((char*)&bh.nElements, sizeof(bh.nElements));
    bytes += sizeof(bh.nElements);
	for (int i = 0; i < bh.nElements; i++)
	{
		// char entry[headerFilenameSize] = {};
		// strcpy(entry, bh.elements[i].c_str());
		// of.write(entry, headerFilenameSize);
  //       bytes += sizeof(headerFilenameSize);
	}
	return bytes;
}
std::string assemblePath(std::vector<std::string>& path)
{
	std::string pathString;
	for (std::string& s : path)
		pathString += s + std::string("/");
	return pathString;
}

void scanFolder(const char* dirname, std::vector<std::string>& filepaths, std::vector<std::string>& path)
{
	DIR* d;
	struct dirent* dir;
	d = opendir(std::string(assemblePath(path) + std::string(dirname)).c_str());
	if (d)
	{
		path.emplace_back(dirname);
		while ((dir = readdir(d)) != NULL)
		{
			if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
				continue;
			if (dir->d_type == DT_REG)
			{
				filepaths.emplace_back(assemblePath(path) + std::string(dir->d_name));
				continue;
			}
			if (dir->d_type == DT_DIR)
				scanFolder(dir->d_name, filepaths, path);
		}
		closedir(d);
		path.pop_back();
	}
}
int main(int argc, const char* argv[])
{
	if (argc < 2)
		return 1;
	const char* dirname = argv[1];
	std::vector<std::string> filepaths;
	std::vector<std::string> path;

	scanFolder(dirname, filepaths, path);
	for (int i = 0; i < filepaths.size(); i++)
		std::cout << filepaths[i] << std::endl;


	BlobHeader bh;
    bh.pathLen = pathLen;
	bh.nElements = filepaths.size();
    BlobEntryHeader* headers = (BlobEntryHeader*)calloc(bh.nElements, sizeof(BlobEntryHeader));
    long headerSize = sizeof(BlobEntryHeader) * bh.nElements;

	std::ofstream ofile;
	ofile.open(std::string(dirname) + ".blob");
    ofile.write((const char*)&bh, sizeof(bh));
    ofile.write((const char*) headers, headerSize);
    long bytesWritten = sizeof(bh) + headerSize;
	for (int i = 0; i < bh.nElements; i++)
	{
		file f = openFile(filepaths[i]);
        strcpy(headers[i].path, filepaths[i].c_str()); 
        headers[i].datasize = f.size;
        headers[i].offset = bytesWritten;
        ofile.write((const char*)f.data, f.size);
        bytesWritten += f.size;
		free(f.data);
	}
    ofile.seekp(0);
    ofile.write((const char*)&bh, sizeof(bh));
    ofile.write((const char*)headers, headerSize);
	ofile.close();
	std::cout << "Total files: " << filepaths.size() << ", total size: " << bytesWritten << " ("
			  << bytesWritten / 1024 << " kB)." << std::endl;
	return 0;
}
