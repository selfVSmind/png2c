#ifndef MYLIBPNGHELPER_H
#define MYLIBPNGHELPER_H

#include <png.h>
#include <string>

// this is self-instantiating because only one should exist at a time
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
} myLibPngHelper;

#endif