/*
 * Copyright 2016-2020 Jason Lambert.
 */

const char *version_number = "2.0";
 
#include <tclap/CmdLine.h>

#include <png.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <bitset>
#include <math.h>

using namespace std;

struct rgb {
	bitset<5> *r;
	bitset<5> *g;
	bitset<5> *b;
	bitset<1> *a;
	bitset<16> *sample;
	char *n64Format;
};

char *buffer;
rgb *rgbValues;
string baseFileName;
string outputFileName;
string inputFileName;

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
			return (char)NULL;
	}
}

int x, y;
int width, height;

png_byte colorType;
png_byte bitDepth;
png_structp pngPointer;
png_infop infoPointer;
png_bytep * row_pointers;
int number_of_passes;

void readInputPngFile(char* fileName)
{
	char fileHeader[8];    // 8 is the maximum size that can be checked

	// open file
	FILE *inputFilePointer = fopen(fileName, "rb");
	if (!inputFilePointer) {
		cout << fileName << " could not be opened." << endl;
		cout << "Exiting." << endl;
		exit(EXIT_FAILURE);
	}

	// check if png
	fread(fileHeader, 1, 8, inputFilePointer);
	if (png_sig_cmp((png_const_bytep)fileHeader, 0, 8)) {
		cout << fileName << " is not a PNG." << endl;
		cout << "Exiting." << endl;
		exit(EXIT_FAILURE);
	}


	// initialize libPNG and prepare for file reading
	pngPointer = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	infoPointer = png_create_info_struct(pngPointer);
	png_init_io(pngPointer, inputFilePointer);
	png_set_sig_bytes(pngPointer, 8);

	png_read_info(pngPointer, infoPointer);

	width = png_get_image_width(pngPointer, infoPointer);
	height = png_get_image_height(pngPointer, infoPointer);
	colorType = png_get_color_type(pngPointer, infoPointer);
	bitDepth = png_get_bit_depth(pngPointer, infoPointer);

	number_of_passes = png_set_interlace_handling(pngPointer);
	png_read_update_info(pngPointer, infoPointer);

	// read in the png file
	row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
	for (y = 0; y < height; y++) {
		row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(pngPointer, infoPointer));
	}

	png_read_image(pngPointer, row_pointers);

	fclose(inputFilePointer);
}

void helper(ofstream *outputFile, int subHeight, int subWidth, int originY, int originX) {
	*outputFile << "static Gfx " + baseFileName << "_" << originY << "_" << originX << "_C_dummy_aligner[] = { gsSPEndDisplayList() };" << endl;
	*outputFile << "static unsigned short " + baseFileName << "_" << originY << "_" << originX << "[] = {" << endl;

	for(int iHeight = originY; iHeight < originY+subHeight; ++iHeight) {
		if(iHeight > originY) *outputFile << ", " << endl;
		*outputFile << "\t";
		for(int jWidth = originX; jWidth < originX+subWidth; ++jWidth) {
			if(jWidth > originX) *outputFile << ", ";
			int rgbValuesPosition = iHeight*width + jWidth;
			*outputFile << "0x";
			*outputFile << rgbValues[rgbValuesPosition].n64Format[0];
			*outputFile << rgbValues[rgbValuesPosition].n64Format[1];
			*outputFile << rgbValues[rgbValuesPosition].n64Format[2];
			*outputFile << rgbValues[rgbValuesPosition].n64Format[3];
		}
	}
	*outputFile << endl << "};\n" << endl;
}

void convertPixelData()
{
	// does the input image has an alpha channel
	bool hasAlpha = true;
	
	if (png_get_color_type(pngPointer, infoPointer) == PNG_COLOR_TYPE_RGB) {
		cout << "No alpha channel in image." << endl;
		hasAlpha = false;
	} else {
		cout << "There is an alpha channel in the image." << endl;
	}


	// allocate rgbValues array
	int rgbValuesSize = width*height;
	rgbValues = new rgb[rgbValuesSize];

	// fill up the rgbValues array
	for(int iHeight = 0; iHeight < height; ++iHeight) {
		png_byte* row = row_pointers[iHeight];
		for (int jWidth = 0; jWidth < width; jWidth++) {
			png_byte* ptr = hasAlpha ? &(row[jWidth*4]) : &(row[jWidth*3]);
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

			// clean it up!
			delete rgbValues[rgbValuesPosition].r;
			delete rgbValues[rgbValuesPosition].g;
			delete rgbValues[rgbValuesPosition].b;
			delete rgbValues[rgbValuesPosition].a;
			delete rgbValues[rgbValuesPosition].sample;
		}
	}
}

// static Vtx shade_vtx[] =  {
    // {   -64, 64, -5, 0, 0  << 6, 0  << 6, 0xff, 0xff, 0xff, 0xff},
    // {   64,  64, -5, 0, 32 << 6, 0  << 6, 0xff, 0xff, 0xff, 0xff},
    // {   64, -64, -5, 0, 32 << 6, 32 << 6, 0xff, 0xff, 0xff, 0xff},
    // {   -64,-64, -5, 0, 0  << 6, 32 << 6, 0xff, 0xff, 0xff, 0xff},
