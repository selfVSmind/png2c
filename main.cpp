#define cimg_use_png

#include "CImg.h"
using namespace cimg_library;

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <bitset>

using namespace std;

struct rgb {
	std::bitset<5> *r;
	std::bitset<5> *g;
	std::bitset<5> *b;
	std::bitset<1> *a;
	std::bitset<16> *sample;
	char *n64Format;
};

char *buffer;
rgb *rgbValues;

char toHexNib(int decimal) {
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
			return 'z';
	}
}

int main(int argc, char *argv[]) {
	if ( argc != 2 ) {
	    // We print argv[0] assuming it is the program name
		cout<<"usage: "<< argv[0] <<" <filename>\n";
		return 1;
	} // argc should be 2 for correct execution


	//ifstream inputFile("brick.ppm", ios::in | ios::binary);

	ofstream debugFile("debug.txt", ios::out);



	CImg<unsigned char> texture(argv[1]);
	int width = texture.width();
	int height = texture.height();
	int maxval = texture.max();
	int numBytes;
//	int width, height, numBytes;
//	double maxval;
	
	//grabbing the 'P6' (hopefully)
	//inputFile >> current;
	//cout << current;
	//inputFile >> current;
	//cout << current << endl;

	//grab the width
	//inputFile >> width;
	cout << "Width = " << width << endl;
	
	//grab the height
	//inputFile >> height;
	cout << "Height = " << height << endl;

	//grab the maximum color value
	//inputFile >> maxval;
	cout << "The maximum color value = " << maxval << endl;
	
	//the number of bytes per sample
	numBytes = (maxval > 255) ? 2 : 1;
	
	//w x h x (rgb) x (how large of a sample)
	int sizeOfBuffer = width*height*3*numBytes;
	
	//advance past the whitespace character after MAXVAL
	//inputFile.ignore();

	//allocate some buffer space and read from the file
	buffer = new char[sizeOfBuffer];
	//inputFile.read(buffer, sizeOfBuffer);
	
	//close the input file
	//inputFile.close();

	//allocate rgbValues array
	int rgbValuesSize = width*height;
	rgbValues = new rgb[rgbValuesSize];

	//fill up the rgbValues array
	//WARNING: this logic only handles cases where (maxval<255) !!
	for(int iHeight = 0; iHeight < height; ++iHeight) {
		for(int jWidth = 0; jWidth < width*3; jWidth +=3) {
			//unused variable     int bufferPosition = iHeight*width*3 + jWidth;
			int rgbValuesPosition = iHeight*width + jWidth/3;
			int max8BitValue = 31;

			//first the red
			double colorValue = texture(jWidth/3, iHeight, 0, 0);
			double calculatedValue = (colorValue/maxval)*max8BitValue;
			rgbValues[rgbValuesPosition].r = new bitset<5>((int)calculatedValue);
			
			//now for the green
			colorValue = texture(jWidth/3, iHeight, 0, 1);
			calculatedValue = (colorValue/maxval)*max8BitValue;
			rgbValues[rgbValuesPosition].g = new bitset<5>((int)calculatedValue);
			
			//and, finally, the blue
			colorValue = texture(jWidth/3, iHeight, 0, 2);
			calculatedValue = (colorValue/maxval)*max8BitValue;
			rgbValues[rgbValuesPosition].b = new bitset<5>((int)calculatedValue);

			//OH! don't forget the alpha
			rgbValues[rgbValuesPosition].a = new bitset<1>(1);

			//now make the sample bitset
			string redStr = (*rgbValues[rgbValuesPosition].r).to_string();
			string greenStr = (*rgbValues[rgbValuesPosition].g).to_string();
			string blueStr = (*rgbValues[rgbValuesPosition].b).to_string();
			string alphaStr = (*rgbValues[rgbValuesPosition].a).to_string();
			rgbValues[rgbValuesPosition].sample = new bitset<16>(redStr+greenStr+blueStr+alphaStr);
			debugFile << rgbValues[rgbValuesPosition].sample << endl;

			//now for the final steps in conversion
			//so far we have a 16bit sample value that needs broken up into hex
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

			//clean shit up!
			delete rgbValues[rgbValuesPosition].r;
			delete rgbValues[rgbValuesPosition].g;
			delete rgbValues[rgbValuesPosition].b;
			delete rgbValues[rgbValuesPosition].a;
			delete rgbValues[rgbValuesPosition].sample;
		}
	}

	delete buffer;

	int test = 0;
/*	cout << *rgbValues[test].r << endl;
	cout << *rgbValues[test].g << endl;
	cout << *rgbValues[test].b << endl;
	cout << *rgbValues[test].a << endl;
	cout << *rgbValues[test].sample << endl;
*/	cout << rgbValues[test].n64Format << endl;

/*	cout << ((*rgbValues[test].sample)>>12).to_ulong() << endl;
	cout << (((*rgbValues[test].sample)<<4)>>12).to_ulong() << endl;
	cout << (((*rgbValues[test].sample)<<8)>>12).to_ulong() << endl;
	cout << (((*rgbValues[test].sample)<<12)>>12).to_ulong() << endl;
*/


	ofstream outputFile("brick.h", ios::out);
	
	string lineOne = "static Gfx brick_C_dummy_aligner1[] = { gsSPEndDisplayList() };";
	string lineTwo = "unsigned short brick[] = {";

	outputFile << lineOne << endl << endl << lineTwo << endl;
	
	//now cycle through the rgbValues array (go ahead and delete stuffz as you go along)
	for(int iHeight = 0; iHeight < height; ++iHeight) {
		outputFile << "\t";
		for(int jWidth = 0; jWidth < width; ++jWidth) {
			int rgbValuesPosition = iHeight*width + jWidth;
			outputFile << "0x";
			outputFile << rgbValues[rgbValuesPosition].n64Format[0];
			outputFile << rgbValues[rgbValuesPosition].n64Format[1];
			outputFile << rgbValues[rgbValuesPosition].n64Format[2];
			outputFile << rgbValues[rgbValuesPosition].n64Format[3];
			outputFile << ", ";
			
			//deletz
			//delete[] rgbValues[rgbValuesPosition].n64Format;
			//ummmmmmmm so well figure this out later?? idk
		}
		outputFile << endl;
	}
	
	outputFile << "};\n";
	
	return 0;
 }

