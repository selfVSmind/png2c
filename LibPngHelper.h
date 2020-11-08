#ifndef LIBPNGHELPER_H
#define LIBPNGHELPER_H

#include <png.h>
#include <string>

// only one instance at a time please
class LibPngHelper {
    public:
        png_byte colorType;
        png_byte bitDepth;
        png_structp pngPointer;
        png_infop infoPointer;
        png_bytep *rowPointers;
        int numberOfPasses;
        int width, height;

        LibPngHelper();
        std::string readInputPngFile(char* fileName);
        bool hasAlpha();
};

#endif