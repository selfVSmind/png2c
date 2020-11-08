#include "PixelWriter.h"

PixelWriter::PixelWriter(string textureName, int textureWidth, int textureHeight) {
	this->textureName = textureName;
	this->textureWidth = textureWidth;
	this->textureHeight = textureHeight;
}

void PixelWriter::writeOutputFileHeader(ofstream &outputFile, bool fullRes) {
	outputFile << "/* Width " << textureWidth << " */" << endl;
	outputFile << "/* Height " << textureHeight << " */" << endl;
	if(fullRes) {
		outputFile << endl << "/*" << endl;
		outputFile << "\tThis file represents a large image designed to fill the entire screen." << endl;
		outputFile << "\tAs such, it has been broken up into smaller pieces and the required vertex data is also included." << endl;
		outputFile << "*/" << endl;
	}
	outputFile << endl << endl;
}

void PixelWriter::writePixelRange(rgb *rgbValues, ofstream *outputFile, int subHeight, int subWidth, int originY, int originX) {
	*outputFile << "static Gfx " + textureName << "_" << originY << "_" << originX << "_C_dummy_aligner[] = { gsSPEndDisplayList() };" << endl;
	*outputFile << "static unsigned short " + textureName << "_" << originY << "_" << originX << "[] = {" << endl;

	for(int iHeight = originY; iHeight < originY+subHeight; ++iHeight) {
		if(iHeight > originY) *outputFile << ", " << endl;
		*outputFile << "\t";
		for(int jWidth = originX; jWidth < originX+subWidth; ++jWidth) {
			if(jWidth > originX) *outputFile << ", ";
			int rgbValuesPosition = iHeight*textureWidth + jWidth;
			*outputFile << "0x";
			*outputFile << rgbValues[rgbValuesPosition].n64Format[0];
			*outputFile << rgbValues[rgbValuesPosition].n64Format[1];
			*outputFile << rgbValues[rgbValuesPosition].n64Format[2];
			*outputFile << rgbValues[rgbValuesPosition].n64Format[3];
		}
	}
	*outputFile << endl << "};\n" << endl;
}

bool PixelWriter::isFullScreenImage() {
	return (textureHeight != fullScreenHeight) || (textureWidth != fullScreenWidth);
}

void PixelWriter::writeStandardImage(rgb *rgbValues, ofstream *outputFile) {
	writePixelRange(rgbValues, outputFile, textureHeight, textureWidth, 0, 0);
}

