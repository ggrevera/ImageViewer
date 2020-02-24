/**
    \file TIFFWriter.h
    Header file for (definition of) TIFF image writers.

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
#ifndef TIFFWriter_h
#define TIFFWriter_h
//----------------------------------------------------------------------
#ifndef uint8
    #define uint8   unsigned char
    #define uint16  unsigned short
    #define uint32  unsigned int
#endif
//----------------------------------------------------------------------
/** \brief CLUT (Color Lookup Table) class for writing some color TIFF 
 *  image files.
 *
 *  For optional use with TIFFWriter class.
 */
class CLUT {
public:
    int  r_entries;          ///< number of table entries for red
    int  r_first_value;      ///< index (offset) of first value in table
    int  r_num_bits;         ///< number of bits per table entry (8 or 16 only)
    int  r_bytes;            ///< bytes used by table
    union {
        uint8*   r_table8;   ///< if r_num_bits == 8
        uint16*  r_table16;  ///< if r_num_bits == 16
    };

    int  g_entries;          ///< number of table entries for green
    int  g_first_value;      ///< index (offset) of first value in table
    int  g_num_bits;         ///< number of bits per table entry (8 or 16 only)
    int  g_bytes;            ///< bytes used by table
    union {
        uint8*   g_table8;   ///< if g_num_bits == 8
        uint16*  g_table16;  ///< if g_num_bits == 16
    };

    int  b_entries;          ///< number of table entries for blue
    int  b_first_value;      ///< index (offset) of first value in table
    int  b_num_bits;         ///< number of bits per table entry (8 or 16 only)
    int  b_bytes;            ///< bytes used by table
    union {
        uint8*   b_table8;   ///< if b_num_bits == 8
        uint16*  b_table16;  ///< if b_num_bits == 16
    };

    /// CLUT constructor.  Initial CLUT is empty.
    CLUT ( ) {
        this->r_entries = this->g_entries = this->b_entries = 0;
        this->r_first_value = this->g_first_value = this->b_first_value = 0;
        this->r_num_bits = this->g_num_bits = this->b_num_bits = 0;
        this->r_bytes = this->g_bytes = this->b_bytes = 0;
        this->r_table8 = this->g_table8 = this->b_table8 = NULL;
    };
};

extern CLUT  clut;
//----------------------------------------------------------------------
/** \brief This class contains methods that write 8-bit color rgb images
 *         or float, double, 8-bit, or 16-bit grey images.
 */
class TIFFWriter {
  public:
    /** \brief Write a 24-bit color tiff image.
     *  \param buff image pixel buffer
     *  \param width image width
     *  \param height image height
     *  \param fp output file pointer
     *  \param use_clut indicates that the clut should be used (and samples_per_pixel is one);
     *         otherwise, the clut is not not used and samples_per_pixel is 3
     *  \param samples_per_pixel 1 when using clut,
     *                           or 3 when specifying individual rgb values
     */
    static void write_tiff_data8_rgb ( const uint8* const buff,
        const int width, const int height, FILE* fp,
        const bool use_clut, const int samples_per_pixel );

    /** \brief Write a grey tiff image using float data as input.
     *  Floats will be linearly scaled to 8-bit int data.
     *  \param buff image pixel buffer
     *  \param width image width
     *  \param height image height
     *  \param fname output TIFF file name
     */
    static void write_tiff_float_grey ( const float* const buff,
        const int width, const int height, const char* const fname );

    /** \brief Write a grey tiff image using double data as input.
     *  Doubles will be linearly scaled to 8-bit int data.
     *  \param buff image pixel buffer
     *  \param width image width
     *  \param height image height
     *  \param fname output TIFF file name
     */
    static void write_tiff_double_grey ( const double* const buff,
        const int width, const int height, const char* const fname );

    /** \brief Write a grey tiff image using 8-bit data as input.
     *  \param buff image pixel buffer
     *  \param width image width
     *  \param height image height
     *  \param fp output file pointer
     */
    static void write_tiff_data8_grey ( const uint8* const buff,
        const int width, const int height, FILE* fp );

    /** \brief Write a grey tiff image using 16-bit data as input.
     *  Note: The output file will contain 16-bit data.
     *  \param buff image pixel buffer
     *  \param width image width
     *  \param height image height
     *  \param fp output file pointer
     */
    static void write_tiff_data16 ( const uint16* const buff,
        const int width, const int height, FILE* fp );
};

#endif
//----------------------------------------------------------------------
