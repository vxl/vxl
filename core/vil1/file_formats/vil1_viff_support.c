/*
 *----------------------------------------------------------------------
 *
 * Copyright 1990, University of New Mexico.  All rights reserved.

 * Permission to copy and modify this software and its documen-
 * tation only for internal use in your organization is hereby
 * granted, provided that this notice is retained thereon and
 * on all copies.  UNM makes no representations as too the sui-
 * tability and operability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * UNM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FIT-
 * NESS.  IN NO EVENT SHALL UNM BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY OTHER DAMAGES WHAT-
 * SOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PER-
 * FORMANCE OF THIS SOFTWARE.
 *
 * No other rights, including for example, the right to redis-
 * tribute this software and its documentation or the right to
 * prepare derivative works, are granted unless specifically
 * provided in a separate license agreement.
 *---------------------------------------------------------------------
 */

/*
#
# FILE NAME:    cr_image.c
#
# AUTHORS:      Ron E. Neher
#
# DATE:         3/23/89
#
# DESCRIPTION:  Create a generic image
#
# MODIFICATIONS:  Added "map_scheme" to the parameter list.  The image's
#                 map_scheme was hardwired to VFF_MS_NONE which meant that
#                 no map space was being allocated.
#                       (Mark Young & John Rasure 7/23/89)
#
*/

#include "vil_viffheader.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LENGTH 512

int vil_viff_imagesize(struct vil_viff_xvimage *image,int *dsize, int *dcount, int *msize,
                  int *mcount, int *lsize,int *lcount);

/**************************************************************
*
* MODULE NAME: createimage
*
*     PURPOSE: Create a generic image
*
*       INPUT:  col_size -- the size of a column
*               row_size -- the size of a row
*               data_storage_type -- the VFF_TYP_* define of image
*               num_of_images -- # of images pointed to by imagedata
*               num_data_bands -- # of bands/pixel, /image or dim vec data
*               comment -- description of image
*               map_row_size -- # of columns in map array
*               map_col_size -- # of rows in map array
*               map_storage_type -- Storage type of cells in the maps
*               location_type -- implied or explicit location data
*               location_dim -- explicit locations can be of any dimension
*
*      OUTPUT:  1.  returns a pointer to a vil_viff_xvimage with image defined
*
* CALLED FROM:
*
* ROUTINES CALLED:
*
**************************************************************/

struct vil_viff_xvimage *
vil_viff_createimage(col_size, row_size, data_storage_type, num_of_images,
                num_data_bands, comment, map_row_size, map_col_size,
                map_scheme, map_storage_type, location_type, location_dim)
unsigned
long    col_size,
        row_size,
        data_storage_type,
        num_of_images,
        num_data_bands,
        map_row_size,
        map_col_size,
        map_scheme,
        map_storage_type,
        location_type,
        location_dim;
