#ifndef vil_viff_support_h_
#define vil_viff_support_h_

#include <vxl_config.h> /* for vxl_uint_32 */
struct vil_viff_xvimage;

struct vil_viff_xvimage *
vil_viff_createimage(vxl_uint_32 col_size, vxl_uint_32 row_size,
                     vxl_uint_32 data_storage_type, vxl_uint_32 num_of_images,
                     vxl_uint_32 num_data_bands, const char* comment,
                     vxl_uint_32 map_row_size, vxl_uint_32 map_col_size,
                     vxl_uint_32 map_scheme, vxl_uint_32 map_storage_type,
                     vxl_uint_32 location_type, vxl_uint_32 location_dim);
void vil_viff_freeimage (struct vil_viff_xvimage *);

#endif /* vil_viff_support_h_ */
