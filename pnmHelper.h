/**
    \file pnmHelper.h
    Header file for PNM image readers & writers.

    \author George J. Grevera, Ph.D., ggrevera@sju.edu

    Copyright (C) 2002, George J. Grevera

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
    USA or from http://www.gnu.org/licenses/gpl.txt.

    This General Public License does not permit incorporating this
    code into proprietary programs.  (So a hypothetical company such
    as GH (Generally Hectic) should NOT incorporate this code into
    their proprietary programs.)
 */
#ifndef pnmHelper_h
#define pnmHelper_h

#include  <assert.h>
#include  <stdio.h>
//----------------------------------------------------------------------
/** \brief This class contains methods that read and write PNM images
 *  (color rgb and grey images).
 */
class pnmHelper {
private:
    /** \brief This method is called as the result of an error.
     */
    static void usage ( const char* const msg=NULL ) {
        exit( 0 );
    }
public:
    //------------------------------------------------------------------
    /** \brief This method should be generally used to read any pnm
     *  (pgm grey, ppm color) binary or ascii image files.
     *
     *  It's the caller's responsibility to free the malloc'd data.
     */
    static int* read_pnm_file ( const char* const fname, int* w, int* h,
        int* samplesPerPixel, int* min, int* max )
    {
        assert( fname!=NULL && w!=NULL && h!=NULL && samplesPerPixel!=NULL
             && min!=NULL && max!=NULL );
        *w = *h = *samplesPerPixel = *min = *max = 0;
        if (strlen(fname) == 0)    usage("bad input file name");
        FILE*  fp = fopen(fname, "rb");
        if (fp == NULL)    usage("can't open the input file");
        char ln[BUFSIZ];
        //get the first non-comment line
        for ( ; ; ) {
            ln[0] = 0;
            fgets(ln, sizeof ln, fp);
            if (ln[0] != '#')    break;
        }
        fclose(fp);    fp=NULL;

        if ( strcmp(ln,"P2\n")==0   || strcmp(ln,"P2\r")==0 
          || strcmp(ln,"P2\n\r")==0 || strcmp(ln,"P2\r\n")==0 ) {
            *samplesPerPixel = 1;
            return read_ascii_pgm_file( fname, w, h, min, max );
        } else if ( strcmp(ln,"P3\n")==0   || strcmp(ln,"P3\r")==0 
                 || strcmp(ln,"P3\n\r")==0 || strcmp(ln,"P3\r\n")==0 ) {
            *samplesPerPixel = 3;
            return read_ascii_ppm_file( fname, w, h, min, max );
        } else if ( strcmp(ln,"P5\n")==0   || strcmp(ln,"P5\r")==0 
                 || strcmp(ln,"P5\n\r")==0 || strcmp(ln,"P5\r\n")==0 ) {
            *samplesPerPixel = 1;
            return read_binary_pgm_file( fname, w, h, min, max );
        } else if ( strcmp(ln,"P6\n")==0   || strcmp(ln,"P6\r")==0 
                 || strcmp(ln,"P6\n\r")==0 || strcmp(ln,"P6\r\n")==0 ) {
            *samplesPerPixel = 3;
            return read_binary_ppm_file( fname, w, h, min, max );
        } else {
            char  buff[BUFSIZ];
            sprintf(buff,
              "input image file: %s, is not a proper pgm formatted file", fname);
            usage( buff );
        }
        return NULL;
    }
//----------------------------------------------------------------------
/** \brief This function reads an ascii grey pgm file.
 *
 *  This type of file is formatted as follows:
 *  <pre>
 *  P2
 *  w h
 *  maxval
 *  v_1 v_2 v_3 . . . v_w*h
 *  </pre>
 *
 *  It's the caller's responsibility to free the malloc'd data.
 */
static int* read_ascii_pgm_file ( const char* const fname, int* w, int* h,
                                  int* min, int* max )
{
    assert( fname!=NULL && w!=NULL && h!=NULL && min!=NULL && max!=NULL );
    *w = *h = *min = *max = 0;
    if (strlen(fname) == 0)    usage("bad input file name");
    FILE*  fp = fopen(fname, "rb");
    if (fp == NULL)    usage("can't open the input file");
    char ln[BUFSIZ];
    //get the first non-comment line
    for ( ; ; ) {
        ln[0] = 0;
        fgets(ln, sizeof ln, fp);
        if (ln[0] != '#')    break;
    }
    if ( strcmp(ln,"P2\n")!=0   && strcmp(ln,"P2\r")!=0 
      && strcmp(ln,"P2\n\r")!=0 && strcmp(ln,"P2\r\n")!=0 ) {
        char buff[1000];
        sprintf(buff,
          "input image file: %s, is not a proper pgm formatted file", fname);
        usage( buff );
    }
    //get the next non-comment line
    for ( ; ; ) {
        ln[0] = 0;
        fgets(ln, sizeof ln, fp);
        if (ln[0] != '#')    break;
    }
    //get the width and height
    int  c = sscanf(ln, "%d %d", w, h);
    if (c != 2)
        usage("input image file is not a proper pgm formatted file");
    int*  slice = (int*)malloc(*w * *h * sizeof *slice);
    if (slice == NULL)    usage("out of memory");
    //get the next non-comment line (should be the max value)
    for ( ; ; ) {
        ln[0] = 0;
        fgets(ln, sizeof ln, fp);
        if (ln[0] != '#')    break;
    }
    //forget the max value (above) & read the actual data (we're assuming
    // that there aren't any comments between the max value and the data)
    int  myMin=INT_MAX, myMax=INT_MIN;
    int  i=0;
    for (int y=0; y<*h; y++) {
        for (int x=0; x<*w; x++) {
            c = fscanf(fp, "%d", &slice[i]);
            if (c!=1)    usage("error reading input file");
            if (slice[i]<myMin)    myMin=slice[i];
            if (slice[i]>myMax)    myMax=slice[i];
            i++;
        }
    }
    *min = myMin;
    *max = myMax;
    fclose(fp);    fp=NULL;
    return slice;
}
//----------------------------------------------------------------------
/** \brief This function reads an ascii color ppm file.
 *
 *  This type of file is formatted as follows:
 *  <pre>
 *  P3
 *  w h
 *  maxval
 *  vr_1 vg_1 vb_1 
 *  vr_2 vg_2 vb_2
 *  . . .
 *  vr_w*h vg_w*h vb_w*h
 *  </pre>
 *
 *  It's the caller's responsibility to free the malloc'd data.
 */
static int* read_ascii_ppm_file ( const char* const fname, int* w, int* h,
                                 int* min, int* max )
{
    assert( fname!=NULL && w!=NULL && h!=NULL && min!=NULL && max!=NULL );
    *w = *h = *min = *max = 0;
    if (strlen(fname) == 0)    usage("bad input file name");
    FILE*  fp = fopen(fname, "rb");
    if (fp == NULL)    usage("can't open the input file");
    char ln[BUFSIZ];
    //get the first non-comment line
    for ( ; ; ) {
        ln[0] = 0;
        fgets(ln, sizeof ln, fp);
        if (ln[0] != '#')    break;
    }
    if ( strcmp(ln,"P3\n")!=0   && strcmp(ln,"P3\r")!=0 
      && strcmp(ln,"P3\n\r")!=0 && strcmp(ln,"P3\r\n")!=0 ) {
        char buff[1000];
        sprintf(buff,
          "input image file: %s, is not a proper pgm formatted file", fname);
        usage( buff );
    }
    //get the next non-comment line
    for ( ; ; ) {
        ln[0] = 0;
        fgets(ln, sizeof ln, fp);
        if (ln[0] != '#')    break;
    }
    //get the width and height
    int  c = sscanf(ln, "%d %d", w, h);
    if (c != 2)
        usage("input image file is not a proper pgm formatted file");
    int*  slice = (int*)malloc(3 * *w * *h * sizeof *slice);
    if (slice == NULL)    usage("out of memory");
    //get the next non-comment line (should be the max value)
    for ( ; ; ) {
        ln[0] = 0;
        fgets(ln, sizeof ln, fp);
        if (ln[0] != '#')    break;
    }
    //forget the max value (above) & read the actual data (we're assuming
    // that there aren't any comments between the max value and the data)
    int  myMin=INT_MAX, myMax=INT_MIN;
    int  i=0;
    for (int y=0; y<*h; y++) {
        for (int x=0; x<*w; x++) {
            c = fscanf( fp, "%d %d %d", &slice[i], &slice[i+1], &slice[i+2] );
            if (c!=3)    usage("error reading input file");
            if (slice[i]<myMin)      myMin=slice[i];
            if (slice[i]>myMax)      myMax=slice[i];
            if (slice[i+1]<myMin)    myMin=slice[i+1];
            if (slice[i+1]>myMax)    myMax=slice[i+1];
            if (slice[i+2]<myMin)    myMin=slice[i+2];
            if (slice[i+2]>myMax)    myMax=slice[i+2];
            i+=3;
        }
    }
    *min = myMin;
    *max = myMax;
    fclose(fp);    fp=NULL;
    return slice;
}
//----------------------------------------------------------------------
/** \brief This function reads a binary grey pgm file.
 *
 *  This type of file is formatted as follows:
 *  <pre>
 *  P5
 *  w h
 *  maxval
 *  v_1 v_2 v_3 . . . v_w*h
 *  </pre>
 *
 *  It's the caller's responsibility to free the malloc'd data.
 */
static int* read_binary_pgm_file ( const char* const fname, int* w, int* h,
                                   int* min, int* max )
{
    assert( fname!=NULL && w!=NULL && h!=NULL && min!=NULL && max!=NULL );
    *w = *h = *min = *max = 0;
    if (strlen(fname) == 0)    usage("bad input file name");
    FILE*  fp = fopen(fname, "rb");
    if (fp == NULL)    usage("can't open the input file");
    char ln[BUFSIZ];
    //get the first non-comment line
    for ( ; ; ) {
        ln[0] = 0;
        fgets(ln, sizeof ln, fp);
        if (ln[0] != '#')    break;
    }
    if ( strcmp(ln,"P5\n")!=0   && strcmp(ln,"P5\r")!=0 
      && strcmp(ln,"P5\n\r")!=0 && strcmp(ln,"P5\r\n")!=0 ) {
        char buff[1000];
        sprintf(buff,
          "input image file: %s, is not a proper pgm formatted file", fname);
        usage( buff );
    }
    //get the next non-comment line
    for ( ; ; ) {
        ln[0] = 0;
        fgets(ln, sizeof ln, fp);
        if (ln[0] != '#')    break;
    }
    //get the width and height
    int  c = sscanf(ln, "%d %d", w, h);
    if (c != 2)
        usage("input image file is not a proper pgm formatted file");
    int*  slice = (int*)malloc(*w * *h * sizeof *slice);
    if (slice == NULL)    usage("out of memory");
    //get the next non-comment line (should be the max value)
    for ( ; ; ) {
        ln[0] = 0;
        fgets(ln, sizeof ln, fp);
        if (ln[0] != '#')    break;
    }
    //forget the max value (above) & read the actual data (we're assuming
    // that there aren't any comments between the max value and the data)
    int  myMin=INT_MAX, myMax=INT_MIN;
    int  i=0;
    for (int y=0; y<*h; y++) {
        for (int x=0; x<*w; x++) {
            unsigned char  uc;
            c = fread( &uc, sizeof uc, 1, fp );
            if (c != 1)    usage("error reading input file");
            slice[i] = uc;
            if (slice[i]<myMin)    myMin=slice[i];
            if (slice[i]>myMax)    myMax=slice[i];
            i++;
        }
    }
    *min = myMin;
    *max = myMax;
    fclose(fp);    fp=NULL;
    return slice;
}
//----------------------------------------------------------------------
/** \brief This function reads a binary color (rgb) ppm file.
 *
 *  This type of file is formatted as follows:
 *  <pre>
 *  P6
 *  w h
 *  maxval
 *  vr_1 vg_1 vb_1 
 *  vr_2 vg_2 vb_2
 *  . . .
 *  vr_w*h vg_w*h vb_w*h
 *  </pre>
 *
 *  It's the caller's responsibility to free the malloc'd data.
 */
static int* read_binary_ppm_file ( const char* const fname, int* w, int* h,
                                   int* min, int* max )
{
    assert( fname!=NULL && w!=NULL && h!=NULL && min!=NULL && max!=NULL );
    *w = *h = *min = *max = 0;
    if (strlen(fname) == 0)    usage("bad input file name");
    FILE*  fp = fopen(fname, "rb");
    if (fp == NULL)    usage("can't open the input file");
    char ln[BUFSIZ];
    //get the first non-comment line
    for ( ; ; ) {
        ln[0] = 0;
        fgets(ln, sizeof ln, fp);
        if (ln[0] != '#')    break;
    }
    if ( strcmp(ln,"P6\n")!=0   && strcmp(ln,"P6\r")!=0 
      && strcmp(ln,"P6\n\r")!=0 && strcmp(ln,"P6\r\n")!=0 ) {
        char buff[1000];
        sprintf(buff,
          "input image file: %s, is not a proper pgm formatted file", fname);
        usage( buff );
    }
    //get the next non-comment line
    for ( ; ; ) {
        ln[0] = 0;
        fgets(ln, sizeof ln, fp);
        if (ln[0] != '#')    break;
    }
    //get the width and height
    int  c = sscanf(ln, "%d %d", w, h);
    if (c != 2)
        usage("input image file is not a proper pgm formatted file");
    int*  slice = (int*)malloc(3 * *w * *h * sizeof *slice);
    if (slice == NULL)    usage("out of memory");
    //get the next non-comment line (should be the max value)
    for ( ; ; ) {
        ln[0] = 0;
        fgets(ln, sizeof ln, fp);
        if (ln[0] != '#')    break;
    }
    //forget the max value (above) & read the actual data (we're assuming
    // that there aren't any comments between the max value and the data)
    int  myMin=INT_MAX, myMax=INT_MIN;
    int  i=0;
    for (int y=0; y<*h; y++) {
        for (int x=0; x<*w; x++) {
            unsigned char  ucr, ucg, ucb;

            c = fread( &ucr, sizeof ucr, 1, fp );
            if (c != 1)    usage("error reading input file");
            slice[i] = ucr;
            if (slice[i]<myMin)    myMin=slice[i];
            if (slice[i]>myMax)    myMax=slice[i];
            i++;
            
            c = fread( &ucg, sizeof ucg, 1, fp );
            if (c != 1)    usage("error reading input file");
            slice[i] = ucg;
            if (slice[i]<myMin)    myMin=slice[i];
            if (slice[i]>myMax)    myMax=slice[i];
            i++;
            
            c = fread( &ucb, sizeof ucb, 1, fp );
            if (c != 1)    usage("error reading input file");
            slice[i] = ucb;
            if (slice[i]<myMin)    myMin=slice[i];
            if (slice[i]>myMax)    myMax=slice[i];
            i++;
        }
    }
    *min = myMin;
    *max = myMax;
    fclose(fp);    fp=NULL;
    return slice;
}
//----------------------------------------------------------------------
/** \brief Read 16-bit values as a binary pgm file.
 *
 *  It's the caller's responsibility to free the malloc'd data.
 */
static void read_binary_pgm16_file ( const char* const fname, short** sp,
                                     int* wp, int* hp )
{
    *wp = *hp = 0;
    assert( fname != NULL && strlen(fname)>0 && sp != NULL && wp != NULL && hp != NULL );
    FILE*  fp = fopen(fname, "rb");
    if (fp==NULL)    usage( "read_raw_pgm16_file: can't open input file " );
    char ln[BUFSIZ];
    //get the first non-comment line
    for ( ; ; ) {
        ln[0] = 0;
        fgets(ln, sizeof ln, fp);
        if (ln[0] != '#')    break;
    }
    //determine the byte order (just like tiff)
    char   byteOrder;
    short  x = 1;
    char*  p = (char*) &x;
    if      (p[0] == 1 && p[1] == 0)    byteOrder = 'I';
    else if (p[0] == 0 && p[1] == 1)    byteOrder = 'M';
    else                                assert(0);
    char  buff[BUFSIZ];
    sprintf(buff, "P5-16-%c%c\n", byteOrder, byteOrder);
    assert(strcmp(ln,buff) == 0);
    //get the next non-comment line
    for ( ; ; ) {
        ln[0] = 0;
        fgets(ln, sizeof ln, fp);
        if (ln[0] != '#')    break;
    }
    //get the width and height
    int     c = sscanf(ln, "%d %d", wp, hp);    assert(c == 2);
    short*  slice;
    slice = (short*)malloc(*wp * *hp * sizeof *slice);    assert(slice != NULL);
    //get the next non-comment line (should be the max value)
    for ( ; ; ) {
        ln[0] = 0;
        fgets(ln, sizeof ln, fp);
        if (ln[0] != '#')    break;
    }
    int  maxval;
    c = sscanf(ln, "%d", &maxval);    assert(c == 1);
    assert( maxval <= SHRT_MAX );
    //skip the max value & read the actual data (we're assuming that
    //there aren't any comments between the max value and the data)
    c = fread(slice, *wp * *hp * sizeof *slice, 1, fp);  assert(c == 1);
    fclose(fp);    fp=NULL;
    *sp = slice;
}
//----------------------------------------------------------------------
/** \brief Write values as a pgm (grey) or ppm (color) ascii file.
 */
static void write_pgm_or_ppm_ascii_data ( const int* const buff,
    const int width, const int height, const char* const fname,
    const int samples_per_pixel )
{
    long  i, count, maxval=LONG_MIN;

    if (fname == NULL || strlen(fname) == 0)  usage("bad input file name");
    FILE*  fp = fopen(fname, "wb");
    if (fp == NULL)  usage("can't open the input file");

    if      (samples_per_pixel==1)    fputs("P2\n", fp);    //grey
    else if (samples_per_pixel==3)    fputs("P3\n", fp);    //color
    else                              assert(0);

    fputs("# created by george (ASCII, obviously)\n", fp);
    fprintf(fp, "%d %d\n", width, height);
    for (i=0; i<(width*height*samples_per_pixel); i++)  {
        if (buff[i] > maxval)    maxval = buff[i];
    }

    if (maxval == 0)    maxval = 255;
    fprintf(fp, "%d\n", maxval);

    for (count=i=0; i<(width*height*samples_per_pixel); i++,count++)  {
        //fprintf(fp, " %*d", output_width, buff[i]);
        fprintf(fp, " %d", buff[i]);
        if (count > 10)  {  fputs("\n", fp);  count = 0;  }
    }
    fputs("\n", fp);
}
//----------------------------------------------------------------------
/** \brief Write 32-bit values as a raw (binary) pgm file.
 */
static void write_raw_pgm_data32 ( int* buff, int width, int height, 
                                  const char* const fname )
{
    if (fname == NULL || strlen(fname) == 0)    usage("bad input file name");
    FILE*  fp = fopen(fname, "wb");
    if (fp == NULL)    usage("can't open the input file");

    //determine the byte order (just like tiff)

    char  byteOrder;
    short x = 1;
    char* p = (char*) &x;
    if      (p[0] == 1 && p[1] == 0)  byteOrder = 'I';
    else if (p[0] == 0 && p[1] == 1)  byteOrder = 'M';
    else                              assert(0);

    fprintf(fp, "P5-32-%c%c\n", byteOrder, byteOrder);
    fputs("# created by dicom2pgm (raw-32, not-so-obviously)\n", fp);
    fprintf(fp, "%d %d\n", width, height);
    //determine the greatest value
    long i, maxval=LONG_MIN;
    for (i=0; i < width*height; i++)
        if (buff[i] > maxval)  maxval = buff[i];
    //default if necessary
    if (maxval == 0)  maxval = 255;
    fprintf(fp, "%d\n", maxval);
    //write out the data
    for (i=0; i < width*height; i++) {
//        if (unsigned_flag) {
//            uint32 x=buff[i];
//            fwrite(&x, 1, sizeof x, fp);
//        }
//        else {
            int  x=buff[i];
            fwrite(&x, 1, sizeof x, fp);
//        }
    }
}
//----------------------------------------------------------------------
/** \brief Write 16-bit values as a raw (binary) pgm file.
 */
static void write_raw_pgm_data16 ( int* buff, int width, int height, 
                                   const char* const fname )
{
    if (fname == NULL || strlen(fname) == 0)  usage("bad input file name");
    FILE*  fp = fopen(fname, "wb");
    if (fp == NULL)  usage("can't open the input file");
    fputs("write_raw_pgm_data16\n", stderr);

    //determine the byte order (just like tiff)

    char  byteOrder;
    short x = 1;
    char* p = (char*) &x;
    if      (p[0] == 1 && p[1] == 0)  byteOrder = 'I';
    else if (p[0] == 0 && p[1] == 1)  byteOrder = 'M';
    else                              assert(0);

    fprintf(fp, "P5-16-%c%c\n", byteOrder, byteOrder);
    fprintf(fp, "# created by dicom2pgm (raw-16, not-so-obviously)\n");

//#define PAD
#ifdef  PAD //so DMA can rip!
    int wide = width;
    if (height>width)  wide=height;
    int extra = ((wide + 0x3f) & ~0x3f) - wide;
    if (extra != 0) {
        if (verbose_flag)
            fprintf(stderr, "padding each row with %d\n", extra);
        int cols = wide + extra;
        int32* tmp = (int*)malloc(height * cols * sizeof *tmp);
        assert(tmp!=NULL);
        for (int i=0; i<height*cols; i++)  tmp[i]=0;
        for (int y=0; y<height; y++) {
            for (x=0; x<width; x++) {
                tmp[y*cols + x] = buff[y*width + x];
            }
        }
        width = cols;
        buff  = tmp;
    }
#endif

    fprintf(fp, "%d %d\n", width, height);
    //determine the greatest value
    long i, maxval=LONG_MIN;
    for (i=0; i < width*height; i++)
        if (buff[i] > maxval)  maxval = buff[i];
    //default if necessary
    if (maxval == 0)  maxval = 255;
    assert(maxval <= SHRT_MAX);
    fprintf(fp, "%d\n", maxval);
    //write out the data
    for (i=0; i < width*height; i++) {
//        if (unsigned_flag) {
//            uint16 x=buff[i];
//            fwrite(&x, 1, sizeof x, fp);
//        } else {
            short  x=buff[i];
            fwrite(&x, 1, sizeof x, fp);
//        }
    }

#ifdef  PAD //so DMA can rip!
    if (extra!=0) { free(buff); buff=NULL; }
#endif
}
//----------------------------------------------------------------------
/** \brief Write 8-bit values as a binary pgm file.
 */
static void write_binary_pgm_or_ppm_data8 ( const unsigned char* const buff,
    const int width, const int height, const char* const fname,
    const int samples_per_pixel )
{
    if (fname == NULL || strlen(fname) == 0)  usage("bad input file name");
    FILE*  fp = fopen(fname, "wb");
    if (fp == NULL)  usage("can't open the input file");

    long i, maxval=LONG_MIN;

    if      (samples_per_pixel==1)    fputs("P5\n", fp);  //grey
    else if (samples_per_pixel==3)    fputs("P6\n", fp);  //color
    else                              assert(0);

    fputs("# created by dicom2pgm (raw-8, not-so-obviously)\n", fp);
    fprintf(fp, "%d %d\n", width, height);
    for (i=0; i<width*height*samples_per_pixel; i++)
        if (buff[i] > maxval)  maxval = buff[i];

    if (maxval == 0)  maxval = 255;
    fprintf(fp, "%d\n", maxval);

    fwrite(buff, width*height*samples_per_pixel, sizeof *buff, fp);
}
//----------------------------------------------------------------------

};
#endif
//----------------------------------------------------------------------
