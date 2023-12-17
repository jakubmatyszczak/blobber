#include "common.hpp"
#include <iostream>

using namespace blob;

int main(int argc, const char* argv[])
{
	if (argc < 2)
		return 1;
    BlobReader b;
    b.read(argv[1]);
    std::cout << b.bh.nElements << std::endl;
    for(std::string& s : b.bh.elements)
        std::cout << "- " << s << std::endl;
    std::cout << "###########" << std::endl;
    for(BlobEntryHeader& h : b.headers)
    {
        std::cout << h.name << std::endl; 
        std::cout << h.extension << std::endl; 
        std::cout << h.datasize << std::endl; 
        std::cout << h.offset << std::endl; 
        std::cout << "---" << std::endl; 
    }
    return 0;
}
