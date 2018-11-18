// This is gel/geml/geml_matcher.cxx
//
// Author: Geoffrey Cross, Oxford RRG
// Created: ${DATE}
// .SECTION Modifications:
//   @(eval (strftime "%y%m%d")) Geoff Initial version.
//
//-----------------------------------------------------------------------------

#include <utility>

#include "geml_matcher.h"

// Default ctor
geml_matcher::geml_matcher( const vil1_memory_image_of<vxl_byte>& image1,
                            const vil1_memory_image_of<vxl_byte>& image2,
                            std::vector< std::pair<float,float> > corners1,
                            std::vector< std::pair<float,float> > corners2)
  : im1_( image1),
    im2_( image2),
    corners1_(std::move( corners1)),
    corners2_(std::move( corners2))
{
}

geml_matcher::~geml_matcher() = default;