char    *comment;
{
struct
vil_viff_xvimage *image;
char    *maps,
        *imagedata,
        tmp_comment[LENGTH];
float   *location;
size_t  cstrlen;
int     image_data_size_bytes,          /* # data bytes */
        image_data_count_pixels,        /* # data pixels */
        map_size_bytes,                 /* # map bytes */
        map_count_cells,                /* # map cells */
        location_size_bytes,            /* # location bytes */
        location_count_objects;         /* # location objs */

/* malloc room for the vil_viff_xvimage structure */

    if ((image=(struct vil_viff_xvimage *)malloc(sizeof(struct vil_viff_xvimage)))== NULL)
    {
         fprintf(stderr,"vil_viff_createimage: No space for image \
- malloc failed!\n");
        return 0;
    }

/* setup the comment (can only be 511 chars) */

    cstrlen = comment?strlen(comment):0;
    if (cstrlen > 0)
    {
       if (cstrlen < 512)
          strcpy(tmp_comment, comment?comment:"");
       else
       {
          strncpy(tmp_comment, comment?comment:"", LENGTH - 1);
          strcat(tmp_comment, "");
       }
    }
    else
       strcpy(tmp_comment, "");

/* Load the image header with the values. These can be over-ridden by
   giving them a different value after returning to the calling routine.
 */
    image->identifier = (char)XV_FILE_MAGIC_NUM;
    image->file_type = XV_FILE_TYPE_XVIFF;
    image->release = XV_IMAGE_REL_NUM;
    image->version = XV_IMAGE_VER_NUM;
    image->machine_dep = VFF_DEP_IEEEORDER; /* assume IEEE byte order */
    strcpy(image->comment, tmp_comment);
    image->row_size = row_size;
    image->col_size = col_size;
    image->startx = VFF_NOTSUB;
    image->starty = VFF_NOTSUB;
    image->pixsizx = 1.0;
    image->pixsizy = 1.0;
    image->location_type = location_type;
    image->location_dim = location_dim;
    image->num_of_images = num_of_images;
    image->num_data_bands = num_data_bands;
    image->data_storage_type = data_storage_type;
    image->data_encode_scheme = VFF_DES_RAW;
    image->map_scheme = map_scheme;
    image->map_storage_type = map_storage_type;
    image->map_row_size = map_row_size;
    image->map_col_size = map_col_size;
    image->map_subrow_size = 0;
    image->map_enable = VFF_MAP_OPTIONAL;
    image->maps_per_cycle = 0;      /* Don't care */
    image->color_space_model = VFF_CM_NONE;
    image->ispare1 = 0;
    image->ispare2 = 0;
    image->fspare1 = 0;
    image->fspare2 = 0;

/* get the sizes for the image data, map data, and location data */

    if (! vil_viff_imagesize(image,                     /* vil_viff_xvimage */
                             &image_data_size_bytes,    /* # data bytes */
                             &image_data_count_pixels,  /* # data pixels */
                             &map_size_bytes,           /* # map bytes */
                             &map_count_cells,          /* # map cells */
                             &location_size_bytes,      /* # location bytes */
                             &location_count_objects    /* # location objs */
                            ))
    {
        fprintf(stderr, "vil_viff_createimage: Uninterpretable image \
specificationa\n");
        return 0;
    }

/* malloc room for the image data */

    if (image_data_size_bytes > 0)
    {
       if ((imagedata = (char *)
          malloc((size_t)image_data_size_bytes)) == NULL)
       {
           fprintf(stderr,"vil_viff_createimage: Not enough memory for image\
 data!\n");
          return 0;
       }
    }
    else
    {
       imagedata = NULL;
    }

/* malloc room for the color map data */

    if (map_size_bytes > 0)
    {
       if ((maps = (char *)malloc((size_t)map_size_bytes)) == NULL)
       {
            fprintf(stderr,"vil_viff_createimage: Not enough memory for maps\
 data!\n");
           return 0;
       }
    }
    else
    {
       maps = NULL;
    }


/* malloc room for the location */

    if (location_size_bytes)
    {
       if ((location = (float *)
           malloc((size_t)location_size_bytes))==NULL)
       {
            fprintf(stderr,"vil_viff_createimage: Not enough memory \
 for location data!\n");
           return 0;
       }
    }
    else
    {
       location = NULL;
    }


/* Load the image data, color map data, and location data */

    image->maps = maps;
    image->location = location;
    image->imagedata = imagedata;

    return image;
}

/************************************************************
*
*  MODULE NAME: freeimage
*
*      PURPOSE: This routine frees an khoros xvimage structure.
*
*        INPUT: image --  a pointer to an khoros xvimage structure that
*                         contains the image structure to be freed.
*
*       OUTPUT: (none)  since all we are doing is freeing as much of
*               a image structure as we can.
*
*    CALLED BY: any routine that wishes to free an xvimage structure
*
*   WRITTEN BY: Mark Young
*   MODIFIED BY: Scott Wilson - Updated to XV III 27-Feb-89
*                Scott Wilson - Added bzero of header 13-Nov-91
*                Scott Wilson - Added defensive ID test. 14-Jan-92
*
*************************************************************/

void vil_viff_freeimage(struct vil_viff_xvimage *image)
{
        unsigned char id1,id2;

        /*
         *  Free as much of the xvimage structure as we can.  But first check to
         *  make sure the image pointer is not NULL.
         */
        if (image != NULL)
        {
           /* Now see of the image itself is legal. This catches accidental
              attempts to free an image already turned loose by vil_viff_freeimage(). */
           id1 = image->identifier;
           id2 = XV_FILE_MAGIC_NUM;
           if (id1 != id2)
             {
               fprintf(stderr,
       "vil_viff_freeimage: Attempt to free an object that is not a VIFF image.\n");
               fprintf(stderr,
       "vil_viff_freeimage: Object may be a VIFF image that has already been free'd.\n");
               fprintf(stderr,"vil_viff_freeimage: Attempt aborted.\n");
               return;
             }

           /*  free image data */
           if (image->imagedata != NULL && (image->row_size *
               image->col_size) > 0)
              free ((char *) image->imagedata);

           /*  free map data */
           if (image->maps != NULL && image->map_row_size > 0)
              free ((char *) image->maps);

           /*  free location data */
           if (image->location != NULL && image->row_size *
               image->col_size > 0 &&
               image->location_type == VFF_LOC_EXPLICIT)
              free ((char *) image->location);

           free((char *) image);
        }
}


