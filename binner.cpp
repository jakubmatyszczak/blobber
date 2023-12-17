#include <cstring>
#include <dirent.h>
#include <fstream>
#include <iostream>

#include "common.hpp"

struct file
{
	bool ok;
	std::string filename;
	std::string extenxion;
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
	char filename[headerFilenameSize] = {};
	strcpy(filename, basename(filepath.c_str()));
	strip_ext(filename);

	file f;
	f.extenxion = std::string(get_filename_ext(filepath.c_str()));
	f.filename = std::string(filename);
	f.size = ifile.tellg();
	f.data = calloc(f.size, 1);
	ifile.seekg(0);
	ifile.read((char*)f.data, f.size);
	ifile.close();
	f.ok = true;
	return f;
}
bool appendBin(std::ofstream& of, const file& f)
{
	if (!of.is_open())
		return false;
	header h = {};
	strcpy(h.name, f.filename.c_str());
	strcpy(h.extension, f.extenxion.c_str());
	h.size = f.size;
	of.write((const char*)&h, sizeof(h));
	of.write((const char*)f.data, f.size);
	return false;
}
bool writeBlobHeader(std::ofstream& of, const blobHeader& bh)
{
	of.write((char*)&bh.nElements, sizeof(bh.nElements));
	for (int i = 0; i < bh.nElements; i++)
	{
		char entry[headerFilenameSize] = {};
		strcpy(entry, bh.elements[i].c_str());
		of.write(entry, headerFilenameSize);
	}
	return true;
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
	{
		std::cout << filepaths[i] << std::endl;
	}

	blobHeader bh;
	bh.nElements = filepaths.size();
	bh.elements = filepaths;
	long totalsize = sizeof(bh.nElements) + bh.nElements * headerFilenameSize;

	std::ofstream ofile;
	ofile.open("blob.blob");
	writeBlobHeader(ofile, bh);
	for (int i = 0; i < bh.nElements; i++)
	{
		file f = openFile(filepaths[i]);
		appendBin(ofile, f);
		free(f.data);
		totalsize += f.size;
	}
	ofile.close();
	std::cout << "Total files: " << filepaths.size() << ", total size: " << totalsize << " ("
			  << totalsize / 1024 << " kB)." << std::endl;
	return 0;
}
