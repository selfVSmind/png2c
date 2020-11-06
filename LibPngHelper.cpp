#include "LibPngHelper.h"

#include <iostream>
#include <cstdlib>

using namespace std;

LibPngHelper::LibPngHelper() {
    // constructer
}

string LibPngHelper::readInputPngFile(char* fileName)
{
	char fileHeader[8];    // 8 is the maximum size that can be checked

	// open file
	FILE *inputFilePointer = fopen(fileName, "rb");
	if (!inputFilePointer) {
		return (string(fileName) + " could not be opened." + "\n" + "Exiting.");
	}

	// check if png
	fread(fileHeader, 1, 8, inputFilePointer);
	if (png_sig_cmp((png_const_bytep)fileHeader, 0, 8)) {
		return (string(fileName) + " is not a PNG." + "\n" + "Exiting.");
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
	for (int y = 0; y < height; y++) {
		row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(pngPointer, infoPointer));
	}

	png_read_image(pngPointer, row_pointers);

	fclose(inputFilePointer);

    return "";
}