// };

void fullScreenImage(ofstream *outputFile)
{
	int full_screen_height = 240, full_screen_width = 320;
	int z_depth = -5;
	if((height != full_screen_height) || (width != full_screen_width)) {
		cout << "Full screen conversion requires image dimensions of 320 x 240." << endl;
		exit(EXIT_FAILURE);
	}
	
	// we'll be breaking the image up into 32x32 pixel chunks
	int chunk_size = 32;
	int num_rows = (1.0 * full_screen_height)/chunk_size + 0.5, num_columns = full_screen_width/chunk_size; //fix this later.. if you feel like it
	
	// *outputFile << "static Vtx " + baseFileName << "_vtx[] = {" << endl;
	// for(int i = 0; i < num_rows; ++i) {
		// for(int j = 0; j < num_columns; ++j) {
			// // *outputFile << "static Vtx " + baseFileName << "_" << i * chunk_size << "_" << j * chunk_size << "_vtx[] = {" << endl;  //original
			// *outputFile << "\t{ " << j * chunk_size - (full_screen_width / 2) << ", " << i * chunk_size + (full_screen_height / 2) << ", " << z_depth << ", 0, 0 << 6, 0 << 6, 0, 0, 0, 0 }," << endl;
			// *outputFile << "\t{ " << j * chunk_size + chunk_size - (full_screen_width / 2) << ", " << i * chunk_size + (full_screen_height / 2) << ", " << z_depth << ", 0, " << chunk_size << " << 6, 0 << 6, 0, 0, 0, 0 }," << endl;
			// *outputFile << "\t{ " << j * chunk_size + chunk_size - (full_screen_width / 2) << ", " << i * chunk_size - chunk_size + (full_screen_height / 2) << ", " << z_depth << ", 0, " << chunk_size << " << 6, " << chunk_size << " << 6, 0, 0, 0, 0 }," << endl;
			// *outputFile << "\t{ " << j * chunk_size - (full_screen_width / 2) << ", " << i * chunk_size - chunk_size + (full_screen_height / 2) << ", " << z_depth << ", 0, 0 << 6, " << chunk_size << " << 6, 0, 0, 0, 0 }," << endl;
		// }
	// }
	// *outputFile << "};" << endl;

	for(int i = 0; i < num_rows; ++i) {
		for(int j = 0; j < num_columns; ++j) {
			*outputFile << "static Vtx " + baseFileName << "_" << i * chunk_size << "_" << j * chunk_size << "_vtx[] = {" << endl;  //original
			*outputFile << "\t{ " << j * chunk_size - (full_screen_width / 2) << ", " << 0 - 0 - i * chunk_size + (full_screen_height / 2) << ", " << z_depth << ", 0, 0 << 6, 0 << 6, 0, 0, 0, 0 }," << endl;
			*outputFile << "\t{ " << j * chunk_size + chunk_size - (full_screen_width / 2) << ", " << 0 - i * chunk_size + (full_screen_height / 2) << ", " << z_depth << ", 0, " << chunk_size << " << 6, 0 << 6, 0, 0, 0, 0 }," << endl;
			if(i+1 == num_rows) {
				*outputFile << "\t{ " << j * chunk_size + chunk_size - (full_screen_width / 2) << ", " << 0 - i * chunk_size - chunk_size / 2 + (full_screen_height / 2) << ", " << z_depth << ", 0, " << chunk_size << " << 6, " << chunk_size / 2 << " << 6, 0, 0, 0, 0 }," << endl;
				*outputFile << "\t{ " << j * chunk_size - (full_screen_width / 2) << ", " << 0 - i * chunk_size - chunk_size / 2 + (full_screen_height / 2) << ", " << z_depth << ", 0, 0 << 6, " << chunk_size / 2 << " << 6, 0, 0, 0, 0 }" << endl;
			} else {
				*outputFile << "\t{ " << j * chunk_size + chunk_size - (full_screen_width / 2) << ", " << 0 - i * chunk_size - chunk_size + (full_screen_height / 2) << ", " << z_depth << ", 0, " << chunk_size << " << 6, " << chunk_size << " << 6, 0, 0, 0, 0 }," << endl;
				*outputFile << "\t{ " << j * chunk_size - (full_screen_width / 2) << ", " << 0 - i * chunk_size - chunk_size + (full_screen_height / 2) << ", " << z_depth << ", 0, 0 << 6, " << chunk_size << " << 6, 0, 0, 0, 0 }" << endl;
			}
			*outputFile << "};" << endl << endl;
		}
	}
	*outputFile << "static Vtx *" + baseFileName << "_vtx[" << num_columns * num_rows << "] = {" << endl;
	for(int i = 0; i < num_rows; ++i) {
		for(int j = 0; j < num_columns; ++j) {
			*outputFile << "\t" + baseFileName << "_" << i * chunk_size << "_" << j * chunk_size << "_vtx," << endl;
		}
	}
	*outputFile << "};" << endl;

	// *outputFile << "static Vtx *" + baseFileName << "_vtx[" << num_columns * num_rows << "] = {" << endl;
	// for(int i = 0; i < num_rows; ++i) {
		// for(int j = 0; j < num_columns; ++j) {
			// // *outputFile << "static Vtx " + baseFileName << "_" << i * chunk_size << "_" << j * chunk_size << "_vtx[] = {" << endl;  //original
			// *outputFile << "\t(Vtx []){" << endl;
			// *outputFile << "\t\t{ " << j * chunk_size - (full_screen_width / 2) << ", " << i * chunk_size + (full_screen_height / 2) << ", " << z_depth << ", 0, 0 << 6, 0 << 6, 0, 0, 0, 0 }," << endl;
			// *outputFile << "\t\t{ " << j * chunk_size + chunk_size - (full_screen_width / 2) << ", " << i * chunk_size + (full_screen_height / 2) << ", " << z_depth << ", 0, " << chunk_size << " << 6, 0 << 6, 0, 0, 0, 0 }," << endl;
			// *outputFile << "\t\t{ " << j * chunk_size + chunk_size - (full_screen_width / 2) << ", " << i * chunk_size - chunk_size + (full_screen_height / 2) << ", " << z_depth << ", 0, " << chunk_size << " << 6, " << chunk_size << " << 6, 0, 0, 0, 0 }," << endl;
			// *outputFile << "\t\t{ " << j * chunk_size - (full_screen_width / 2) << ", " << i * chunk_size - chunk_size + (full_screen_height / 2) << ", " << z_depth << ", 0, 0 << 6, " << chunk_size << " << 6, 0, 0, 0, 0 }" << endl;
			// *outputFile << "\t}," << endl;
		// }
	// }
	// *outputFile << "};" << endl;
	for(int i = 0; i < num_rows; ++i) {
		for(int j = 0; j < num_columns; ++j) {
			helper(outputFile, (i+1 == num_rows) ? chunk_size/2 : chunk_size, chunk_size, i * chunk_size, j * chunk_size);
		}
	}

	*outputFile << "static unsigned short *" + baseFileName << "_tex[" << num_columns * num_rows << "] = {" << endl;
	for(int i = 0; i < num_rows; ++i) {
		for(int j = 0; j < num_columns; ++j) {
			*outputFile << "\t" + baseFileName << "_" << i * chunk_size << "_" << j * chunk_size << "," << endl;
		}
	}
	*outputFile << "};" << endl;
}

