/**
    \file TIFFWriter.cpp
    This file contains code that will output a TIFF image file.

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
//----------------------------------------------------------------------
#include "stdafx.h"
#undef   NDEBUG
#include <assert.h>
#include <float.h>
#include <stdio.h>

#include "TIFFWriter.h"

CLUT  clut;

struct  {
    //define each of the entry tags
#   define ImageWidth                256
#   define ImageLength               257
#   define BitsPerSample             258
#   define Compression               259
#   define PhotometricInterpretation 262
#   define StripOffsets              273
#   define SamplesPerPixel           277
#   define RowsPerStrip              278
#   define StripByteCounts           279
#   define XResolution               282
#   define YResolution               283
#   define ResolutionUnit            296
    uint16 tag;    //tag for the field

#   define BYTE     1
#   define ASCII    2
#   define SHORT    3
#   define LONG     4
#   define RATIONAL 5
    uint16 type;   //field type

    uint32 count;  //length (not in bytes but the count of the specific data type)
    union  {       //value or value offset
        uint16 sval;  //unsigned short
        uint32 ival;  //unsigned int
        uint32 offset;
    } v;
} entry;
//----------------------------------------------------------------------
void TIFFWriter::write_tiff_data8_rgb ( const uint8* const buff,
    const int width, const int height,
    FILE* fp, const bool use_clut, const int samples_per_pixel )
{
//note: either specify use_clut or samples_per_pixel
// (1=when using clut; 3=when specifying individual rgb)

//
//    0:  II or MM
//    2:  42
//    4:  offset to first image file directory (ifd)
//    8:    number of entries (12)
//   10:    entry 1: width
//   22:    entry 2: length
//   32:    entry 3: bits per sample
//   44:    entry 4: compression
//   56:    entry 5: photometric interpretation
//   68:    entry 6: strip offsets
//   80:    entry 7: samples per pixel
//   92:    entry 8: rows per strip
//  104:    entry 9: strip byte counts
//  116:    entry 10:x resolution
//  128:    entry 11:y resolution
//  140:    entry 12:resolution unit
//  152:  offset to next ifd
//  160:  bits-per-sample-r, bits-per-sample-g, bits-per-sample-b
//  172:  x res numerator
//  166:  x res denominator
//  170:  y res numerator
//  174:  y res denominator
//  178:  pixel data (rgb)
//
    int    n;
    uint8  ui8;
    uint16 ui16;
    uint32 ui32;
    /*
    fill in the image file header (ifh)
    */
    // determine the byte order
    short x = 1;
    char* p = (char*) &x;
    if (p[0] == 1 && p[1] == 0)       ui8 = 'I';
    else if (p[0] == 0 && p[1] == 1)  ui8 = 'M';
    else                              assert(0);
    n = fwrite(&ui8, sizeof ui8, 1, fp);                   assert(n == 1);
    n = fwrite(&ui8, sizeof ui8, 1, fp);                   assert(n == 1);

    ui16 = 42;
    n = fwrite(&ui16, sizeof ui16, 1, fp);                 assert(n == 1);

    ui32 = sizeof ui16 +
           sizeof ui16 +
           sizeof ui32;  // offset to first IFD
    n = fwrite(&ui32, sizeof ui32, 1, fp);                 assert(n == 1);
    /*
    fill in the image file directory
    */
    ui16 = 12;  // # of entries
    n = fwrite(&ui16, sizeof ui16, 1, fp);                 assert(n == 1);

    assert(sizeof(entry) == sizeof(uint16) +
                            sizeof(uint16) +
                            sizeof(uint32) +
                            sizeof(uint32));

    entry.tag      = ImageWidth;
    entry.type     = LONG;
    entry.count    = 1;
    entry.v.ival   = width;
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);

    entry.tag      = ImageLength;
    entry.type     = LONG;
    entry.count    = 1;
    entry.v.ival   = height;
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);

    entry.tag      = BitsPerSample;
    entry.type     = SHORT;
    entry.count    = 3;
    entry.v.ival   = sizeof ui16 +
                     sizeof ui16 +
                     sizeof ui32 +
                     sizeof ui16 +
                     12 * sizeof entry +
                     sizeof ui32;
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);

    entry.tag      = Compression;
    entry.type     = SHORT;
    entry.count    = 1;
    entry.v.ival   = 0;  //zero the whole union out
    entry.v.sval   = 1;
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);

    entry.tag      = PhotometricInterpretation;
    entry.type     = SHORT;
    entry.count    = 1;
    entry.v.ival   = 0;  //zero the whole union out
    entry.v.sval   = 2;  //rgb
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);

    entry.tag      = StripOffsets;
    entry.type     = LONG;
    entry.count    = 1;
    entry.v.ival   = sizeof ui16 +
                     sizeof ui16 +
                     sizeof ui32 +
                     sizeof ui16 +
                     12 * sizeof entry +
                     sizeof ui32 +
                     3 * sizeof ui16 +
                     2 * sizeof ui32 +
                     2 * sizeof ui32;
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);

    entry.tag      = SamplesPerPixel;
    entry.type     = SHORT;
    entry.count    = 1;
    entry.v.ival   = 0;  //zero the whole union out
    entry.v.sval   = 3;
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);

    entry.tag      = RowsPerStrip;
    entry.type     = LONG;
    entry.count    = 1;
    entry.v.ival   = height;
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);

    entry.tag      = StripByteCounts;
    entry.type     = LONG;
    entry.count    = 1;
    entry.v.ival   = width * height * 3;
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);

    entry.tag      = XResolution;
    entry.type     = RATIONAL;
    entry.count    = 1;
    entry.v.offset = sizeof ui16 +
                     sizeof ui16 +
                     sizeof ui32 +
                     sizeof ui16 +
                     12 * sizeof entry +
                     sizeof ui32 +
                     3 * sizeof ui16;
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);

    entry.tag      = YResolution;
    entry.type     = RATIONAL;
    entry.count    = 1;
    entry.v.offset = sizeof ui16 +
                     sizeof ui16 +
                     sizeof ui32 +
                     sizeof ui16 +
                     12 * sizeof entry +
                     sizeof ui32 +
                     3 * sizeof ui16 +
                     2 * sizeof ui32;
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);

    entry.tag     = ResolutionUnit;
    entry.type    = SHORT;
    entry.count   = 1;
    entry.v.ival  = 0;  //zero the whole union out
    entry.v.sval  = 1;
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);
    /*
    fill in all of the entries
    */
    /* offset to the next ifd */
    ui32 = 0;
    n = fwrite(&ui32, sizeof ui32, 1, fp);                 assert(n == 1);

    //bits per sample r,g,b
    ui16 = 8;
    n = fwrite(&ui16, sizeof ui16, 1, fp);                 assert(n == 1);
    n = fwrite(&ui16, sizeof ui16, 1, fp);                 assert(n == 1);
    n = fwrite(&ui16, sizeof ui16, 1, fp);                 assert(n == 1);

    /* x res numerator */
    ui32 = 1;
    n = fwrite(&ui32, sizeof ui32, 1, fp);                 assert(n == 1);
    /* x res denominator */
    n = fwrite(&ui32, sizeof ui32, 1, fp);                 assert(n == 1);
    /* y res numerator */
    n = fwrite(&ui32, sizeof ui32, 1, fp);                 assert(n == 1);
    /* y res denominator */
    n = fwrite(&ui32, sizeof ui32, 1, fp);                 assert(n == 1);

    //write the actual pixel data

    if (!use_clut) {
        assert( samples_per_pixel==3 );
        n = fwrite(buff, height*width*sizeof(*buff)*samples_per_pixel, 1, fp);
        assert(n == 1);
    } else {
        for (int i=0; i<height*width; i++) {
            uint8  rgb[3];
            const int  x=buff[i];

            if (clut.r_num_bits == 8) {
                if (x<clut.r_first_value)                       rgb[0] = clut.r_table8[0];
                else if (x>=clut.r_entries+clut.r_first_value)  rgb[0] = clut.r_table8[clut.r_entries-1];
                else                                            rgb[0] = clut.r_table8[x-clut.r_first_value];
            }
            else if (clut.r_num_bits == 16) {
                uint32 r;
                if (x<clut.r_first_value)                       r = clut.r_table16[0];
                else if (x>=clut.r_entries+clut.r_first_value)  r = clut.r_table16[clut.r_entries-1];
                else                                            r = clut.r_table16[x-clut.r_first_value];
                assert(r<=255 && r>=0);
                rgb[0] = r;
            }
            else { assert(0); }

            if (clut.g_num_bits == 8) {
                if (x<clut.g_first_value)                       rgb[1] = clut.g_table8[0];
                else if (x>=clut.g_entries+clut.g_first_value)  rgb[1] = clut.g_table8[clut.g_entries-1];
                else                                            rgb[1] = clut.g_table8[x-clut.g_first_value];
            }
            else if (clut.g_num_bits == 16) {
                uint32 g;
                if (x<clut.g_first_value)                       g = clut.g_table16[0];
                else if (x>=clut.g_entries+clut.g_first_value)  g = clut.g_table16[clut.g_entries-1];
                else                                            g = clut.g_table16[x-clut.g_first_value];
                assert(g<=255 && g>=0);
                rgb[1] = g;
            }
            else { assert(0); }

            if (clut.b_num_bits == 8) {
                if (x<clut.b_first_value)                       rgb[2] = clut.b_table8[0];
                else if (x>=clut.b_entries+clut.b_first_value)  rgb[2] = clut.b_table8[clut.b_entries-1];
                else                                            rgb[2] = clut.b_table8[x-clut.b_first_value];
            }
            else if (clut.b_num_bits == 16) {
                uint32 b;
                if (x<clut.b_first_value)                       b = clut.b_table16[0];
                else if (x>=clut.b_entries+clut.b_first_value)  b = clut.b_table16[clut.b_entries-1];
                else                                            b = clut.b_table16[x-clut.b_first_value];
                assert(b<=255 && b>=0);
                rgb[2] = b;
            }
            else { assert(0); }

            n = fwrite(rgb, sizeof rgb, 1, fp);
            assert(n == 1);
        }
    }
}
//----------------------------------------------------------------------
void TIFFWriter::write_tiff_float_grey ( const float* const buff,
    const int width, const int height, const char* const fname )
{
    float  max = 0;
    int    i;
    for (i=0; i<width*height; i++) {
        if (buff[i]<FLT_MAX && buff[i]>max)  max=buff[i];
    }

    //printf( "TIFFWriter: max=%f \n", max );

    uint8* u8buff = new unsigned char[height*width];
    for (i=0; i<width*height; i++) {
        if (buff[i]<FLT_MAX) {
            double d = (double)buff[i] / max * 255 + 0.5;
            int tmp = (int)d;
            if (tmp<  0)  tmp=  0;
            if (tmp>255)  tmp=255;
            u8buff[i] = tmp;
        } else {
            u8buff[i] = 255;
        }
        u8buff[i] = 255-u8buff[i];  //invert
    }

    FILE* fp = fopen(fname, "wb");  assert(fp!=NULL);
    TIFFWriter::write_tiff_data8_grey(u8buff, width, height, fp );
    fclose(fp);  fp=NULL;

    delete u8buff;  u8buff=NULL;
}
//----------------------------------------------------------------------
void TIFFWriter::write_tiff_double_grey ( const double* const buff,
    const int width, const int height, const char* const fname )
{
    double max = 0;
    int i;
    for (i=0; i<width*height; i++) {
        if (buff[i]<FLT_MAX && buff[i]>max)  max=buff[i];
    }

    //printf( "TIFFWriter: max=%f \n", max );

    uint8* u8buff = new unsigned char[height*width];
    for (i=0; i<width*height; i++) {
        if (buff[i]<FLT_MAX) {
            double d = (double)buff[i] / max * 255 + 0.5;
            int tmp = (int)d;
            if (tmp<  0)  tmp=  0;
            if (tmp>255)  tmp=255;
            u8buff[i] = tmp;
        } else {
            u8buff[i] = 255;
        }
        u8buff[i] = 255-u8buff[i];  //invert
    }

    FILE* fp = fopen(fname, "wb");  assert(fp!=NULL);
    TIFFWriter::write_tiff_data8_grey(u8buff, width, height, fp );
    fclose(fp);  fp=NULL;

    delete u8buff;  u8buff=NULL;
}
//----------------------------------------------------------------------
void TIFFWriter::write_tiff_data8_grey ( const uint8* const buff,
    const int width, const int height, FILE* fp )
{
//
//    0:  II or MM
//    2:  42
//    4:  offset to first image file directory (ifd)
//    8:    number of entries (11)
//   10:    entry 1: width
//   22:    entry 2: length
//   32:    entry 3: bits per sample
//   44:    entry 4: compression
//   56:    entry 5: photometric interpretation
//   68:    entry 6: strip offsets
//   80:    entry 7: rows per strip
//   92:    entry 8: strip byte counts
//  104:    entry 9: x resolution
//  116:    entry 10:y resolution
//  128:    entry 11:resolution unit
//  140:  offset to next ifd
//  144:  x res numerator
//  148:  x res denominator
//  152:  y res numerator
//  156:  y res denominator
//  160:  pixel data (grey)
//
    const int  samples_per_pixel = 1;
    int    n;
    uint8  ui8;
    uint16 ui16;
    uint32 ui32;
    /*
    fill in the image file header (ifh)
    */
    // determine the byte order
    short x = 1;
    char* p = (char*) &x;
    if      (p[0] == 1 && p[1] == 0)  ui8 = 'I';
    else if (p[0] == 0 && p[1] == 1)  ui8 = 'M';
    else                              assert(0);
    n = fwrite( &ui8, sizeof ui8, 1, fp );                 assert(n == 1);
    n = fwrite( &ui8, sizeof ui8, 1, fp );                 assert(n == 1);

    ui16 = 42;
    n = fwrite(&ui16, sizeof ui16, 1, fp);                 assert(n == 1);

    ui32 = sizeof ui16 +
           sizeof ui16 +
           sizeof ui32;  // offset to first IFD
    n = fwrite(&ui32, sizeof ui32, 1, fp);                 assert(n == 1);
    /*
    fill in the image file directory
    */
    ui16 = 11;  // # of entries
    n = fwrite(&ui16, sizeof ui16, 1, fp);                 assert(n == 1);

    assert(sizeof(entry) == sizeof(uint16) +
                            sizeof(uint16) +
                            sizeof(uint32) +
                            sizeof(uint32));

    entry.tag      = ImageWidth;
    entry.type     = LONG;
    entry.count    = 1;
    entry.v.ival   = width;
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);

    entry.tag      = ImageLength;
    entry.type     = LONG;
    entry.count    = 1;
    entry.v.ival   = height;
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);

    entry.tag      = BitsPerSample;
    entry.type     = SHORT;
    entry.count    = samples_per_pixel;
    entry.v.ival   = 0;  //zero the whole union out
    entry.v.sval   = 8;
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);

    entry.tag      = Compression;
    entry.type     = SHORT;
    entry.count    = 1;
    entry.v.ival   = 0;  //zero the whole union out
    entry.v.sval   = 1;
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);

    entry.tag      = PhotometricInterpretation;
    entry.type     = SHORT;
    entry.count    = 1;
    entry.v.ival   = 0;  //zero the whole union out
    entry.v.sval   = 0;  //grey or binary - 0 is black
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);

    entry.tag      = StripOffsets;
    entry.type     = LONG;
    entry.count    = 1;
    entry.v.ival   = sizeof ui16 +
                     sizeof ui16 +
                     sizeof ui32 +
                     sizeof ui16 +
                     11 * sizeof entry +
                     sizeof ui32 +
                     2 * sizeof ui32 +
                     2 * sizeof ui32;
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);

    entry.tag      = RowsPerStrip;
    entry.type     = LONG;
    entry.count    = 1;
    entry.v.ival   = height;
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);

    entry.tag      = StripByteCounts;
    entry.type     = LONG;
    entry.count    = 1;
    entry.v.ival   = width * height;
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);

    entry.tag      = XResolution;
    entry.type     = RATIONAL;
    entry.count    = 1;
    entry.v.offset = sizeof ui16 +
                     sizeof ui16 +
                     sizeof ui32 +
                     sizeof ui16 +
                     11 * sizeof entry +
                     sizeof ui32;
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);

    entry.tag      = YResolution;
    entry.type     = RATIONAL;
    entry.count    = 1;
    entry.v.offset = sizeof ui16 +
                     sizeof ui16 +
                     sizeof ui32 +
                     sizeof ui16 +
                     11 * sizeof entry +
                     sizeof ui32 +
                     2 * sizeof ui32;
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);

    entry.tag     = ResolutionUnit;
    entry.type    = SHORT;
    entry.count   = 1;
    entry.v.ival  = 0;  //zero the whole union out
    entry.v.sval  = 1;
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);
    /*
    fill in all of the entries
    */
    /* offset to the next ifd */
    ui32 = 0;
    n = fwrite(&ui32, sizeof ui32, 1, fp);                 assert(n == 1);
    /* x res numerator */
    ui32 = 1;
    n = fwrite(&ui32, sizeof ui32, 1, fp);                 assert(n == 1);
    /* x res denominator */
    n = fwrite(&ui32, sizeof ui32, 1, fp);                 assert(n == 1);
    /* y res numerator */
    n = fwrite(&ui32, sizeof ui32, 1, fp);                 assert(n == 1);
    /* y res denominator */
    n = fwrite(&ui32, sizeof ui32, 1, fp);                 assert(n == 1);

    //write the actual pixel data

    n = fwrite(buff, height*width*sizeof(*buff)*samples_per_pixel, 1, fp);
    assert(n == 1);
}
//----------------------------------------------------------------------
void TIFFWriter::write_tiff_data16 ( const uint16* const buff,
    const int width, const int height, FILE* fp )
{
    int     n;
    uint8   ui8;
    uint16  ui16;
    uint32  ui32;
    /*
    fill in the image file header (ifh)
    */
    // determine the byte order
    short  x = 1;
    char*  p = (char*) &x;
    if (p[0] == 1 && p[1] == 0)       ui8 = 'I';
    else if (p[0] == 0 && p[1] == 1)  ui8 = 'M';
    else                              assert(0);
    n = fwrite(&ui8, sizeof ui8, 1, fp);                   assert(n == 1);
    n = fwrite(&ui8, sizeof ui8, 1, fp);                   assert(n == 1);

    ui16 = 42;
    n = fwrite(&ui16, sizeof ui16, 1, fp);                 assert(n == 1);
    ui32 = sizeof ui16 +
           sizeof ui16 +
           sizeof ui32;  // offset to first IFD
    n = fwrite(&ui32, sizeof ui32, 1, fp);                 assert(n == 1);
    /*
    fill in the image file directory
    */
    ui16 = 11;  // # of entries
    n = fwrite(&ui16, sizeof ui16, 1, fp);                 assert(n == 1);

    assert(sizeof(entry) == sizeof(uint16) +
                            sizeof(uint16) +
                            sizeof(uint32) +
                            sizeof(uint32));

    entry.tag      = ImageWidth;
    entry.type     = LONG;
    entry.count    = 1;
    entry.v.ival   = width;
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);

    entry.tag      = ImageLength;
    entry.type     = LONG;
    entry.count    = 1;
    entry.v.ival   = height;
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);

    entry.tag      = BitsPerSample;
    entry.type     = SHORT;
    entry.count    = 1;
    entry.v.ival   = 0;  //zero the whole union out
    entry.v.sval   =16;
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);

    entry.tag      = Compression;
    entry.type     = SHORT;
    entry.count    = 1;
    entry.v.ival   = 0;  //zero the whole union out
    entry.v.sval   = 1;
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);

    entry.tag      = PhotometricInterpretation;
    entry.type     = SHORT;
    entry.count    = 1;
    entry.v.ival   = 0;  //zero the whole union out
    entry.v.sval   = 1;
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);

    entry.tag      = StripOffsets;
    entry.type     = LONG;
    entry.count    = 1;
    entry.v.ival   = sizeof ui16 +
                     sizeof ui16 +
                     sizeof ui32 +
                     sizeof ui16 +
                     11 * sizeof entry +
                     sizeof ui32 +
                     2 * sizeof ui32 +
                     2 * sizeof ui32;
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);

    entry.tag      = RowsPerStrip;
    entry.type     = LONG;
    entry.count    = 1;
    entry.v.ival   = height;
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);

    entry.tag      = StripByteCounts;
    entry.type     = LONG;
    entry.count    = 1;
    entry.v.ival   = width * height;
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);

    entry.tag      = XResolution;
    entry.type     = RATIONAL;
    entry.count    = 1;
    entry.v.offset = sizeof ui16 +
                     sizeof ui16 +
                     sizeof ui32 +
                     sizeof ui16 +
                     11 * sizeof entry +
                     sizeof ui32;
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);

    entry.tag      = YResolution;
    entry.type     = RATIONAL;
    entry.count    = 1;
    entry.v.offset = sizeof ui16 +
                     sizeof ui16 +
                     sizeof ui32 +
                     sizeof ui16 +
                     11 * sizeof entry +
                     sizeof ui32 +
                     2 * sizeof ui32;
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);

    entry.tag     = ResolutionUnit;
    entry.type    = SHORT;
    entry.count   = 1;
    entry.v.ival  = 0;  //zero the whole union out
    entry.v.sval  = 1;
    n = fwrite(&entry, sizeof entry, 1, fp);               assert(n == 1);
    /*
    fill in all of the entries
    */
    /* ofset to the next ifd */
    ui32 = 0;
    n = fwrite(&ui32, sizeof ui32, 1, fp);                 assert(n == 1);
    /* x res numerator */
    ui32 = 1;
    n = fwrite(&ui32, sizeof ui32, 1, fp);                 assert(n == 1);
    /* x res denominator */
    n = fwrite(&ui32, sizeof ui32, 1, fp);                 assert(n == 1);
    /* y res numerator */
    n = fwrite(&ui32, sizeof ui32, 1, fp);                 assert(n == 1);
    /* y res denominator */
    n = fwrite(&ui32, sizeof ui32, 1, fp);                 assert(n == 1);

    //write the actual pixel data
    n = fwrite(buff, height * width * sizeof(*buff), 1, fp);
    assert(n == 1);
}
//----------------------------------------------------------------------
