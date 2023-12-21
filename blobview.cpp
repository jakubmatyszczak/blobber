#include "blobber.hpp"
#include <iostream>

using namespace blob;

int main(int argc, const char* argv[])
{
	BlobReader b;
	if (argc < 2)
		return 1;
	b.readFile(argv[1]);
	std::cout << "Blob file: " << argv[1] << std::endl;
	std::cout << "Max Path length: " << b.bh.pathLen << std::endl;
	std::cout << "Found " << b.bh.nElements << " headers." << std::endl;
	for (BlobEntryHeader& h : b.headers)
		std::cout << h.path << " - " << h.datasize << " - " << h.offset << std::endl;
	return 0;
}
