//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation "geml_matcher.h"
#endif
//
// Class: geml_matcher
// Author: Geoffrey Cross, Oxford RRG
// Created: ${DATE}
// Modifications:
//   @(eval (strftime "%y%m%d")) Geoff Initial version.
//
//-----------------------------------------------------------------------------

#include "geml_matcher.h"

// Default ctor
geml_matcher::geml_matcher( const vil_memory_image_of<vil_byte> image1, 
			    const vil_memory_image_of<vil_byte> image2, 
			    const vcl_vector< vcl_pair<float,float> > &corners1,
			    const vcl_vector< vcl_pair<float,float> > &corners2)
  : im1_( image1),
    im2_( image2),
    corners1_( corners1),
    corners2_( corners2)
{
}

geml_matcher::~geml_matcher()
{
}