unsigned long vil_viff_getmachsize(unsigned long mtype,unsigned long dtype)
{
   unsigned long tmp = (mtype==VFF_DEP_CRAYORDER) + 1;
   switch(dtype){
      case VFF_TYP_BIT     : return((unsigned long)0);
      case VFF_TYP_1_BYTE  : return((unsigned long)1);
      case VFF_TYP_2_BYTE  : if(mtype==VFF_DEP_CRAYORDER)
                                return((unsigned long)8);
                             else
                                return((unsigned long)2);
      case VFF_TYP_4_BYTE  : return((unsigned long)4*tmp);
      case VFF_TYP_FLOAT   : return((unsigned long)4*tmp);
      case VFF_TYP_DOUBLE  : return((unsigned long)8);
      case VFF_TYP_COMPLEX : return((unsigned long)8*tmp);
      case VFF_TYP_DCOMPLEX: return((unsigned long)16);
      default: return((unsigned long)255);
   }
}

/*

    IMAGESIZE - Compute the size in bytes and objects of the
                components of an KHOROS 3 image.

    Written:  Scott Wilson
    Date:     29-Mar-89

    Modifications: Completely rewritten. Jeremey Worley 1991

    Usage:
       i = imagesize(&image,&dsize,&dcount,&msize,&mcount,&lsize,&lcount);
       if (i == 0) printf("Invalid image configuration\n");
       else {
         printf("Number of data "pixels" of specified data type: %d\n,dcount);
         printf("Number of map cells of specified map data type: %d\n,mcount);
         printf("Number of location objects (floats): %d\n,lcount);
         printf("Number of data bytes: %d\n,dsize);
         printf("Number of map bytes: %d\n,msize);
         printf("Number of location bytes: %d\n,lsize);
       }
*/

int vil_viff_imagesize(struct vil_viff_xvimage *image,int *dsize, int *dcount, int *msize,
                  int *mcount, int *lsize,int *lcount)
{
    long rows,cols;
    unsigned long mach;
    int datasize,datacount;
    int mapsize,mapcount;
    int locsize,loccount;

    cols = image->row_size;
    rows = image->col_size;

    mach = (long)image->machine_dep;

    /*
    ** Compute total size of DATA in bytes
    */
    if(image->data_storage_type==VFF_TYP_BIT){
       datasize = ((cols+7)/8)*rows;
       datacount = datasize;
    }else{
       datasize = cols*rows * vil_viff_getmachsize(mach,
                  (unsigned long)image->data_storage_type);
       datacount = cols*rows;
    }

    datasize *= image->num_of_images*image->num_data_bands;
    datacount *= image->num_of_images*image->num_data_bands;

    /*
    ** Compute number of MAP data objects
    */
    switch(image->map_scheme)
      {
        case VFF_MS_NONE:
          mapcount = 0;
          break;
        case VFF_MS_ONEPERBAND:
        case VFF_MS_CYCLE:
          mapcount =
              image->num_data_bands*image->map_row_size*image->map_col_size;
          break;
        case VFF_MS_SHARED:
        case VFF_MS_GROUP:
          mapcount = image->map_row_size*image->map_col_size;
          break;
        default:
          fprintf(stderr,"\nvil_viff_imagesize: Unknown mapping scheme:");
          fprintf(stderr," %lu\n",image->map_scheme);
          return 0;
          /* break; */
      }

    /*
    ** mapcount now contains the number of CELLS, so convert to bytes
    */
    if(image->map_storage_type==VFF_MAPTYP_NONE){
       mapsize = 0;
    }else{
       mapsize = mapcount*vil_viff_getmachsize(mach,
                 (unsigned long)image->map_storage_type);
    }

    /*
    ** Compute size of LOCATION data in bytes and floats
    */
    loccount = rows*cols*image->location_dim;
    locsize  = loccount*vil_viff_getmachsize(mach,(long)VFF_TYP_FLOAT);

    *dsize = datasize;
    *dcount = datacount;
    *msize = mapsize;
    *mcount = mapcount;
    *lsize = locsize;
    *lcount = loccount;
    return 1;
}

