/*
 * Copyright 2016-2020 Jason Lambert.
 */

const char *version_number = "2.0";

// fancy command line argument parser 
#include <tclap/CmdLine.h>

// my classes
#include "LibPngHelper.h"
#include "PixelFormatter.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

char *buffer;
rgb *rgbValues;
string baseFileName;
string outputFileName;
string inputFileName;

void helper(ofstream *outputFile, int subHeight, int subWidth, int originY, int originX) {
	*outputFile << "static Gfx " + baseFileName << "_" << originY << "_" << originX << "_C_dummy_aligner[] = { gsSPEndDisplayList() };" << endl;
	*outputFile << "static unsigned short " + baseFileName << "_" << originY << "_" << originX << "[] = {" << endl;

	for(int iHeight = originY; iHeight < originY+subHeight; ++iHeight) {
		if(iHeight > originY) *outputFile << ", " << endl;
		*outputFile << "\t";
		for(int jWidth = originX; jWidth < originX+subWidth; ++jWidth) {
			if(jWidth > originX) *outputFile << ", ";
			int rgbValuesPosition = iHeight*myLibPngHelper.width + jWidth;
			*outputFile << "0x";
			*outputFile << rgbValues[rgbValuesPosition].n64Format[0];
			*outputFile << rgbValues[rgbValuesPosition].n64Format[1];
			*outputFile << rgbValues[rgbValuesPosition].n64Format[2];
			*outputFile << rgbValues[rgbValuesPosition].n64Format[3];
		}
	}
	*outputFile << endl << "};\n" << endl;
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
	if((myLibPngHelper.height != full_screen_height) || (myLibPngHelper.width != full_screen_width)) {
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
	*outputFile << "};" << endl << endl;

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

void writeOutputFileHeader(ofstream &outputFile, bool fullRes) {
	outputFile << "/* Width " << myLibPngHelper.width << " */" << endl;
	outputFile << "/* Height " << myLibPngHelper.height << " */" << endl;
	if(fullRes) {
		outputFile << endl << "/*" << endl;
		outputFile << "\tThis file represents a large image designed to fill the entire screen." << endl;
		outputFile << "\tAs such, it has been broken up into smaller pieces and the required vertex data is also included." << endl;
		outputFile << "*/" << endl;
	}
	outputFile << endl << endl;
}

int main(int argc, char **argv)
{
	bool fullRes = setupTclap(argc, argv);

	// remove path from filename
	string temporaryInputFileNameStringObject(inputFileName);
	size_t splitterIndex = temporaryInputFileNameStringObject.find_last_of("/\\");
	temporaryInputFileNameStringObject = temporaryInputFileNameStringObject.substr(splitterIndex+1);

	// remove extension
	size_t lastindex = temporaryInputFileNameStringObject.find_last_of("."); 
	baseFileName = temporaryInputFileNameStringObject.substr(0, lastindex); 


	// try to read the png into memory
	string error = myLibPngHelper.readInputPngFile((char*)inputFileName.c_str());
	if(error != "") {
		cout << error << endl;
		return 0;
	}

	// allocate rgbValues array
	int rgbValuesSize = myLibPngHelper.width*myLibPngHelper.height;
	rgbValues = new rgb[rgbValuesSize];

	// convert the pixels into the 5/5/5/1 format
	PixelFormatter::convertPixelData(myLibPngHelper.rowPointers, myLibPngHelper.width, myLibPngHelper.height, myLibPngHelper.bitDepth, myLibPngHelper.hasAlpha(), rgbValues);

	// generate a name for the output file if it isn't supplied
	char *outName;
	if(outputFileName == "") {
		outName = (char*)(baseFileName + ".h").c_str();
	} else {
		outName = (char*)outputFileName.c_str();
	}

	// prepare the output file for writing
	ofstream outputFile(outName, ios::out);
	
	// put some useful information at the top
	writeOutputFileHeader(outputFile, fullRes);

	// string lineOne = "static Gfx " + baseFileName + "_C_dummy_aligner1[] = { gsSPEndDisplayList() };";
	// string lineTwo = "unsigned short " + baseFileName + "[] = {";
	// outputFile << lineOne << endl << endl << lineTwo << endl;
	
	//now cycle through the rgbValues array and print to file

	if(fullRes)	fullScreenImage(&outputFile);

	delete[] rgbValues;
	
	outputFile.close();

	cout << "Success! (probably)" << endl;

	return 0;
}