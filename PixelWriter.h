#ifndef PIXELWRITER_H
#define PIXELWRITER_H

#include "definitions.h"

#include <fstream>

using namespace std;

class PixelWriter {
    private:
		string textureName;
		int textureWidth;
		int textureHeight;
		static const int fullScreenWidth = 320;
		static const int fullScreenHeight = 240;
		void writePixelRange(rgb *rgbValues, ofstream *outputFile, int subHeight, int subWidth, int originY, int originX);

    public:
		PixelWriter(string textureName, int textureWidth, int textureHeight);

		void writeFullScreenImage(rgb *rgbValues, ofstream *outputFile);
		void writeStandardImage(rgb *rgbValues, ofstream *outputFile);

		bool isFullScreenImage();
		void writeOutputFileHeader(ofstream &outputFile, bool fullRes);
};

#endif