///////////////////////////////////////////////////////////////////////
//  
// Portable Bitmap Reading/Writing
//
// pnm.c - Reader and writer for PNM, PFM and PDM
//
// by Philip Fu (cwfu@acm.org)
// Copyright 2002, Trustees of Indiana University
//
// Mon Jan 22 7:54:04 EST 2001
//
// All rights reserved
// 
///////////////////////////////////////////////////////////////////////




#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "standard.h"
#include "pnm.h"




// Global variable in this pnm.c for allocating and deallocating memory

TypePNMMallocFuncPtr ptrPNMMalloc = &malloc;
TypePNMFreeFuncPtr   ptrPNMFree   = &free;




static void
pre_allocate(FILE **fp, long size)
{
    long currpos;
    int  rtnval;

    // trick to speedup the massive writing (just seek to the end)
    currpos = ftell(*fp);						// backup the current position

    rtnval  = fseek(*fp,size,SEEK_CUR);					// go to the end
    if (rtnval == -1)							// disaster......
	fprintf(stderr,"Disk Full? Failed to fseek in write_pnm.\n\n");

    rtnval  = fseek(*fp,currpos,SEEK_SET);
    if (rtnval == -1)							// disaster......
	fprintf(stderr,"Disaster? Failed to fseek back.\n\n");
}


int
writePNM(const char *file, const char *type, int w, int h, int maxV, unsigned char *data)
{
	FILE *outFP;

	// open the file properly
	outFP = fopen(file,"wb");
	if (outFP == NULL) {
	    printf("Can't open file [%s]!\n",file);
	    return _ERROR;
	}

	// write the header
	fprintf(outFP,"%s\n",   type);
	fprintf(outFP,"%d %d\n",w,h);
	fprintf(outFP,"%d\n",   maxV);

	// write the data
	if (!strcmp(type,"P6")) {
	    pre_allocate(&outFP,w*h*3);
	    cp_fwrite(data, sizeof(unsigned char), w*h*3, outFP);
	} else
	if (!strcmp(type,"P5")) {
	    pre_allocate(&outFP,w*h);
	    cp_fwrite(data, sizeof(unsigned char), w*h, outFP);
	} else {
	    printf("writePNM : Wrong Type! Must be P6 or P5!\n");
	    fclose(outFP);
	    return _ERROR;
	}

	fclose(outFP);

	return _OKAY;
}



int
writePFM(const char *file, const char *type, int w, int h, float maxV, float *data)
{
	FILE *outFP;

	// open the file properly
	outFP = fopen(file,"wb");
	if (outFP == NULL) {
	    printf("Can't open file [%s]!\n",file);
	    return _ERROR;
	}

	// write the header
	fprintf(outFP,"%s\n",   type);
	fprintf(outFP,"%d %d\n",w,h);

	// write the data
	if (!strcmp(type,"PF")) {
	    fprintf(outFP,"%f\n", maxV);
	    pre_allocate(&outFP,w*h*sizeof(float));
	    cp_fwrite(data, sizeof(float), w*h, outFP);
	} else {
	    printf("writePFM : Wrong Type! Must be PF!\n");
	    fclose(outFP);
	    return _ERROR;
	}

	fclose(outFP);

	return _OKAY;
}



int
writePDM(const char *file, const char *type, int w, int h, double maxV, double *data)
{
	FILE *outFP;

	// open the file properly
	outFP = fopen(file,"wb");
	if (outFP == NULL) {
	    printf("Can't open file [%s]!\n",file);
	    return _ERROR;
	}

	// write the header
	fprintf(outFP,"%s\n",   type);
	fprintf(outFP,"%d %d\n",w,h);

	// write the data
	if (!strcmp(type,"PD")) {
	    fprintf(outFP,"%f\n", (float) maxV);
	    pre_allocate(&outFP,w*h*sizeof(double));
	    cp_fwrite(data, sizeof(double), w*h, outFP);
	} else {
	    printf("writePDM : Wrong Type! Must be PD!\n");
	    fclose(outFP);
	    return _ERROR;
	}

	fclose(outFP);

	return _OKAY;
}



