/********************************************************** 
** Copyright 1998 Earth Resource Mapping Ltd.
** This document contains proprietary source code of
** Earth Resource Mapping Ltd, and can only be used under
** one of the three licenses as described in the 
** license.txt file supplied with this distribution. 
** See separate license.txt file for license details 
** and conditions.
**
** This software is covered by US patent #6,442,298,
** #6,102,897 and #6,633,688.  Rights to use these patents 
** is included in the license agreements.
** 
** FILE:   	rangecoder.c
** CREATED:	1998
** AUTHOR: 	SNS
** PURPOSE:	Range encoder/qsmodel, from Michael Schindler, used with permission
**
**
**	SPECIAL NOTES!!
**	(1)	Do NOT compile this - it is included inline into pack.c to gain performance
**		by doing in-line optimizations
**
**  (2) 17th September 1998 Stuart Nixon
**		Used with permission from Michael Schindler
**		Permission specifically includes (1) ability to use in commercial software
**		(2) no need to provide source with commercial software.
**
**
**	[01] modified to handle 16 bit symbols
**
**********************************************************/


/*
  rangecod.c     range encoding

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

  There is a supplementary file called renorm95.c available at the
  website (www.compressconsult.com/rangecoder/) that changes the range
  coder to an arithmetic coder for speed comparisons.
*/

/* define nowarn if you do not expect more than 2^32 outstanding bytes */
/* since I recommend restarting the coder in intervals of less than    */
/* 2^23 symbols for error tolerance this is not expected               */
// #define NOWARN

/* SIZE OF RANGE ENCODING CODE VALUES. */

#define CODE_BITS 32
#define Top_value ((code_value)1 << (CODE_BITS-1))


/* all IO is done by these macros - change them if you want to */
/* no checking is done - do it here if you want it             */
/* cod is a pointer to the used rangecoder                     */
#ifdef NEVER_OLD_FILEIO
//	#define outbyte(cod,x) fputc(x,*(cod)->iofile)
//	#define inbyte(cod)    fgetc((cod)->iofile)
#endif
#define outbyte(cod,x) *(cod)->p_packed++ = (UINT8)(x);
#define inbyte(cod)    *(cod)->p_packed++;


#define SHIFT_BITS (CODE_BITS - 9)
#define EXTRA_BITS ((CODE_BITS-2) % 8 + 1)
#define Bottom_value (Top_value >> 8)

static char coderversion[]="rangecode 1.1 (c) 1997, 1998 Michael Schindler";/* Start the encoder                                           */


/* rc is the range coder to be used                            */
/* c is written as first byte in the datastream                */
/* one could do without c, but then you have an additional if  */
/* per outputbyte.                                             */
static void start_encoding( rangecoder *rc, char c )
{   rc->low = 0;                /* Full code range */
    rc->range = Top_value;
    rc->buffer = c;
    rc->help = 0;               /* No bytes to follow */
    rc->bytecount = 0;
}


/* I do the normalization before I need a defined state instead of */
/* after messing it up. This simplifies starting and ending.       */
static __inline void enc_normalize( rangecoder *rc )
{   while(rc->range <= Bottom_value)     /* do we need renormalisation?  */
    {   if (rc->low < 0xff<<SHIFT_BITS)  /* no carry possible --> output */
        {   outbyte(rc,rc->buffer);
            for(; rc->help; rc->help--)
                outbyte(rc,0xff);
            rc->buffer = (unsigned char)(rc->low >> SHIFT_BITS);
        } else if (rc->low & Top_value) /* carry now, no future carry */
        {   outbyte(rc,rc->buffer+1);
            for(; rc->help; rc->help--)
                outbyte(rc,0);
            rc->buffer = (unsigned char)(rc->low >> SHIFT_BITS);
        } else                           /* passes on a potential carry */
            rc->help++;
        rc->range <<= 8;
        rc->low = (rc->low<<8) & (Top_value-1);
        rc->bytecount++;
    }
}


/* Encode a symbol using frequencies                         */
/* rc is the range coder to be used                          */
/* sy_f is the interval length (frequency of the symbol)     */
/* lt_f is the lower end (frequency sum of < symbols)        */
/* tot_f is the total interval length (total frequency sum)  */
/* or (faster): tot_f = 1<<shift                             */
static __inline void encode_freq( rangecoder *rc, freq sy_f, freq lt_f, freq tot_f )
{	code_value r, tmp;
	enc_normalize( rc );
	r = rc->range / tot_f;
	tmp = r * lt_f;
	if (lt_f+sy_f < tot_f)
		rc->range = r * sy_f;
	else
		rc->range -= tmp;
	rc->low += tmp;
}

static __inline void encode_shift( rangecoder *rc, freq sy_f, freq lt_f, freq shift )
{	code_value r, tmp;
	enc_normalize( rc );
	r = rc->range >> shift;
	tmp = r * lt_f;
	if ((lt_f+sy_f) >> shift)
		rc->range -= tmp;
	else  
		rc->range = r * sy_f;
	rc->low += tmp;
}


