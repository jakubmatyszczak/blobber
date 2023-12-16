#pragma once 

#include <string>
#include <vector>

constexpr long headerFilenameSize = 64;
struct header
{
	char name[64];
	char extension[16];
	long size;
};
struct blobHeader
{
	long nElements;
	std::vector<std::string> elements;
};
