/*
 * listgeo.c -- example client code for LIBGEO geographic
 *     TIFF tag support. Dumps info to GeoTIFF metadata file.
 *
 *  Author: Niles D. Ritter
 *
 */

#include "geotiff.h"
#include "xtiffio.h"
#include "geo_normalize.h"
#include "geovalues.h"
#include "tiffio.h"
#include "cpl_serv.h"
#include <stdio.h>

static void WriteTFWFile( GTIF * gtif, const char * tif_filename );
static void GTIFPrintCorners( GTIF *, GTIFDefn *, FILE *, int, int, int, int );
static const char *CSVFileOverride( const char * );
static const char *CSVDirName = NULL;

void Usage()

{
    printf(
        "%s",
        "Usage: listgeo [-d] [-tfw] [-proj4] [-no_norm] [-t tabledir] filename\n"
        "\n"
        "  -d: report lat/long corners in decimal degrees instead of DMS.\n"
        "  -tfw: Generate a .tfw (ESRI TIFF World) file for the target file.\n"
        "  -proj4: Report PROJ.4 equivelent projection definition.\n"
        "  -no_norm: Don't report 'normalized' parameter values.\n"
        "  filename: Name of the GeoTIFF file to report on.\n" );

    exit( 1 );
}

int main(int argc, char *argv[])
{
    char        *fname = NULL;
    TIFF         *tif=(TIFF*)0;  /* TIFF-level descriptor */
    GTIF        *gtif=(GTIF*)0; /* GeoKey-level descriptor */
    int                i, norm_print_flag = 1, proj4_print_flag = 0;
    int                tfw_flag = 0, inv_flag = 0, dec_flag = 0;

    /*
     * Handle command line options.
     */
    for( i = 1; i < argc; i++ )
    {
        if( strcmp(argv[i],"-no_norm") == 0 )
            norm_print_flag = 0;
        else if( strcmp(argv[i],"-t") == 0 )
        {
            CSVDirName = argv[++i];
            SetCSVFilenameHook( CSVFileOverride );
        }
        else if( strcmp(argv[i],"-tfw") == 0 )
            tfw_flag = 1;
        else if( strcmp(argv[i],"-proj4") == 0 )
            proj4_print_flag = 1;
        else if( strcmp(argv[i],"-i") == 0 )
            inv_flag = 1;
        else if( strcmp(argv[i],"-d") == 0 )
            dec_flag = 1;
        else if( fname == NULL && argv[i][0] != '-' )
            fname = argv[i];
        else
        {
            Usage();
        }
    }

    if( fname == NULL )
        Usage();

    /*
     * Open the file, read the GeoTIFF information, and print to stdout.
     */

    tif=XTIFFOpen(fname,"r");
    if (!tif) goto failure;

    gtif = GTIFNew(tif);
    if (!gtif)
    {
        fprintf(stderr,"failed in GTIFNew\n");
        goto failure;
    }

    if( tfw_flag )
    {
        WriteTFWFile( gtif, fname );

        goto Success;
    }

    /* dump the GeoTIFF metadata to std out */

    GTIFPrint(gtif,0,0);

    /*
     * Capture, and report normalized information if requested.
     */

    if( norm_print_flag )
    {
        GTIFDefn        defn;

        if( GTIFGetDefn( gtif, &defn ) )
        {
            int                xsize, ysize;

            printf( "\n" );
            GTIFPrintDefn( &defn, stdout );

            if( proj4_print_flag )
            {
                printf( "\n" );
                printf( "PROJ.4 Definition: %s\n", GTIFGetProj4Defn(&defn));
            }

            TIFFGetField( tif, TIFFTAG_IMAGEWIDTH, &xsize );
            TIFFGetField( tif, TIFFTAG_IMAGELENGTH, &ysize );
            GTIFPrintCorners( gtif, &defn, stdout, xsize, ysize, inv_flag, dec_flag );
        }

    }

  Success:
    GTIFFree(gtif);
    XTIFFClose(tif);
    return 0;

  failure:
    fprintf(stderr,"failure in listgeo\n");
    if (tif) XTIFFClose(tif);
    if (gtif) GTIFFree(gtif);
    return 1;
}

static const char *CSVFileOverride( const char * pszInput )

{
    static char                szPath[1024];

#ifdef WIN32
    sprintf( szPath, "%s\\%s", CSVDirName, pszInput );
#else
    sprintf( szPath, "%s/%s", CSVDirName, pszInput );
#endif

    return( szPath );
}

const char *GTIFDecToDDec( double dfAngle, const char * pszAxis,
                          int nPrecision )

{
    char        szFormat[30];
    static char szBuffer[50];
    const char  *pszHemisphere = NULL;

    if( EQUAL(pszAxis,"Long") && dfAngle < 0.0 )
        pszHemisphere = "W";
    else if( EQUAL(pszAxis,"Long") )
        pszHemisphere = "E";
    else if( dfAngle < 0.0 )
        pszHemisphere = "S";
    else
        pszHemisphere = "N";

    sprintf( szFormat, "%%%d.%df%s",
             nPrecision+5, nPrecision, pszHemisphere );
    sprintf( szBuffer, szFormat, dfAngle );

    return( szBuffer );
}

