// This is brl/bseg/brip/brip_vil_nitf_ops.h
#ifndef brip_vil_nitf_ops_h_
#define brip_vil_nitf_ops_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author Yi Dong
// \brief operations on NITF format images
//
// Operation that applies onto vxl_uint_16 NITF images to generate desired output image format
//
// \verbatim
//  Modifications
//
// \endverbatim
//-----------------------------------------------------------------------------
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/file_formats/vil_nitf2_image.h>

class brip_vil_nitf_ops
{
public:
  ~brip_vil_nitf_ops() = default;

  //: Truncate the 16 bits NITF image to a byte image.
  //  For each pixel, the most significant 5 bits and less significant 3 bits are ignored
  static bool truncate_nitf_bits(vil_image_view<vxl_uint_16> const& in_img, vil_image_view<vxl_byte>& output);

  //: Truncate the 16 bits NITF image and output a vxl_unit_16 formatted image.
  //  For each pixel, the most significant 5 bits are ignored and all other 11 bits are kept
  static bool truncate_nitf_bits(vil_image_view<vxl_uint_16> const& in_img, vil_image_view<vxl_uint_16>& output);

    //: Truncate the 16 bits NITF image and output a byte image.
  //  For each pixel, the most significant 5 bits are ignored and all other 11 bits are kept
  static bool scale_nitf_bits(vil_image_view<vxl_uint_16> const& in_img, vil_image_view<vxl_byte>& output);

};

#endif  // brip_vil_nitf_ops_h_
