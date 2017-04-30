#pragma once


void writePGM( const char * filename, int W, int H, const char * data );
unsigned char * readPPM( const char * filename, int & W, int & H );
void writePPM( const char * filename, int W, int H, unsigned char * data );