// this function sets up all of our command line arguments
// it utilizes the TCLAP library
bool setupTclap(int argc, char **argv) {
	bool fullRes = false;
	try {  
		TCLAP::CmdLine cmd("Get 'N Or Get Out", ' ',version_number);

		TCLAP::SwitchArg fullResSwitch("f","fullres","Generate data for full screen background.", cmd, false);

		TCLAP::ValueArg<std::string> outObjectArg("o","outfile","Name of output bitmap.",false,"","string");
		cmd.add( outObjectArg );

		TCLAP::UnlabeledValueArg<std::string>  inFile("in","","","","string");
		cmd.add( inFile );
		
		cmd.parse( argc, argv );
		outputFileName = outObjectArg.getValue();
		inputFileName = inFile.getValue();

		fullRes = fullResSwitch.getValue();

	} catch (TCLAP::ArgException &e)  // catch any exceptions
	{ std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }

	return fullRes;
}

int main(int argc, char **argv)
{
	bool fullRes = setupTclap(argc, argv);

	// remove path
	string temporaryInputFileNameStringObject(inputFileName);
	size_t splitterIndex = temporaryInputFileNameStringObject.find_last_of("/\\");
	temporaryInputFileNameStringObject = temporaryInputFileNameStringObject.substr(splitterIndex+1);

	// remove extension
	size_t lastindex = temporaryInputFileNameStringObject.find_last_of("."); 
	cout << lastindex << endl;
	baseFileName = temporaryInputFileNameStringObject.substr(0, lastindex); 

	readInputPngFile((char*)inputFileName.c_str());
	convertPixelData();

	// generate a name for the output file if it isn't supplied
	char *outName;
	if(outputFileName == "") {
		outName = (char*)(baseFileName + ".h").c_str();
	} else {
		outName = (char*)outputFileName.c_str();
	}

	// prepare the output file for writing
	ofstream outputFile(outName, ios::out);
	
	outputFile << "/* Height " << height << " */" << endl;
	outputFile << "/* Width " << width << " */" << endl;
	outputFile << endl << endl;
	
	// string lineOne = "static Gfx " + baseFileName + "_C_dummy_aligner1[] = { gsSPEndDisplayList() };";
	// string lineTwo = "unsigned short " + baseFileName + "[] = {";
	// outputFile << lineOne << endl << endl << lineTwo << endl;
	
	//now cycle through the rgbValues array and print to file

	if(fullRes)	fullScreenImage(&outputFile);
	
	delete[] rgbValues;
	
	outputFile.close();

	return 0;
}