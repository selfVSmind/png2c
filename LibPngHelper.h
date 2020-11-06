#ifndef MYLIBPNGHELPER_H
#define MYLIBPNGHELPER_H

#include <png.h>
#include <string>

class LibPngHelper {
    public:
        png_byte colorType;
        png_byte bitDepth;
        png_structp pngPointer;
        png_infop infoPointer;
        png_bytep *row_pointers;
        int number_of_passes;
        int width, height;

        LibPngHelper();
        std::string readInputPngFile(char* fileName);
};

#endif