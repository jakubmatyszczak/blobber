#include <fstream>
#include <iostream>
#include "common.hpp"

int main(int argc, const char* argv[])
{
    if(argc < 2)    
        return 1;
    std::ifstream ifile;
    ifile.open(argv[1], std::ios::binary | std::ios::in);
    if(!ifile.is_open())
        return 1;

    blobHeader bh;
    ifile.read((char*)&bh.nElements, sizeof(bh.nElements));
    for(int i = 0; i < bh.nElements; i++)
    {
        char entry[headerFilenameSize] = {};
        ifile.read(entry, headerFilenameSize);
        bh.elements.emplace_back(entry);
    }
    std::cout << "Binfile [" << argv[1] << "] contains " << bh.nElements << " entries:" << std::endl;
    for(int i = 0; i < bh.nElements; i++)
        std::cout << " - "<< bh.elements[i] << std::endl;

    header h;
    void* data;
    ifile.read((char*)&h, sizeof(h));
    data = calloc(h.size, 1);
    ifile.read((char*)data, h.size);
    std::cout << "first entry:" << std::endl;
    std::cout << "name: " << h.name << std::endl;
    std::cout << "ext: " << h.extension << std::endl;
    std::cout << "size: " << h.size << std::endl;
    std::cout << "data: " << std::endl << (char*)data << std::endl;

    ifile.close();
    return 0;
}

