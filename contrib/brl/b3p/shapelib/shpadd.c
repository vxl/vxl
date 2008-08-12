/******************************************************************************
 * Id
 *
 * Project:  Shapelib
 * Purpose:  Sample application for adding a shape to a shapefile.
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 ******************************************************************************
 * Copyright (c) 1999, Frank Warmerdam
 *
 * This software is available under the following "MIT Style" license,
 * or at the option of the licensee under the LGPL (see LICENSE.LGPL).  This
 * option is discussed in more detail in shapelib.html.
 *
 * --
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 *
 * Log: shpadd.c
 * Revision 1.13  2002/01/15 14:36:07  warmerda
 * updated email address
 *
 * Revision 1.12  2001/05/31 19:35:29  warmerda
 * added support for writing null shapes
 *
 * Revision 1.11  2000/07/07 13:39:45  warmerda
 * removed unused variables, and added system include files
 *
 * Revision 1.10  2000/05/24 15:09:22  warmerda
 * Added logic to graw vertex lists of needed.
 *
 * Revision 1.9  1999/11/05 14:12:04  warmerda
 * updated license terms
 *
 * Revision 1.8  1998/12/03 16:36:26  warmerda
 * Use r+b rather than rb+ for binary access.
 *
 * Revision 1.7  1998/11/09 20:57:04  warmerda
 * Fixed SHPGetInfo() call.
 *
 * Revision 1.6  1998/11/09 20:19:16  warmerda
 * Changed to use SHPObject based API.
 *
 * Revision 1.5  1997/03/06 14:05:02  warmerda
 * fixed typo.
 *
 * Revision 1.4  1997/03/06 14:01:16  warmerda
 * added memory allocation checking, and free()s.
 *
 * Revision 1.3  1995/10/21 03:14:37  warmerda
 * Changed to use binary file access
 *
 * Revision 1.2  1995/08/04  03:18:01  warmerda
 * Added header.
 *
 */

static char rcsid[] = 
  "Id";

#include <stdlib.h>
#include <string.h>
#include "shapefil.h"

int main( int argc, char ** argv )

{
    SHPHandle	hSHP;
    int		nShapeType, nVertices, nParts, *panParts, i, nVMax;
    double	*padfX, *padfY;
    SHPObject	*psObject;

/* -------------------------------------------------------------------- */
/*      Display a usage message.                                        */
/* -------------------------------------------------------------------- */
    if( argc < 2 )
    {
	printf( "shpadd shp_file [[x y] [+]]*\n" );
	exit( 1 );
    }

/* -------------------------------------------------------------------- */
/*      Open the passed shapefile.                                      */
/* -------------------------------------------------------------------- */
    hSHP = SHPOpen( argv[1], "r+b" );

    if( hSHP == NULL )
    {
	printf( "Unable to open:%s\n", argv[1] );
	exit( 1 );
    }

    SHPGetInfo( hSHP, NULL, &nShapeType, NULL, NULL );

    if( argc == 2 )
        nShapeType = SHPT_NULL;

/* -------------------------------------------------------------------- */
/*	Build a vertex/part list from the command line arguments.	*/
/* -------------------------------------------------------------------- */
    nVMax = 1000;
    padfX = (double *) malloc(sizeof(double) * nVMax);
    padfY = (double *) malloc(sizeof(double) * nVMax);
    
    nVertices = 0;

    if( (panParts = (int *) malloc(sizeof(int) * 1000 )) == NULL )
    {
        printf( "Out of memory\n" );
        exit( 1 );
    }
    
    nParts = 1;
    panParts[0] = 0;

    for( i = 2; i < argc;  )
    {
	if( argv[i][0] == '+' )
	{
	    panParts[nParts++] = nVertices;
	    i++;
	}
	else if( i < argc-1 )
	{
            if( nVertices == nVMax )
            {
                nVMax = nVMax * 2;
                padfX = (double *) realloc(padfX,sizeof(double)*nVMax);
                padfY = (double *) realloc(padfY,sizeof(double)*nVMax);
            }

	    sscanf( argv[i], "%lg", padfX+nVertices );
	    sscanf( argv[i+1], "%lg", padfY+nVertices );
	    nVertices += 1;
	    i += 2;
	}
    }

/* -------------------------------------------------------------------- */
/*      Write the new entity to the shape file.                         */
/* -------------------------------------------------------------------- */
    psObject = SHPCreateObject( nShapeType, -1, nParts, panParts, NULL,
                                nVertices, padfX, padfY, NULL, NULL );
    SHPWriteObject( hSHP, -1, psObject );
    SHPDestroyObject( psObject );
    
    SHPClose( hSHP );

    free( panParts );
    free( padfX );
    free( padfY );

    return 0;
}
