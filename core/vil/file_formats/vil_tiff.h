#ifndef vil2_tiff_h_
#define vil2_tiff_h_
#include <vil2/vil2_image_resource.h>
#include <vil2/vil2_file_format.h>
class vil2_tiff_file_format : public vil2_file_format
{
 public:
  char const* tag() const { return "tiff"; }
  vil2_image_resource_sptr make_input_image(vil_stream* vs) { return 0; }
  vil2_image_resource_sptr make_output_image(vil_stream* vs,
                                             unsigned ni,
                                             unsigned nj,
                                             unsigned nplanes,
                                             vil2_pixel_format ) { return 0; }
};
#endif // vil2_tiff_h_