/* Finish encoding                                           */
/* rc is the range coder to be used                          */
/* actually not that many bytes need to be output, but who   */
/* cares. I output them because decode will read them :)     */
static void done_encoding( rangecoder *rc )
{   uint tmp;
    enc_normalize(rc);     /* now we have a normalized state */
    rc->bytecount += 5;
    if ((rc->low & (Bottom_value-1)) < (rc->bytecount>>1))
       tmp = rc->low >> SHIFT_BITS;
    else
       tmp = (rc->low >> SHIFT_BITS) + 1;
    if (tmp > 0xff) /* we have a carry */
    {   outbyte(rc, rc->buffer+1);
        for(; rc->help; rc->help--)
            outbyte(rc,0);
    } else  /* no carry */
    {   outbyte(rc, rc->buffer);
        for(; rc->help; rc->help--)
            outbyte(rc,0xff);
    }
    outbyte(rc, tmp & 0xff);
    outbyte(rc, (rc->bytecount>>16) & 0xff);
    outbyte(rc, (rc->bytecount>>8) & 0xff);
    outbyte(rc, rc->bytecount & 0xff);
}


/* Start the decoder                                         */
/* rc is the range coder to be used                          */
/* returns the char from start_encoding or EOF               */
static int start_decoding( rangecoder *rc )
{   int c = inbyte(rc);
    if (c==EOF)
        return EOF;
    rc->buffer = inbyte(rc);
    rc->low = rc->buffer >> (8-EXTRA_BITS);
    rc->range = 1 << EXTRA_BITS;
    return c;
}

/* [04] converted to a macro for speed */
#define DEC_NORMALIZE( rc )												\
	while (rc->range <= Bottom_value) {									\
		rc->low = (rc->low<<8) | ((rc->buffer<<EXTRA_BITS)&0xff);		\
        rc->buffer = inbyte(rc);										\
        rc->low |= rc->buffer >> (8-EXTRA_BITS);						\
        rc->range <<= 8;												\
    }



/* Calculate culmulative frequency for next symbol. Does NO update!*/
/* rc is the range coder to be used                          */
/* tot_f is the total frequency                              */
/* or: totf is 1<<shift                                      */
/* returns the culmulative frequency                         */
static __inline freq decode_culfreq( rangecoder *rc, freq tot_f )
{   freq tmp;
    DEC_NORMALIZE(rc);
    rc->help = rc->range/tot_f;
    tmp = rc->low/rc->help;
    return (tmp>=tot_f ? tot_f-1 : tmp);
}

static __inline freq decode_culshift( rangecoder *rc, freq shift )
{   freq tmp;
    DEC_NORMALIZE(rc);
    rc->help = rc->range>>shift;
    tmp = rc->low/rc->help;
    return (tmp>>shift ? (1<<shift)-1 : tmp);
}


/* Update decoding state                                     */
/* rc is the range coder to be used                          */
/* sy_f is the interval length (frequency of the symbol)     */
/* lt_f is the lower end (frequency sum of < symbols)        */
/* tot_f is the total interval length (total frequency sum)  */
static __inline void decode_update( rangecoder *rc, freq sy_f, freq lt_f, freq tot_f)
{   code_value tmp;
    tmp = rc->help * lt_f;
    rc->low -= tmp;
    if (lt_f + sy_f < tot_f)
        rc->range = rc->help * sy_f;
    else
        rc->range -= tmp;
}


/* Decode a byte/short without modelling                     */
/* rc is the range coder to be used                          */
static unsigned char decode_byte(rangecoder *rc)
{   unsigned char tmp = (unsigned char)decode_culshift(rc,8);
    decode_update( rc,1,tmp,1<<8);
    return tmp;
}

static unsigned short decode_short(rangecoder *rc)
{   unsigned short tmp = (unsigned short)decode_culshift(rc,16);
    decode_update( rc,1,tmp,(freq)1<<16);
    return tmp;
}


/* Finish decoding                                           */
/* rc is the range coder to be used                          */
static void done_decoding( rangecoder *rc )
{
	rc;// Keep compiler happy
// [03] The dec_normalize is longer needed as we don't stream input from a file
//	dec_normalize(rc);      /* normalize to use up all bytes */
}




/*
  qsmodel.c     headerfile for quasistatic probability model

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

  Qsmodel is a quasistatic probability model that periodically
  (at chooseable intervals) updates probabilities of symbols;
  it also allows to initialize probabilities. Updating is done more
  frequent in the beginning, so it adapts very fast even without
  initialisation.

  it provides function for creation, deletion, query for probabilities
  and symbols and model updating.

  for usage see example.c
*/

/* default tablesize 1<<TBLSHIFT */
#define TBLSHIFT 7