// !!! memory for img is here
int
readPNM(const char *file, const char *type, int *w, int *h, int *maxV, unsigned char **data)
{
	FILE *outFP;

	// open the file
	outFP = fopen(file,"r");
	if (outFP == NULL) {
	    printf("Can't read file [%s]!\n",file);
	    return _ERROR;
	}

	// skip header
	while ( getc(outFP) != '\n' ) ;

	// skip comment if any
	while (getc(outFP) == '#')
	    while ( getc(outFP) != '\n' );
	fseek(outFP,-1,SEEK_CUR);

	// get width, height and max. value
	fscanf(outFP,"%d %d",w,h);
	fscanf(outFP,"%d", maxV);
	while ( getc(outFP) != '\n' ) ;
	//printf("Img : %d x %d (%d)\n",*w,*h,*maxV);

	// read the data
	if (!strcmp(type,"P6")) {
	    *data = (unsigned char *) (*ptrPNMMalloc)( sizeof(unsigned char)*(*w)*(*h)*3 );
	    cp_fread(*data, (*w)*(*h)*3, sizeof(unsigned char), outFP);

	} else
	if (!strcmp(type,"P5")) {
	    *data = (unsigned char *) (*ptrPNMMalloc)( sizeof(unsigned char)*(*w)*(*h) );
	    cp_fread(*data, (*w)*(*h), sizeof(unsigned char), outFP);

	} else {
	    printf("readPNM : Wrong Type! Must be P6 or P5!\n");
	    fclose(outFP);
	    return _ERROR;
	}

	fclose(outFP);

	return _OKAY;
}



// !!! memory for img is here
int
readPFM(const char *file, const char *type, int *w, int *h, float *maxV, float **data)
{
	FILE *outFP;

	// open the file
	outFP = fopen(file,"r");
	if (outFP == NULL) {
	    printf("Can't read file [%s]!\n",file);
	    return _ERROR;
	}

	// skip header
	while ( getc(outFP) != '\n' ) ;

	// skip comment if any
	while (getc(outFP) == '#')
	    while ( getc(outFP) != '\n' );
	fseek(outFP,-1,SEEK_CUR);

	// get width, height and max. value
        fscanf(outFP,"%d %d",w,h);


	if (!strcmp(type,"PF")) {

	    fscanf(outFP,"%f",maxV);
	    //printf("Img : %d x %d (%d)\n",*w,*h,*maxV);

	    // skip the \n
	    while ( getc(outFP) != '\n' );

	    // read the data
	    *data = (float *) (*ptrPNMMalloc)( sizeof(float)*(*w)*(*h) );
	    cp_fread(*data, (*w)*(*h), sizeof(float), outFP);

	} else {
	    printf("writePFM : Wrong Type! Must be PF!\n");
	    fclose(outFP);
	    return _ERROR;
	}

        fclose(outFP);

        return _OKAY;
}



// !!! memory for img is here
int
readPDM(const char *file, const char *type, int *w, int *h, double *maxV, double **data)
{
	FILE *outFP;
	float myFloat;

	// open the file
	outFP = fopen(file,"r");
	if (outFP == NULL) {
	    printf("Can't read file [%s]!\n",file);
	    return _ERROR;
	}

	// skip header
	while ( getc(outFP) != '\n' ) ;

	// skip comment if any
	while (getc(outFP) == '#')
		while ( getc(outFP) != '\n' );
	fseek(outFP,-1,SEEK_CUR);

	// get width, height and max. value
        fscanf(outFP,"%d %d",w,h);

	// get the data
	if (!strcmp(type,"PD")) {

	    // get the maxV
            fscanf(outFP,"%f",&myFloat);
	    *maxV = (double) myFloat;
	    //printf("Img : %d x %d (%d)\n",*w,*h,*maxV);

	    // skip the \n
	    while ( getc(outFP) != '\n' );

	    // read the data
	    *data = (double *) (*ptrPNMMalloc)( sizeof(double)*(*w)*(*h) );
            cp_fread(*data, (*w)*(*h), sizeof(double), outFP);

	} else {
	    printf("writePDM : Wrong Type! Must be PD!\n");
	    fclose(outFP);
	    return _ERROR;
	}

        fclose(outFP);

        return _OKAY;
}




///////////////////////////////////////////////////////////////
//
// Set the memory allocation functions

void
setAllocFunc_PNM( TypePNMMallocFuncPtr funcPtrToMalloc, TypePNMFreeFuncPtr funcPtrToFree )
{
    ptrPNMMalloc = funcPtrToMalloc;
    ptrPNMFree   = funcPtrToFree;
}

