#ifndef PIXELFORMATTER_H
#define PIXELFORMATTER_H

#include "definitions.h"

class PixelFormatter {
    public:
        static void convertPixelData(unsigned char **rowPointers, int width, int height, int bitDepth, bool hasAlpha, rgb *destinationRgbValues);
    
    private:
        static char toHexNib(int decimal);
};

#endif