#include "PixelFormatter.h"

#include <math.h>
#include <iostream>

using namespace std;

char PixelFormatter::toHexNib(int decimal) {
	switch(decimal) {
		case 0:
			return '0';
		case 1:
			return '1';
		case 2:
			return '2';
		case 3:
			return '3';
		case 4:
			return '4';
		case 5:
			return '5';
		case 6:
			return '6';
		case 7:
			return '7';
		case 8:
			return '8';
		case 9:
			return '9';
		case 10:
			return 'a';
		case 11:
			return 'b';
		case 12:
			return 'c';
		case 13:
			return 'd';
		case 14:
			return 'e';
		case 15:
			return 'f';
		default:
			return (char)NULL;
	}
}

void PixelFormatter::convertPixelData(unsigned char **rowPointers, int width, int height, int bitDepth, bool hasAlpha, rgb *rgbValues) {

	// fill up the rgbValues array
	for(int iHeight = 0; iHeight < height; ++iHeight) {
		unsigned char* row = rowPointers[iHeight];
		for (int jWidth = 0; jWidth < width; jWidth++) {
			unsigned char* ptr = hasAlpha ? &(row[jWidth*4]) : &(row[jWidth*3]);
			int rgbValuesPosition = iHeight*width + jWidth;
			int maxval = pow(2, bitDepth);
			int max8BitValue = 31;

			//first the red
			double colorValue = ptr[0];
			double calculatedValue = (colorValue/maxval)*max8BitValue;
			rgbValues[rgbValuesPosition].r = new bitset<5>(calculatedValue);
			
			// now for the green
			colorValue = ptr[1];		
			calculatedValue = (colorValue/maxval)*max8BitValue;
			rgbValues[rgbValuesPosition].g = new bitset<5>(calculatedValue);
			
			// and, finally, the blue
			colorValue = ptr[2];		
			calculatedValue = (colorValue/maxval)*max8BitValue;
			rgbValues[rgbValuesPosition].b = new bitset<5>(calculatedValue);

			// OH! don't forget the alpha
			if(hasAlpha) {
				rgbValues[rgbValuesPosition].a = ptr[3] == 255? new bitset<1>(1) : new bitset<1>(0);
			} else {
				rgbValues[rgbValuesPosition].a = new bitset<1>(1);
			}

			// now make the sample bitset
			string redStr = (*rgbValues[rgbValuesPosition].r).to_string();
			string greenStr = (*rgbValues[rgbValuesPosition].g).to_string();
			string blueStr = (*rgbValues[rgbValuesPosition].b).to_string();
			string alphaStr = (*rgbValues[rgbValuesPosition].a).to_string();
			rgbValues[rgbValuesPosition].sample = new bitset<16>(redStr+greenStr+blueStr+alphaStr);
			// if(ptr[3] == 0) rgbValues[rgbValuesPosition].sample = new bitset<16>(0xFFFE);

			// now for the final steps in conversion
			// so far we have a 16bit sample value that needs broken up into hex
			int first, second, third, fourth;
			first = ((*rgbValues[rgbValuesPosition].sample)>>12).to_ulong();
			second = (((*rgbValues[rgbValuesPosition].sample)<<4)>>12).to_ulong();
			third = (((*rgbValues[rgbValuesPosition].sample)<<8)>>12).to_ulong();
			fourth = (((*rgbValues[rgbValuesPosition].sample)<<12)>>12).to_ulong();

			rgbValues[rgbValuesPosition].n64Format = new char[4];
			rgbValues[rgbValuesPosition].n64Format[0] = toHexNib(first);
			rgbValues[rgbValuesPosition].n64Format[1] = toHexNib(second);
			rgbValues[rgbValuesPosition].n64Format[2] = toHexNib(third);
			rgbValues[rgbValuesPosition].n64Format[3] = toHexNib(fourth);

			// if(iHeight == 9) {
				// if(jWidth == 68) {
					// cout << ptr[0]/1 << " " << ptr[1]/1 << " " << ptr[2]/1 << " " << ptr[3]/1 << endl;
					// cout << rgbValues[rgbValuesPosition].n64Format << endl;
				// }
			// }

			// these values will no longer be needed
			delete rgbValues[rgbValuesPosition].r;
			delete rgbValues[rgbValuesPosition].g;
			delete rgbValues[rgbValuesPosition].b;
			delete rgbValues[rgbValuesPosition].a;
			delete rgbValues[rgbValuesPosition].sample;
		}
	}
}
