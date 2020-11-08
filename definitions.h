#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <bitset>

struct rgb {
	std::bitset<5> *r;
	std::bitset<5> *g;
	std::bitset<5> *b;
	std::bitset<1> *a;
	std::bitset<16> *sample;
	char *n64Format;
};

#endif