// This is mul/mil/mil_convert_image_2d.h
#ifndef mil_convert_image_2d_h_
#define mil_convert_image_2d_h_
//:
//  \file
//  \brief Functions to convert images from one type to another
//  \author Tim Cootes

#include <mil/mil_image_2d_of.h>
#include <vxl_config.h>

//: Copies src_im (of bytes) into dest_im (of float)
void mil_convert_image_2d(mil_image_2d_of<float>& dest_im,
                          mil_image_2d_of<vxl_byte>const& src_im);


//: Copies src_im (of bytes) into dest_im (of float) and flips actual data
void mil_convert_image_2d_flip(mil_image_2d_of<float>& dest_im,
                               mil_image_2d_of<vxl_byte>const& src_im);


//: Copies src_im (of float) into dest_im (of byte)
void mil_convert_image_2d(mil_image_2d_of<vxl_byte>& dest_im,
                          mil_image_2d_of<float>const& src_im);

//: Copies src_im (of float) into dest_im (of byte) and stretches to 0-255 range
void mil_convert_image_2d_stretch(mil_image_2d_of<vxl_byte>& dest,
                                  mil_image_2d_of<float>const& src);

//: Copies src_im (of float) into dest_im (of byte) after linear transform
//  Applies scale and offset to pixels of src_im and puts results in dest_im
//  Useful to compress range of data for display
void mil_convert_image_2d(mil_image_2d_of<vxl_byte>& dest_im,
                          mil_image_2d_of<float>const& src_im,
                          double scale, double offset);

//: Convert 3plane RGB image to 1 plane greyscale image
void mil_rgb_to_greyscale(mil_image_2d_of<vxl_byte>& g_im,
                          mil_image_2d_of<vxl_byte>const& rgb_im);

//: Convert 3plane RGB image to 1 plane greyscale image
void mil_rgb_to_greyscale(mil_image_2d_of<float>& g_im,
                          const mil_image_2d_of<float>& rgb_im);


#endif // mil_convert_image_2d_h_
