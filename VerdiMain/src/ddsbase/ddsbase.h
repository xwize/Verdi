// (c) by Stefan Roettger

#ifndef DDSBASE_H
#define DDSBASE_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <float.h>
#include <math.h>

#pragma warning(disable : 4390)
#pragma warning(disable : 4996)

#define BOOLINT char

#ifndef TRUE
#define TRUE (1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

#define ffloor(x) floor((double)(x))
#define ftrc(x) (int)ffloor(x)

#ifdef _MSC_VER
#define snprintf _snprintf
#endif 

void writeDDSfile(const char *filename,unsigned char *data,unsigned int bytes,unsigned int skip=0,unsigned int strip=0,BOOLINT nofree=FALSE);
unsigned char *readDDSfile(const char *filename,unsigned int *bytes);

void writeRAWfile(const char *filename,unsigned char *data,unsigned int bytes,BOOLINT nofree=FALSE);
unsigned char *readRAWfile(const char *filename,unsigned int *bytes);

void writePNMimage(const char *filename,unsigned char *image,unsigned int width,unsigned int height,unsigned int components,BOOLINT dds=FALSE);
unsigned char *readPNMimage(const char *filename,unsigned int *width,unsigned int *height,unsigned int *components);

void writePVMvolume(const char *filename,unsigned char *volume,
                    unsigned int width,unsigned int height,unsigned int depth,unsigned int components=1,
                    float scalex=1.0f,float scaley=1.0f,float scalez=1.0f,
                    unsigned char *description=NULL,
                    unsigned char *courtesy=NULL,
                    unsigned char *parameter=NULL,
                    unsigned char *comment=NULL);

unsigned char *readPVMvolume(const char *filename,
                             unsigned int *width,unsigned int *height,unsigned int *depth,unsigned int *components=NULL,
                             float *scalex=NULL,float *scaley=NULL,float *scalez=NULL,
                             unsigned char **description=NULL,
                             unsigned char **courtesy=NULL,
                             unsigned char **parameter=NULL,
                             unsigned char **comment=NULL);

int checkfile(const char *filename);
unsigned int checksum(unsigned char *data,unsigned int bytes);

void swapbytes(unsigned char *data,unsigned int bytes);
void convbytes(unsigned char *data,unsigned int bytes);
void convfloat(unsigned char *data,unsigned int bytes);

unsigned char *quantize(unsigned char *volume,
                        unsigned int width,unsigned int height,unsigned int depth,
                        BOOLINT linear=FALSE,BOOLINT nofree=FALSE);

#endif
