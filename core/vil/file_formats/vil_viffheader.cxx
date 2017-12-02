//:
// \file
#include "vil_viffheader.h"
#include "vcl_compiler.h"
#include <string.h>


//: Construct an image header
// \param ncols - vil_image::ni()
// \param nrows - vil_image::nj()
// \param storage_type - similar to vil_image::pixel_format()
// \param num_bands - similar to vil_image::n_planes()
vil_viff_xvimage::vil_viff_xvimage(
  unsigned ncols, unsigned nrows,
  vil_viff_data_storage storage_type,
  unsigned num_bands)
{
  const unsigned VIL_VIFF_COMMENT_LENGTH = 512L;

  identifier = (char)XV_FILE_MAGIC_NUM;
  file_type = XV_FILE_TYPE_XVIFF;
  release = XV_IMAGE_REL_NUM;
  version = XV_IMAGE_VER_NUM;
  machine_dep = VFF_DEP_IEEEORDER; /* assume IEEE byte order */
  memset(  reserve, 0, VIFF_HEADERSIZE-21*sizeof(vxl_sint_32)-520*sizeof(char)-4*sizeof(float));
  memset(  trash, 0, 3L);
  memset(  comment, 0, VIL_VIFF_COMMENT_LENGTH);
  strncpy( comment, "vil_viff image writer output", 28); // must be <= 511 chars
  row_size = ncols;
  col_size = nrows;
  subrow_size = 0; /* Don't care, just avoid uninitialised memory. */
  startx = VFF_NOTSUB;
  starty = VFF_NOTSUB;
  pixsizx = 1.0;
  pixsizy = 1.0;
  location_type = VFF_LOC_IMPLICIT;
  location_dim = 0;
  location = VXL_NULLPTR;
  num_of_images = 1;
  num_data_bands = num_bands;
  data_storage_type = storage_type;
  data_encode_scheme = VFF_DES_RAW;
  map_scheme = VFF_MS_NONE;
  map_storage_type = VFF_MAPTYP_NONE;
  maps = VXL_NULLPTR;
  map_row_size = 0;
  map_col_size = 0;
  map_subrow_size = 0;
  map_enable = VFF_MAP_OPTIONAL;
  maps_per_cycle = 0;      /* Don't care */
  color_space_model = VFF_CM_NONE;
  ispare1 = 0;
  ispare2 = 0;
  fspare1 = 0;
  fspare2 = 0;

  unsigned long image_data_n_bytes=0L; // dummy initialisation to avoid compiler warning
  
  imagedata = VXL_NULLPTR;
}

vil_viff_xvimage::vil_viff_xvimage()
{
  memset(this,0, sizeof(vil_viff_xvimage));
}

