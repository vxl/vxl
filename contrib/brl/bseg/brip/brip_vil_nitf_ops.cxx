#include "brip_vil_nitf_ops.h"
#include <vil/vil_plane.h>
#include <vil/vil_math.h>
#include <bsta/bsta_histogram.h>
//:
// \file


// Truncate the input 16 bits NITF image to a byte image by ignoring the most significant 5
// bits and less significant 3 bits
bool brip_vil_nitf_ops::truncate_nitf_bits(vil_image_view<vxl_uint_16> const& in_img,
                                           vil_image_view<vxl_byte>& output)
{
  if (in_img.ni() != output.ni() || in_img.nj() != output.nj() || in_img.nplanes() != in_img.nplanes() ) {
    std::cout << "In brip_vil_nitf_ops::truncated_nitf_bits: input image size differs from output image\n";
    return false;
  }

  int bigendian = 0;
  { union { unsigned int i; char c[4]; } u; u.i = 1; bigendian = u.c[0] == 0; }
  // loop over each pixel to perform bit operation
  for (unsigned i = 0; i < output.ni(); i++) {
    for (unsigned j = 0; j < output.nj(); j++) {
      for (unsigned p = 0; p < output.nplanes(); p++)
      {
        vxl_uint_16 curr_pixel_val = in_img(i,j,p);
        // the most significant 5 bits and less significant 3 bits are ignored
        if (bigendian) {  // big endian
          auto* arr = (unsigned char*) &curr_pixel_val;
          unsigned char big = *arr;
          unsigned char small = *(++arr);
          big >>=3;                      // [defgh3][5abc] --> [000defgh][5abc]
          small <<= 5;                   // [000defgh][5abc] ---> [000defgh][abc00000]
          curr_pixel_val = big | small;  // [abcdefgh]
        }
        else {           // little endian
          curr_pixel_val <<= 5;  // [5abcdefgh3] --> [abcdefgh35]
          curr_pixel_val >>= 8;  // [abcdefgh35] --> [abcdefgh]
        }
        auto pixel_val = static_cast<unsigned char> (curr_pixel_val);
        output(i,j,p) = pixel_val;
      }
    }
  }
  return true;
}

// Truncate the input 16 bits NITF image by ignoring the most significant 5 bits and keeping all
// other 11 bits
bool brip_vil_nitf_ops::truncate_nitf_bits(vil_image_view<vxl_uint_16> const& in_img,
                                           vil_image_view<vxl_uint_16>& output)
{
  if (in_img.ni() != output.ni() || in_img.nj() != output.nj() || in_img.nplanes() != in_img.nplanes() ) {
    std::cout << "In brip_vil_nitf_ops::truncated_nitf_bits: input image size differs from output image\n";
    return false;
  }
  // bit operation on the input NITF image
  // check endianness
  int bigendian = 0;
  { union { unsigned int i; char c[4]; } u; u.i = 1; bigendian = u.c[0] == 0; }
  // loop over each pixel to perform bit operation
  for (unsigned i = 0; i < output.ni(); i++) {
    for (unsigned j = 0; j < output.nj(); j++) {
      for (unsigned p = 0; p < output.nplanes(); p++)
      {
        vxl_uint_16 curr_pixel_val = in_img(i,j,p);
        // the most significant 5 bits are ignored and all other 11 bits are kept
        if (bigendian) {  // big endian
          auto* arr = (unsigned char*) &curr_pixel_val;
          unsigned char big = *arr;
          unsigned char small = *(++arr);
          small <<= 5;                   // [defgh3][5abc] ---> [defgh3][abc00000]
          curr_pixel_val = big | small;  // [abcdefgh]
        }
        else {           // little endian
          curr_pixel_val <<= 5;  // [5abcdefgh3] --> [abcdefgh35]
          curr_pixel_val >>= 5;  // [abcdefgh35] --> [abcdefgh]
        }
        output(i,j,p) = curr_pixel_val;
      }
    }
  }

  return true;
}

// Truncate the input 16 bits NITF image by ignoring the most significant 5 bits and keeping all
// other 11 bits
bool brip_vil_nitf_ops::scale_nitf_bits(vil_image_view<vxl_uint_16> const& in_img,
                                           vil_image_view<vxl_byte>& output_byte)
{
  vil_image_view<vxl_uint_16> output(in_img.ni(),in_img.nj(),in_img.nplanes());
  // bit operation on the input NITF image
  // check endianness
  int bigendian = 0;
  { union { unsigned int i; char c[4]; } u; u.i = 1; bigendian = u.c[0] == 0; }
  // loop over each pixel to perform bit operation
  for (unsigned p = 0; p < output.nplanes(); p++)
  {
    for (unsigned i = 0; i < output.ni(); i++) {
      for (unsigned j = 0; j < output.nj(); j++) {
        vxl_uint_16 curr_pixel_val = in_img(i,j,p);
        // the most significant 5 bits are ignored and all other 11 bits are kept
        if (bigendian) {  // big endian
            auto* arr = (unsigned char*) &curr_pixel_val;
            unsigned char big = *arr;
            unsigned char small = *(++arr);
            small <<= 5;                   // [defgh3][5abc] ---> [defgh3][abc00000]
            curr_pixel_val = big | small;  // [abcdefgh]
        }
        else {           // little endian
          curr_pixel_val <<= 5;  // [5abcdefgh3] --> [abcdefgh35]
          curr_pixel_val >>= 5;  // [abcdefgh35] --> [abcdefgh]
        }
          output(i,j,p) = curr_pixel_val;
      }
    }
    vil_image_view<vxl_uint_16> temp_plane = vil_plane<vxl_uint_16>(output,p);
    vxl_uint_16 min_val = 0, max_val = 0;
    vil_math_value_range<vxl_uint_16>(temp_plane, min_val, max_val );
    int nbins = 100;
    bsta_histogram<double> hist((double)min_val,(double)max_val,nbins);
    for (unsigned i = 0; i < output.ni(); i++)
      for (unsigned j = 0; j < output.nj(); j++)
        hist.upcount((double)temp_plane(i,j), 1.0);

    double smax_val = hist.value_with_area_above(0.0002);
    double smin_val = hist.value_with_area_below(0.0002);
    for (unsigned i = 0; i < output.ni(); i++)
      for (unsigned j = 0; j < output.nj(); j++)
      {
        int scaledval = std::floor((float)(temp_plane(i,j)-smin_val)/(float)(smax_val-smin_val) * 255.0f);
        scaledval = scaledval > 255 ?  255: scaledval;
        scaledval = scaledval < 0   ?    0: scaledval;
        output_byte(i,j,p) = (unsigned char) scaledval;
      }
  }
  return true;
}
