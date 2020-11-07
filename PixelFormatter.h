#ifndef PIXELFORMATTER_H
#define PIXELFORMATTER_H

#include <bitset>

struct rgb {
	std::bitset<5> *r;
	std::bitset<5> *g;
	std::bitset<5> *b;
	std::bitset<1> *a;
	std::bitset<16> *sample;
	char *n64Format;
};

class PixelFormatter {
    public:
        static void convertPixelData(unsigned char **rowPointers, int width, int height, int bitDepth, bool hasAlpha, rgb *destinationRgbValues);
    
    private:
        static char toHexNib(int decimal);
};

#endif