void PixelWriter::writeFullScreenImage(rgb *rgbValues, ofstream *outputFile, int zDepth) {

	// we'll be breaking the image up into 32x32 pixel chunks
	int chunk_size = 32;
	int num_rows = (1.0 * fullScreenHeight)/chunk_size + 0.5, num_columns = fullScreenWidth/chunk_size; //fix this later.. if you feel like it
	
	// *outputFile << "static Vtx " + textureName << "_vtx[] = {" << endl;
	// for(int i = 0; i < num_rows; ++i) {
		// for(int j = 0; j < num_columns; ++j) {
			// // *outputFile << "static Vtx " + textureName << "_" << i * chunk_size << "_" << j * chunk_size << "_vtx[] = {" << endl;  //original
			// *outputFile << "\t{ " << j * chunk_size - (fullScreenWidth / 2) << ", " << i * chunk_size + (fullScreenHeight / 2) << ", " << zDepth << ", 0, 0 << 6, 0 << 6, 0, 0, 0, 0 }," << endl;
			// *outputFile << "\t{ " << j * chunk_size + chunk_size - (fullScreenWidth / 2) << ", " << i * chunk_size + (fullScreenHeight / 2) << ", " << zDepth << ", 0, " << chunk_size << " << 6, 0 << 6, 0, 0, 0, 0 }," << endl;
			// *outputFile << "\t{ " << j * chunk_size + chunk_size - (fullScreenWidth / 2) << ", " << i * chunk_size - chunk_size + (fullScreenHeight / 2) << ", " << zDepth << ", 0, " << chunk_size << " << 6, " << chunk_size << " << 6, 0, 0, 0, 0 }," << endl;
			// *outputFile << "\t{ " << j * chunk_size - (fullScreenWidth / 2) << ", " << i * chunk_size - chunk_size + (fullScreenHeight / 2) << ", " << zDepth << ", 0, 0 << 6, " << chunk_size << " << 6, 0, 0, 0, 0 }," << endl;
		// }
	// }
	// *outputFile << "};" << endl;

	for(int i = 0; i < num_rows; ++i) {
		for(int j = 0; j < num_columns; ++j) {
			*outputFile << "static Vtx " + textureName << "_" << i * chunk_size << "_" << j * chunk_size << "_vtx[] = {" << endl;  //original
			*outputFile << "\t{ " << j * chunk_size - (fullScreenWidth / 2) << ", " << 0 - 0 - i * chunk_size + (fullScreenHeight / 2) << ", " << zDepth << ", 0, 0 << 6, 0 << 6, 0, 0, 0, 0 }," << endl;
			*outputFile << "\t{ " << j * chunk_size + chunk_size - (fullScreenWidth / 2) << ", " << 0 - i * chunk_size + (fullScreenHeight / 2) << ", " << zDepth << ", 0, " << chunk_size << " << 6, 0 << 6, 0, 0, 0, 0 }," << endl;
			if(i+1 == num_rows) {
				*outputFile << "\t{ " << j * chunk_size + chunk_size - (fullScreenWidth / 2) << ", " << 0 - i * chunk_size - chunk_size / 2 + (fullScreenHeight / 2) << ", " << zDepth << ", 0, " << chunk_size << " << 6, " << chunk_size / 2 << " << 6, 0, 0, 0, 0 }," << endl;
				*outputFile << "\t{ " << j * chunk_size - (fullScreenWidth / 2) << ", " << 0 - i * chunk_size - chunk_size / 2 + (fullScreenHeight / 2) << ", " << zDepth << ", 0, 0 << 6, " << chunk_size / 2 << " << 6, 0, 0, 0, 0 }" << endl;
			} else {
				*outputFile << "\t{ " << j * chunk_size + chunk_size - (fullScreenWidth / 2) << ", " << 0 - i * chunk_size - chunk_size + (fullScreenHeight / 2) << ", " << zDepth << ", 0, " << chunk_size << " << 6, " << chunk_size << " << 6, 0, 0, 0, 0 }," << endl;
				*outputFile << "\t{ " << j * chunk_size - (fullScreenWidth / 2) << ", " << 0 - i * chunk_size - chunk_size + (fullScreenHeight / 2) << ", " << zDepth << ", 0, 0 << 6, " << chunk_size << " << 6, 0, 0, 0, 0 }" << endl;
			}
			*outputFile << "};" << endl << endl;
		}
	}
	*outputFile << "static Vtx *" + textureName << "_vtx[" << num_columns * num_rows << "] = {" << endl;
	for(int i = 0; i < num_rows; ++i) {
		for(int j = 0; j < num_columns; ++j) {
			*outputFile << "\t" + textureName << "_" << i * chunk_size << "_" << j * chunk_size << "_vtx," << endl;
		}
	}
	*outputFile << "};" << endl << endl;

	// *outputFile << "static Vtx *" + textureName << "_vtx[" << num_columns * num_rows << "] = {" << endl;
	// for(int i = 0; i < num_rows; ++i) {
		// for(int j = 0; j < num_columns; ++j) {
			// // *outputFile << "static Vtx " + textureName << "_" << i * chunk_size << "_" << j * chunk_size << "_vtx[] = {" << endl;  //original
			// *outputFile << "\t(Vtx []){" << endl;
			// *outputFile << "\t\t{ " << j * chunk_size - (fullScreenWidth / 2) << ", " << i * chunk_size + (fullScreenHeight / 2) << ", " << zDepth << ", 0, 0 << 6, 0 << 6, 0, 0, 0, 0 }," << endl;
			// *outputFile << "\t\t{ " << j * chunk_size + chunk_size - (fullScreenWidth / 2) << ", " << i * chunk_size + (fullScreenHeight / 2) << ", " << zDepth << ", 0, " << chunk_size << " << 6, 0 << 6, 0, 0, 0, 0 }," << endl;
			// *outputFile << "\t\t{ " << j * chunk_size + chunk_size - (fullScreenWidth / 2) << ", " << i * chunk_size - chunk_size + (fullScreenHeight / 2) << ", " << zDepth << ", 0, " << chunk_size << " << 6, " << chunk_size << " << 6, 0, 0, 0, 0 }," << endl;
			// *outputFile << "\t\t{ " << j * chunk_size - (fullScreenWidth / 2) << ", " << i * chunk_size - chunk_size + (fullScreenHeight / 2) << ", " << zDepth << ", 0, 0 << 6, " << chunk_size << " << 6, 0, 0, 0, 0 }" << endl;
			// *outputFile << "\t}," << endl;
		// }
	// }
	// *outputFile << "};" << endl;
	for(int i = 0; i < num_rows; ++i) {
		for(int j = 0; j < num_columns; ++j) {
			writePixelRange(rgbValues, outputFile, (i+1 == num_rows) ? chunk_size/2 : chunk_size, chunk_size, i * chunk_size, j * chunk_size);
		}
	}

	*outputFile << "static unsigned short *" + textureName << "_tex[" << num_columns * num_rows << "] = {" << endl;
	for(int i = 0; i < num_rows; ++i) {
		for(int j = 0; j < num_columns; ++j) {
			*outputFile << "\t" + textureName << "_" << i * chunk_size << "_" << j * chunk_size << "," << endl;
		}
	}
	*outputFile << "};" << endl;
}
