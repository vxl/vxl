#ifndef rangecod_h
#define rangecod_h

/*
  rangecod.h     headerfile for range encoding

  (c) Michael Schindler
  1997, 1998
  http://www.compressconsult.com/ or http://eiunix.tuwien.ac.at/~michael
  michael@compressconsult.com        michael@eiunix.tuwien.ac.at

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.  It may be that this
  program violates local patents in your country, however it is
  belived (NO WARRANTY!) to be patent-free here in Austria.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston,
  MA 02111-1307, USA.

  Range encoding is based on an article by G.N.N. Martin, submitted
  March 1979 and presented on the Video & Data Recording Conference,
  Southampton, July 24-27, 1979. If anyone can name the original
  copyright holder of that article or locate G.N.N. Martin please
  contact me; this might allow me to make that article available on
  the net for general public.

  Range coding is closely related to arithmetic coding, except that
  it does renormalisation in larger units than bits and is thus
  faster. An earlier version of this code was distributed as byte
  oriented arithmetic coding, but then I had no knowledge of Martin's
  paper from seventy-nine.

  The input and output is done by the INBYTE and OUTBYTE macros
  defined in the .c file; change them as needed; the first parameter
  passed to them is a pointer to the rangecoder structure; extend that
  structure as needed (and don't forget to initialize the values in
  start_encoding resp. start_decoding). This distribution writes to
  stdout and reads from stdin.

  There are no global or static var's, so if the IO is thread save the
  whole rangecoder is.

  For error recovery the last 3 bytes written contain the total number
  of bytes written since starting the encoder. This can be used to
  locate the beginning of a block if you have only the end.
*/

//extern char coderversion[];

typedef UINT32 code_value;       /* Type of an rangecode value       */
                                /* must accomodate 32 bits          */
/* it is highly recommended that the total frequency count is less  */
/* than 1 << 19 to minimize rounding effects.                       */
/* the total frequency count MUST be less than 1<<23                */

typedef UINT32 freq; 

/* make the following private in the arithcoder object in C++	    */

typedef struct {
    UINT32 low,           /* low end of interval */
           range,         /* length of interval */
           help;          /* bytes_to_follow resp. intermediate value */
    unsigned char buffer;/* buffer for input/output */
/* the following is used only when encoding */
    UINT32 bytecount;     /* counter for outputed bytes  */
/* insert fields you need for input/output below this line! */
/*	FILE *iofile;	*/	/* file to read or write */
	UINT8	*p_packed;	/* pointer for output / input to/from memory instead of disk */
} rangecoder;


/* supply the following as methods of the arithcoder object  */
/* omit the first parameter then (C++)                       */


/* Start the encoder                                         */
/* rc is the range coder to be used                          */
/* c is written as first byte in the datastream (header,...) */
static void start_encoding( rangecoder *rc, char c );


/* Encode a symbol using frequencies                         */
/* rc is the range coder to be used                          */
/* sy_f is the interval length (frequency of the symbol)     */
/* lt_f is the lower end (frequency sum of < symbols)        */
/* tot_f is the total interval length (total frequency sum)  */
/* or (a lot faster): tot_f = 1<<shift                       */
static __inline void encode_freq( rangecoder *rc, freq sy_f, freq lt_f, freq tot_f );
static __inline void encode_shift( rangecoder *rc, freq sy_f, freq lt_f, freq shift );

/* Encode a byte/short without modelling                     */
/* rc is the range coder to be used                          */
/* b,s is the data to be encoded                             */
#define encode_byte(ac,b)  encode_shift(ac,(freq)1,(freq)(b),(freq)8)
#define encode_short(ac,s) encode_shift(ac,(freq)1,(freq)(s),(freq)16)


/* Finish encoding                                           */
/* rc is the range coder to be shut down                     */
static void done_encoding( rangecoder *rc );



/* Start the decoder                                         */
/* rc is the range coder to be used                          */
/* returns the char from start_encoding or EOF               */
static int start_decoding( rangecoder *rc );

/* Calculate culmulative frequency for next symbol. Does NO update!*/
/* rc is the range coder to be used                          */
/* tot_f is the total frequency                              */
/* or: totf is 1<<shift                                      */
/* returns the <= culmulative frequency                      */
static __inline freq decode_culfreq( rangecoder *rc, freq tot_f );
static __inline freq decode_culshift( rangecoder *ac, freq shift );

/* Update decoding state                                     */
/* rc is the range coder to be used                          */
/* sy_f is the interval length (frequency of the symbol)     */
/* lt_f is the lower end (frequency sum of < symbols)        */
/* tot_f is the total interval length (total frequency sum)  */
static __inline void decode_update( rangecoder *rc, freq sy_f, freq lt_f, freq tot_f);
#define decode_update_shift(rc,f1,f2,f3) decode_update((rc),(f1),(f2),(freq)1<<(f3));

/* Decode a byte/short without modelling                     */
/* rc is the range coder to be used                          */
static unsigned char decode_byte(rangecoder *rc);
static unsigned short decode_short(rangecoder *rc);


/* Finish decoding                                           */
/* rc is the range coder to be used                          */
static void done_decoding( rangecoder *rc );

#endif
