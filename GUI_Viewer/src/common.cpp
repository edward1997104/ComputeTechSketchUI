//
// common.cpp
//
// By Tien-Tsin Wong
//

#include <stdio.h>
#include <stdlib.h>
#include "common.h"


// This function ensures the data order is increasing i.e.
// interval[0] < interval[1]
void SpanReorder(int *interval)
{
  int tmp;
  // make sure the data order in xwin & ywin are alright.
  tmp = MAX(interval[0], interval[1]);
  interval[0] = MIN(interval[0], interval[1]);
  interval[1] = tmp;
}


// Calculate the intersection region of two input windows.
// Note that if there is no intersection, xout[0] > xout[1] and/or
// yout[0] > yout[1]
void WindowIntersect(int *xwin1, int *ywin1, int *xwin2, 
		     int *ywin2, int *xout,  int *yout)
{
  xout[0] = MAX(xwin1[0], xwin2[0]); // intersection left is MAX of the left ends of both spans.
  xout[1] = MIN(xwin1[1], xwin2[1]); // intersection right is MIN of the right ends of both spans.
  yout[0] = MAX(ywin1[0], ywin2[0]);
  yout[1] = MIN(ywin1[1], ywin2[1]);
}


// Swap an array of nelem data (each with size of "size") 
// It is designed to solve the byte order inconsistency in SGI, SUN 
// and PC.
void SwapByte(unsigned char *buf, size_t elemsize, size_t nelem)
{
  size_t head, end, elem;
  unsigned char tmp;

  for(elem=0 ; elem < nelem ; elem++)
  {
    for(head=0, end=elemsize-1 ; head < end ; head++, end--)
    {
      tmp       = buf[head];
      buf[head] = buf[end];
      buf[end]  = tmp;
    }
    buf += elemsize;
  }
}



// Cross platform fread
// This routine is used to replace fread(). In PC, this routine will
// automatically swap the byte order after reading the data. In UNIX, 
// this routine is equivalent to fread().
// This routine decides how to swap the data by looking the argument
// "size". This argument tells the size of the basic element. 
// If size=1, no swapping is done. Otherwise, swapping is done.
// If the user doesn't want his data to be automatically byte-swapped
// he should set the "size" argument to 1
size_t cp_fread(void *buf, size_t size, size_t n, FILE* file)
{
#ifdef WIN32

  if (size == 1)
    return fread(buf, size, n, file);
  else if (size < 1)
    ERREXIT("[cp_fread]: fread size < 1\n");

  int result = fread(buf, size, n, file);
  // swap byte order automatically
  SwapByte((unsigned char *)buf, size, result); 
  return result;

#else
  return fread(buf, size, n, file);
#endif
}



// Cross platform fwrite
// This routine is used to replace fwrite(). In PC, this routine will
// automatically swap the byte order before writing the data. In UNIX, 
// this routine is equivalent to fread(). After writing, the data
// is swapped back to restore to the original byte order. Note this 
// may introduce error, if the given data has out-of-bound problem.
// This routine decides how to swap the data by looking the argument
// "size". This argument tells the size of the basic element. 
// If size=1, no swapping is done. Otherwise, swapping is done.
// If the user doesn't want his data to be automatically byte-swapped
// he should set the "size" argument to 1
size_t cp_fwrite(void *buf, size_t size, size_t n, FILE* file)
{
#ifdef WIN32

  if (size == 1)
    return fwrite(buf, size, n, file);
  else if (size < 1)
    ERREXIT("[cp_fwrite]: fwrite size < 1\n");

  // swap byte order automatically for writing
  SwapByte((unsigned char *)buf, size, n); 
  int result = fwrite(buf, size, n, file);
  // swap byte order back to original byte order
  SwapByte((unsigned char *)buf, size, n); 
  return result;

#else
  return fwrite(buf, size, n, file);
#endif
}



// Get the file size of the binary file pointed by fptr.
// This function is not suitable for text file because of two reasons:
// 1. Text mode causes carriage return-linefeed translation;
// 2. Ctrl-Z will be interpreted as end of file.
// Return -1 if error occurs.
long getFileSize(FILE *fptr)
{
	long currpos, filesize;
	
	currpos = ftell(fptr);	// backup the current position

	if (fseek(fptr, 0, SEEK_END))
		return -1;
	filesize = ftell(fptr);

	if (fseek(fptr, currpos, SEEK_SET))	// reset the current position; disaster if fail
		return -1;

	return filesize;
}
