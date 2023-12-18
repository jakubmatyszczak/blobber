### Blobber

Blobber is a small tools, to create binary blobs from directories containing varuius assets,
such as textures, images, sounds, text, other binarier etc. Blobbler will recursively accumulate all
regular files in directory (ignores symlinks, pipes etc.), into single `.blob` files that can be
then peered into with attached `blobview` app. 

The blobs can be eaisly accessed using BlobReader class (defined in header `blobber.hpp`).

Blobs can also be compiled directly into executable,to create single file program with assets,
using something like:
```
add_custom_command(OUTPUT res.o COMMAND ld -r -b binary -o res.o res.blob)
add_executable(${PROJECT_NAME} ${src} res.o)
```
The whole process can be automated, refer to CMakeLists.txt in https://github.com/jakubmatyszczak/timeR
for easy to follow and replicate example.

# Building
```
mkdir build
cd build
cmake ..
make
```

# Usage Blobbing
```
./blobber res
res/tex/dish.png
res/tex/earth.png
res/tex/station.png
res/tex/satellite.png
res/tex/select.png
res/tex/factory.png
res/tex/rocket.png
res/tex/rock.png
res/tex/station2.png
res/fonts/roboto.ttf
res/audio/pew.wav
res/audio/ambient.wav
Total files: 12, total size: 23836067 (23277 kB).
```
Where `res` is a directory containing assets for blobberification.

# Usage reading from .blob and from memory 
"not" is filename of the asset (pre-blobberification it was a file `res/not.mp3`).

Reading blob from file:
```
blob::BlobReader br;
br.readFile("res.blob");
long datasize;
void* ptr = br.getAsset("not", datasize);
```
Reading blob from memory (.blob integrated into executable):
```
extern char _binary_res_blob_start;     // section name read from res.o with readelf

blob::BlobReader br;
br.readMemory(&_binary_res_blob_start);
long datasize;
void* ptr = br.getAsset("not", datasize);
```

