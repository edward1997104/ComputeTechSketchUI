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




#ifndef _PNM_PFM_PDM_RW
#define _PNM_PFM_PDM_RW


#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif


    // Type of function pointer for my internal malloc
    typedef void	*(*TypePNMMallocFuncPtr)(size_t);
    typedef void	 (*TypePNMFreeFuncPtr)(void *);

    // on Error, _ERROR will be returned, otherwise _OKAY! (def. in standard.h)
    int writePNM(const char *file, const char *type, int w, int h, int    maxV, unsigned char *data);
    int writePFM(const char *file, const char *type, int w, int h, float  maxV, float *data);
    int writePDM(const char *file, const char *type, int w, int h, double maxV, double *data);

    // !!! memory for data is allocated here
    int readPNM(const char *file, const char *type, int *w, int *h, int    *maxV, unsigned char **data);
    int readPFM(const char *file, const char *type, int *w, int *h, float  *maxV, float **data);
    int readPDM(const char *file, const char *type, int *w, int *h, double *maxV, double **data);

    // set the memory allocation functions
    void setAllocFunc_PNM( TypePNMMallocFuncPtr funcPtrToMalloc, TypePNMFreeFuncPtr funcPtrToFree );


#ifdef __cplusplus
}
#endif


#endif
