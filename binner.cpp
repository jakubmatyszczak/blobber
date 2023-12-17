#include <cstring>
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
int main()
{
	blobHeader bh;
	bh.nElements = 10;
	for (int i = 0; i < 10; i++)
		bh.elements.emplace_back("asdf.txt");

	std::ofstream ofile;
	ofile.open("blob.bin");
	writeBlobHeader(ofile, bh);
	for (int i = 0; i < 10; i++)
	{
		file f = openFile("1.txt");
		appendBin(ofile, f);
		free(f.data);
	}
	ofile.close();

	return 0;
}
