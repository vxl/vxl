#include "vil/vil_bicub_interp.h"
#include "vil/vil_bilin_interp.h"
#include "vil/vil_block_cache.h"
#include "vil/vil_border.h"
#include "vil/vil_chord.h"
#include "vil/vil_clamp.h"
#include "vil/vil_color_table.h"
#include "vil/vil_convert.h"
#include "vil/vil_copy.h"
#include "vil/vil_crop.h"
#include "vil/vil_decimate.h"
#include "vil/vil_exception.h"
#include "vil/vil_file_format.h"
#include "vil/vil_fill.h"
#include "vil/vil_flip.h"
#include "vil/vil_flatten.h"
#include "vil/vil_fwd.h"
#include "vil/vil_image_resource_sptr.h"
#include "vil/vil_image_resource.h"
#include "vil/vil_image_resource_plugin.h"
#include "vil/vil_blocked_image_resource_sptr.h"
#include "vil/vil_blocked_image_resource.h"
#include "vil/vil_blocked_image_facade.h"
#include "vil/vil_cached_image_resource.h"
#include "vil/vil_pyramid_image_resource_sptr.h"
#include "vil/vil_pyramid_image_resource.h"
#include "vil/vil_pyramid_image_view.h"
#include "vil/vil_image_list.h"
#include "vil/vil_image_view.h"
#include "vil/vil_image_view_base.h"
#include "vil/vil_load.h"
#include "vil/vil_math.h"
#include "vil/vil_memory_chunk.h"
#include "vil/vil_memory_image.h"
#include "vil/vil_nearest_interp.h"
#include "vil/vil_new.h"
#include "vil/vil_na.h"
#include "vil/vil_open.h"
#include "vil/vil_pixel_format.h"
#include "vil/vil_plane.h"
#include "vil/vil_print.h"
#include "vil/vil_property.h"
#include "vil/vil_resample_bicub.h"
#include "vil/vil_resample_bilin.h"
#include "vil/vil_resample_nearest.h"
#include "vil/vil_rgb.h"
#include "vil/vil_rgba.h"
#include "vil/vil_rotate.h"
#include "vil/vil_round.h"
#include "vil/vil_sample_grid_bicub.h"
#include "vil/vil_sample_grid_bilin.h"
#include "vil/vil_sample_profile_bicub.h"
#include "vil/vil_sample_profile_bilin.h"
#include "vil/vil_save.h"
#include "vil/vil_smart_ptr.h"
#include "vil/vil_stream.h"
#include "vil/vil_stream_read.h"
#include "vil/vil_stream_write.h"
#include "vil/vil_stream_core.h"
#include "vil/vil_stream_fstream.h"
#include "vil/vil_stream_fstream64.h"
#include "vil/vil_stream_section.h"
#include "vil/vil_stream_url.h"
#include "vil/vil_transform.h"
#include "vil/vil_transpose.h"
#include "vil/vil_view_as.h"
#include "vil/vil_warp.h"

// Image file format interface headers:
#include <vil/file_formats/vil_bmp.h>
#include <vil/file_formats/vil_bmp_core_header.h>
#include <vil/file_formats/vil_bmp_file_header.h>
#include <vil/file_formats/vil_bmp_info_header.h>
#include <vil/file_formats/vil_iris.h>
#include <vil/file_formats/vil_jpeg.h>
#include <vil/file_formats/vil_jpeg_compressor.h>
#include <vil/file_formats/vil_jpeg_decompressor.h>
#include <vil/file_formats/vil_jpeg_destination_mgr.h>
#include <vil/file_formats/vil_jpeg_source_mgr.h>
#include <vil/file_formats/vil_jpeglib.h>
#include <vil/file_formats/vil_mit.h>
#include <vil/file_formats/vil_nitf2.h>
#include <vil/file_formats/vil_nitf2_array_field.h>
#include <vil/file_formats/vil_nitf2_classification.h>
#include <vil/file_formats/vil_nitf2_compound_field_value.h>
#include <vil/file_formats/vil_nitf2_data_mask_table.h>
#include <vil/file_formats/vil_nitf2_des.h>
#include <vil/file_formats/vil_nitf2_field.h>
#include <vil/file_formats/vil_nitf2_field_definition.h>
#include <vil/file_formats/vil_nitf2_field_formatter.h>
#include <vil/file_formats/vil_nitf2_field_functor.h>
#include <vil/file_formats/vil_nitf2_field_sequence.h>
#include <vil/file_formats/vil_nitf2_header.h>
#include <vil/file_formats/vil_nitf2_image.h>
#include <vil/file_formats/vil_nitf2_image_subheader.h>
#include <vil/file_formats/vil_nitf2_index_vector.h>
#include <vil/file_formats/vil_nitf2_scalar_field.h>
#include <vil/file_formats/vil_nitf2_tagged_record.h>
#include <vil/file_formats/vil_nitf2_tagged_record_definition.h>
#include <vil/file_formats/vil_nitf2_typed_array_field.h>
#include <vil/file_formats/vil_nitf2_typed_field_formatter.h>
#include <vil/file_formats/vil_nitf2_typed_scalar_field.h>
#include <vil/file_formats/vil_png.h>
#include <vil/file_formats/vil_pnm.h>
#include <vil/file_formats/vil_pyramid_image_list.h>
#include <vil/file_formats/vil_ras.h>
#include <vil/file_formats/vil_viff.h>
#include <vil/file_formats/vil_viffheader.h>
// Only the following ones need library-specific #includes:
#if HAS_J2K
#  include <vil/file_formats/vil_j2k_image.h>
#  include <vil/file_formats/vil_j2k_nitf2_pyramid_image_resource.h>
#  include <vil/file_formats/vil_j2k_pyramid_image_resource.h>
#  include <vil/file_formats/NCSJPCVilIOStream.h>
#endif
#if HAS_OPENJPEG2
#  include <vil/file_formats/vil_openjpeg.h>
#  include <vil/file_formats/vil_openjpeg_pyramid_image_resource.h>
#endif
#if HAS_DCMTK
#  include <vil/file_formats/vil_dicom.h>
#  include <vil/file_formats/vil_dicom_header.h>
#  include <vil/file_formats/vil_dicom_stream.h>
#endif
#if HAS_TIFF
#  include <vil/file_formats/vil_tiff.h>
#  include <vil/file_formats/vil_tiff_header.h>
#  if HAS_GEOTIFF
#    include <vil/file_formats/vil_geotiff_header.h>
#  endif
#endif

int
main()
{
  return 0;
}