/*
 * Report the file(s) corner coordinates in projected coordinates, and
 * if possible lat/long.
 */

static int GTIFReportACorner( GTIF *gtif, GTIFDefn *defn, FILE * fp_out,
                              const char * corner_name,
                              double x, double y, int inv_flag, int dec_flag )

{
    double        x_saved, y_saved;

    /* Try to transform the coordinate into PCS space */
    if( !GTIFImageToPCS( gtif, &x, &y ) )
        return FALSE;

    x_saved = x;
    y_saved = y;

    fprintf( fp_out, "%-13s ", corner_name );

    if( defn->Model == ModelTypeGeographic )
    {
        if (dec_flag)
        {
            fprintf( fp_out, "(%s,", GTIFDecToDDec( x, "Long", 7 ) );
            fprintf( fp_out, "%s)\n", GTIFDecToDDec( y, "Lat", 7 ) );
        }
        else
        {
            fprintf( fp_out, "(%s,", GTIFDecToDMS( x, "Long", 2 ) );
            fprintf( fp_out, "%s)\n", GTIFDecToDMS( y, "Lat", 2 ) );
        }
    }
    else
    {
        fprintf( fp_out, "(%12.3f,%12.3f)", x, y );

        if( GTIFProj4ToLatLong( defn, 1, &x, &y ) )
        {
            if (dec_flag)
            {
                fprintf( fp_out, "  (%s,", GTIFDecToDDec( x, "Long", 7 ) );
                fprintf( fp_out, "%s)", GTIFDecToDDec( y, "Lat", 7 ) );
            }
            else
            {
                fprintf( fp_out, "  (%s,", GTIFDecToDMS( x, "Long", 2 ) );
                fprintf( fp_out, "%s)", GTIFDecToDMS( y, "Lat", 2 ) );
            }
        }

        fprintf( fp_out, "\n" );
    }

    if( inv_flag && GTIFPCSToImage( gtif, &x_saved, &y_saved ) )
    {
        fprintf( fp_out, "      inverse (%11.3f,%11.3f)\n", x_saved, y_saved );
    }

    return TRUE;
}

static void GTIFPrintCorners( GTIF *gtif, GTIFDefn *defn, FILE * fp_out,
                              int xsize, int ysize, int inv_flag, int dec_flag )

{
    printf( "\nCorner Coordinates:\n" );
    if( !GTIFReportACorner( gtif, defn, fp_out,
                            "Upper Left", 0.0, 0.0, inv_flag, dec_flag ) )
    {
        printf( " ... unable to transform points between pixel/line and PCS space\n" );
        return;
    }

    GTIFReportACorner( gtif, defn, fp_out, "Lower Left", 0.0, ysize,
                       inv_flag, dec_flag );
    GTIFReportACorner( gtif, defn, fp_out, "Upper Right", xsize, 0.0,
                       inv_flag, dec_flag );
    GTIFReportACorner( gtif, defn, fp_out, "Lower Right", xsize, ysize,
                       inv_flag, dec_flag );
    GTIFReportACorner( gtif, defn, fp_out, "Center", xsize/2.0, ysize/2.0,
                       inv_flag, dec_flag );
}

/*
 * Write the defining matrix for this file to a .tfw file with the same
 * basename.
 */

static void WriteTFWFile( GTIF * gtif, const char * tif_filename )

{
    char        tfw_filename[1024];
    int                i;
    double        adfCoeff[6], x, y;
    FILE        *fp;

    /*
     * form .tfw filename
     */
    strncpy( tfw_filename, tif_filename, sizeof(tfw_filename)-4 );
    for( i = strlen(tfw_filename)-1; i > 0; i-- )
    {
        if( tfw_filename[i] == '.' )
        {
            strcpy( tfw_filename + i, ".tfw" );
            break;
        }
    }

    if( i <= 0 )
        strcat( tfw_filename, ".tfw" );

    /*
     * Compute the coefficients.
     */
    x = 0.5;
    y = 0.5;
    if( !GTIFImageToPCS( gtif, &x, &y ) )
        return;
    adfCoeff[4] = x;
    adfCoeff[5] = y;

    x = 1.5;
    y = 0.5;
    if( !GTIFImageToPCS( gtif, &x, &y ) )
        return;
    adfCoeff[0] = x - adfCoeff[4];
    adfCoeff[1] = y - adfCoeff[5];

    x = 0.5;
    y = 1.5;
    if( !GTIFImageToPCS( gtif, &x, &y ) )
        return;
    adfCoeff[2] = x - adfCoeff[4];
    adfCoeff[3] = y - adfCoeff[5];

    /*
     * Write out the coefficients.
     */

    fp = fopen( tfw_filename, "wt" );
    if( fp == NULL )
    {
        perror( "fopen" );
        fprintf( stderr, "Failed to open TFW file `%s'\n", tfw_filename );
        return;
    }

    for( i = 0; i < 6; i++ )
        fprintf( fp, "%24.10f\n", adfCoeff[i] );

    fclose( fp );
}
