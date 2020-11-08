/*
 * Copyright 2016-2020 Jason Lambert.
 */

const char *versionNumber = "2.0";

// fancy command line argument parser 
#include <tclap/CmdLine.h>

// my classes
#include "LibPngHelper.h"
#include "PixelFormatter.h"
#include "PixelWriter.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

char *buffer;
rgb *rgbValues;
string inputFileName;
string outputFileName;
int zDepth;

LibPngHelper myLibPngHelper = LibPngHelper();

// this function handles all of our command line arguments
// it utilizes the TCLAP library
bool setupTclap(int argc, char **argv) {
	bool fullRes = false;
	try {  
		TCLAP::CmdLine cmd("Get 'N Or Get Out", ' ', versionNumber);

		TCLAP::SwitchArg fullResSwitch("f", "fullres", "Generate data for full screen background.", cmd, false);

		TCLAP::ValueArg<string> outObjectArg("o", "outfile", "Name of output bitmap.", false, "", "string");
		cmd.add(outObjectArg);

		// TCLAP::ValueArg<float> zDepthArg("z", "zdepth", "Z-Depth of full screen image.", false, 0, "string");
		// cmd.add(zDepthArg);

		TCLAP::UnlabeledValueArg<string>  inFileArg("in", "", "", "", "string");
		cmd.add(inFileArg);
		
		cmd.parse( argc, argv );

		outputFileName = outObjectArg.getValue();
		inputFileName = inFileArg.getValue();
		// zDepth = zDepthArg.getValue();
		zDepth = 0; // manually setting 0 here because TCLAP isn't doing what I'm expecting when I drag and drop.. fix later
		fullRes = fullResSwitch.getValue();

	} catch (TCLAP::ArgException &e) {
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
	}

	return fullRes;
}

// when an output texture name/file name is not provided,
// this function is used to generate one from the input file
string stripFileName(string inputFileName) {
	// remove path from filename
	string temporaryInputFileNameStringObject(inputFileName);
	size_t splitterIndex = temporaryInputFileNameStringObject.find_last_of("/\\");
	temporaryInputFileNameStringObject = temporaryInputFileNameStringObject.substr(splitterIndex+1);

	// remove extension
	size_t lastindex = temporaryInputFileNameStringObject.find_last_of("."); 
	return temporaryInputFileNameStringObject.substr(0, lastindex); 
}

int main(int argc, char **argv) {
	
	bool fullRes = setupTclap(argc, argv);

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

	string textureName = stripFileName(inputFileName);
	PixelWriter myPixelWriter = PixelWriter(textureName, myLibPngHelper.width, myLibPngHelper.height);

	// generate a name for the output file if it isn't supplied
	char *outputFileNameCStr;
	if(outputFileName == "") {
		outputFileNameCStr = (char*)(textureName + ".h").c_str();
	} else {
		outputFileNameCStr = (char*)outputFileName.c_str();
	}

	// prepare the output file for writing
	ofstream outputFile(outputFileNameCStr, ios::out);
	
	// put some useful information at the top
	myPixelWriter.writeOutputFileHeader(outputFile, fullRes);

	// string lineOne = "static Gfx " + baseFileName + "_C_dummy_aligner1[] = { gsSPEndDisplayList() };";
	// string lineTwo = "unsigned short " + baseFileName + "[] = {";
	// outputFile << lineOne << endl << endl << lineTwo << endl;
	
	//now cycle through the rgbValues array and print to file

	if(fullRes)	{
		if(myPixelWriter.isFullScreenImage()) {
			cout << "Full screen conversion requires image dimensions of 320 x 240." << endl;
			exit(EXIT_FAILURE);
		}
		myPixelWriter.writeFullScreenImage(rgbValues, &outputFile, zDepth);
	} else {
		myPixelWriter.writeStandardImage(rgbValues, &outputFile);
		cout << "got here" << endl;
	}

	delete[] rgbValues;
	
	outputFile.close();

	cout << "Success! (probably)" << endl;

	return 0;
}