/* rescale frequency counts */
static __inline void dorescale( qsmodel *m)
{   int i, cf, missing;
    if (m->nextleft)  /* we have some more before actual rescaling */
    {   m->incr++;
        m->left = m->nextleft;
        m->nextleft = 0;
        return;
    }
    if (m->rescale < m->targetrescale)  /* double rescale interval if needed */
    {   m->rescale <<= 1;
        if (m->rescale > m->targetrescale)
            m->rescale = m->targetrescale;
    }
    cf = missing = m->cf[m->n];  /* do actual rescaling */
    for(i=m->n-1; i; i--)
    {   int tmp = m->newf[i];
        cf -= tmp;
        m->cf[i] = (qssymbol)cf;
        tmp = tmp>>1 | 1;
        missing -= tmp;
        m->newf[i] = (qssymbol)tmp;
    }
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
    if (cf!=m->newf[0])
    {   fprintf(stderr,"BUG: rescaling left %d total frequency\n",cf);
        deleteqsmodel(m);
        exit(1);
    }
#endif // NCS_BUILD_WITH_STDERR_DEBUG_INFO
    m->newf[0] = m->newf[0]>>1 | 1;
    missing -= m->newf[0];
    m->incr = missing / m->rescale;
    m->nextleft = missing % m->rescale;
    m->left = m->rescale - m->nextleft;
    if (m->search != NULL)
    {   i=m->n;
        while (i)
        {   int start, end;
            end = (m->cf[i]-1) >> m->searchshift;
            i--;
            start = m->cf[i] >> m->searchshift;
            while (start<=end)
            {   m->search[start] = (qssymbol)i;
                start++;
            }
        }
    }
}


/* initialisation of qsmodel                           */
/* m   qsmodel to be initialized                       */
/* n   number of symbols in that model                 */
/* lg_totf  base2 log of total frequency count         */
/* rescale  desired rescaling interval, should be < 1<<(lg_totf+1) */
/* init  array of int's to be used for initialisation (NULL ok) */
/* compress  set to 1 on compression, 0 on decompression */
static void initqsmodel( qsmodel *m, int n, int lg_totf, int rescale, int *init, int compress )
{   m->n = n;
    m->targetrescale = rescale;
    m->searchshift = lg_totf - TBLSHIFT;
    if (m->searchshift < 0)
        m->searchshift = 0;
    m->cf = NCSMalloc((n+1)*sizeof(qssymbol), FALSE);		/* [01] was uint2 */
    m->newf = NCSMalloc((n+1)*sizeof(qssymbol), FALSE);	/* [01] was uint2 */
    m->cf[n] = (qssymbol)(1<<lg_totf);
    m->cf[0] = 0;
    if (compress)
        m->search = NULL;
    else
    {   m->search = NCSMalloc(((1<<TBLSHIFT)+1)*sizeof(qssymbol), FALSE);	/* [01] was uint2 */
        m->search[1<<TBLSHIFT] = (qssymbol)(n-1);
    }
    resetqsmodel(m, init);
}


/* reinitialisation of qsmodel                         */
/* m   qsmodel to be initialized                       */
/* init  array of int's to be used for initialisation (NULL ok) */
static void resetqsmodel( qsmodel *m, int *init)
{   int i, end, initval;
    m->rescale = m->n>>4 | 2;
    m->nextleft = 0;
    if (init == NULL)
    {   initval = m->cf[m->n] / m->n;
        end = m->cf[m->n] % m->n;
        for (i=0; i<end; i++)
            m->newf[i] = (qssymbol)(initval+1);
        for (; i<m->n; i++)
            m->newf[i] = (qssymbol)initval;
    } else
        for(i=0; i<m->n; i++)
            m->newf[i] = (qssymbol)init[i];
    dorescale(m);
}


/* deletion of qsmodel m                               */
static void deleteqsmodel( qsmodel *m )
{   NCSFree(m->cf);
    NCSFree(m->newf);
    if (m->search != NULL)
        NCSFree(m->search);
}


/* retrieval of estimated frequencies for a symbol     */
/* m   qsmodel to be questioned                        */
/* sym  symbol for which data is desired; must be <n   */
/* sy_f frequency of that symbol                       */
/* lt_f frequency of all smaller symbols together      */
/* the total frequency is 1<<lg_totf                   */
static __inline int qsgetfreq( qsmodel *m, int sym, int *lt_f )
{   return(m->cf[sym+1] - (*lt_f = m->cf[sym]));
}	


/* find out symbol for a given cumulative frequency    */
/* m   qsmodel to be questioned                        */
/* lt_f  cumulative frequency                          */
static __inline int qsgetsym( qsmodel *m, int lt_f )
{   int lo, hi;
    qssymbol *tmp;	/* [01] was uint2 */
    tmp = m->search+(lt_f>>m->searchshift);
    lo = *tmp;
    hi = *(tmp+1) + 1;
    while (lo+1 < hi )
    {   int mid = (lo+hi)>>1;
        if (lt_f < m->cf[mid])
            hi = mid;
        else
            lo = mid;
    }
    return lo;
}


/* update model                                        */
/* m   qsmodel to be updated                           */
/* sym  symbol that occurred (must be <n from init)    */
static __inline void qsupdate( qsmodel *m, int sym )
{   if (m->left <= 0)
        dorescale(m);
    m->left--;
    m->newf[sym] = m->newf[sym] + ((qssymbol)m->incr);
}
