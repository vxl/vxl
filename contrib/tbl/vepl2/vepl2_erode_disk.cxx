#include "vepl2_erode_disk.h"
#include <vil2/vil2_rgb.h>
#include <vepl2/accessors/vipl_accessors_vil2_image_view_base.h>
#include <vipl/vipl_erode_disk.h>
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_pixel_format.h>
#include <vxl_config.h> // for vxl_byte

#if 0 // currently no erosion for colour images
static inline bool operator<(vil2_rgb<vxl_byte> const& a, vil2_rgb<vxl_byte> const& b)
{
  return a.r<b.r || (a.r==b.r && a.g<b.g) || (a.r==b.r && a.g==b.g && a.b<b.b);
}
#endif // 0

vil2_image_view_base_sptr vepl2_erode_disk(vil2_image_view_base const& image, float radius)
{
  // byte rgb
  if (image.nplanes() > 1 || image.pixel_format() == VIL2_PIXEL_FORMAT_RGB_BYTE) {
    vcl_cerr << __FILE__ ": currently no erosion of colour images\n";
    return 0;
  }

  // byte greyscale
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_BYTE) {
    vil2_image_view<vxl_byte>* out = new vil2_image_view<vxl_byte>(image.ni(),image.nj(),image.nplanes());
    vipl_erode_disk<vil2_image_view_base,vil2_image_view_base,vxl_byte,vxl_byte,vipl_trivial_pixeliter> op(radius);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // short
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_UINT_16) {
    vil2_image_view<unsigned short>* out = new vil2_image_view<unsigned short>(image.ni(),image.nj(),image.nplanes());
    vipl_erode_disk<vil2_image_view_base,vil2_image_view_base,unsigned short,unsigned short,vipl_trivial_pixeliter> op(radius);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // int
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_UINT_32) {
    vil2_image_view<unsigned>* out = new vil2_image_view<unsigned>(image.ni(),image.nj(),image.nplanes());
    vipl_erode_disk<vil2_image_view_base,vil2_image_view_base,unsigned,unsigned,vipl_trivial_pixeliter> op(radius);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // float
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_FLOAT) {
    vil2_image_view<float>* out = new vil2_image_view<float>(image.ni(),image.nj(),image.nplanes());
    vipl_erode_disk<vil2_image_view_base,vil2_image_view_base,float,float,vipl_trivial_pixeliter> op(radius);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // double
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_DOUBLE) {
    vil2_image_view<double>* out = new vil2_image_view<double>(image.ni(),image.nj(),image.nplanes());
    vipl_erode_disk<vil2_image_view_base,vil2_image_view_base,double,double,vipl_trivial_pixeliter> op(radius);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl2_erode_disk() not implemented for " << image.is_a() << '\n';
    return 0;
  }
}

