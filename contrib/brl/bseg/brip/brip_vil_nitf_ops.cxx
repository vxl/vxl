#include "brip_vil_nitf_ops.h"
//:
// \file


// Truncate the input 16 bits NITF image to a byte image by ignoring the most significant 5
// bits and less significant 3 bits
bool brip_vil_nitf_ops::truncate_nitf_bits(vil_nitf2_image const& nitf,
                                           vil_image_view<vxl_byte>& output)
{
  vil_image_view_base_sptr in_img = nitf.get_copy_view();
  if (!in_img) {
    vcl_cout << "In brip_vil_nitf_ops::truncated_nitf_bits: can not get copy view from input NITF image\n";
    return false;
  }
  if (in_img->ni() != output.ni() || in_img->nj() != output.nj() || in_img->nplanes() != in_img->nplanes() ) {
    vcl_cout << "In brip_vil_nitf_ops::truncated_nitf_bits: input NITF image size differs from output image\n";
    return false;
  }

  // bit operation on the input NITF image
  if ( vil_image_view<vxl_uint_16>* img_short = dynamic_cast<vil_image_view<vxl_uint_16>*>(in_img.ptr()) )
  {
    // check endianness
    int bigendian = 0;
    { union { unsigned int i; char c[4]; } u; u.i = 1; bigendian = u.c[0] == 0; }
    // loop over each pixel to perform bit operation
    for (unsigned i = 0; i < output.ni(); i++) {
      for (unsigned j = 0; j < output.nj(); j++) {
        for (unsigned p = 0; p < output.nplanes(); p++)
        {
          vxl_uint_16 curr_pixel_val = (*img_short)(i,j,p);
          // the most significant 5 bits and less significant 3 bits are ignored
          if (bigendian) {  // big endian
            unsigned char* arr = (unsigned char*) &curr_pixel_val;
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
          unsigned char pixel_val = static_cast<unsigned char> (curr_pixel_val);
          output(i,j,p) = pixel_val;
        }
      }
    }
  }
  else if ( vil_image_view<vxl_byte>* img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(in_img.ptr()) )
  {
    for (unsigned i = 0; i < output.ni(); i++)
      for (unsigned j = 0; j < output.nj(); j++)
        for (unsigned p = 0; p < output.nplanes(); p++)
          output(i,j,p) = (*img_byte)(i,j,p);
  }
  else
  {
    vcl_cout << "In brip_vil_nitf_ops::truncated_nitf_bits: Unsupported NITF pixel format = " << in_img->pixel_format() << '\n';
    return false;
  }

  return true;
}

// Truncate the input 16 bits NITF image by ignoring the most significant 5 bits and keeping all
// other 11 bits
bool brip_vil_nitf_ops::truncate_nitf_bits(vil_nitf2_image const& nitf,
                                           vil_image_view<vxl_uint_16>& output)
{
  vil_image_view_base_sptr in_img = nitf.get_copy_view();
  if (!in_img) {
    vcl_cout << "In brip_vil_nitf_ops::truncated_nitf_bits: can not get copy view from input NITF image\n";
    return false;
  }
  if (in_img->ni() != output.ni() || in_img->nj() != output.nj() || in_img->nplanes() != in_img->nplanes() ) {
    vcl_cout << "In brip_vil_nitf_ops::truncated_nitf_bits: input NITF image size differs from output image\n";
    return false;
  }
  // bit operation on the input NITF image
  if ( vil_image_view<vxl_uint_16>* img_short = dynamic_cast<vil_image_view<vxl_uint_16>*>(in_img.ptr()) )
  {
    // check endianness
    int bigendian = 0;
    { union { unsigned int i; char c[4]; } u; u.i = 1; bigendian = u.c[0] == 0; }
    // loop over each pixel to perform bit operation
    for (unsigned i = 0; i < output.ni(); i++) {
      for (unsigned j = 0; j < output.nj(); j++) {
        for (unsigned p = 0; p < output.nplanes(); p++)
        {
          vxl_uint_16 curr_pixel_val = (*img_short)(i,j,p);
          // the most significant 5 bits are ignored and all other 11 bits are kept
          if (bigendian) {  // big endian
            unsigned char* arr = (unsigned char*) &curr_pixel_val;
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
  }
  else if ( vil_image_view<vxl_byte>* img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(in_img.ptr()) )
  {
    for (unsigned i = 0; i < output.ni(); i++)
      for (unsigned j = 0; j < output.nj(); j++)
        for (unsigned p = 0; p < output.nplanes(); p++)
          output(i,j,p) = static_cast<vxl_uint_16>( (*img_byte)(i,j,p) );
  }
  else
  {
    vcl_cout << "In brip_vil_nitf_ops::truncated_nitf_bits: Unsupported NITF pixel format = " << in_img->pixel_format() << '\n';
    return false;
  }

  return true;
}