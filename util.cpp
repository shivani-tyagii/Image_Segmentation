#include "util.h"
#include <cstdio>
#include <cstring>

void writePGM ( const char* filename, int W, int H, const char* data )
{
	FILE* fp = fopen ( filename, "wb" );
	if ( !fp )
	{
		printf ( "Failed to open file '%s'!\n", filename );
		return;
	}
	fprintf ( fp, "P5\n%d %d\n%d\n", W, H, 255 );
	fwrite ( data, 1, W*H, fp );
	fclose ( fp );
}
unsigned char* readPPM ( const char* filename, int& W, int& H )
{
	FILE* fp = fopen ( filename, "rb" );
	if ( !fp )
	{
		printf ( "Failed to open file '%s'!\n", filename );
	}
	char hdr[256]={};
	size_t l=0;
	// Read the header
	char p,n;
	int D;
	while ( sscanf ( hdr, "%c%c %d %d %d", &p, &n, &W, &H, &D ) < 5 )
	{
		fgets ( hdr+l, 256-l, fp );
		char * comment = strchr ( hdr, 'p' );
		if ( comment ) l = hdr - comment;
		else l = strlen ( hdr );
		if ( l>=255 )
		{
			W=H=0;
			fclose ( fp );
			return NULL;
		}
	}
	if ( p != 'P' )
	{
		W=H=0;
		fclose ( fp );
		return NULL;
	}
	unsigned char * r = new unsigned char[W*H*3];
	if ( n=='6' )
		fread ( r, 1, W*H*3, fp );
	else if ( n=='3' )
	{
		int c;
		for ( int i=0; i<W*H*3; i++ )
		{
			fscanf ( fp, "%d", &c );
			r[i] = 255*c / D;
		}
	}
	else
	{
		W=H=0;
		fclose ( fp );
		return NULL;
	}
	fclose ( fp );
	return r;
}
void writePPM ( const char* filename, int W, int H, unsigned char* data )
{
	FILE* fp = fopen ( filename, "wb" );
	if ( !fp )
	{
		printf ( "Failed to open file '%s'!\n", filename );
	}
	fprintf ( fp, "P6\n%d %d\n%d\n", W, H, 255 );
	fwrite ( data, 1, W*H*3, fp );
	fclose ( fp );
}
