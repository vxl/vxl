// This is gel/geml/geml_matcher.h
#ifndef geml_matcher_h_
#define geml_matcher_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Geoffrey Cross, CRD, ${DATE}
//
//-----------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vcl_utility.h>

#include <vil/vil_byte.h>
#include <vil/vil_memory_image_of.h>

class geml_matcher
{
 public:
  // Constructors/Destructors--------------------------------------------------

  geml_matcher( const vil_memory_image_of<vil_byte> image1,
                const vil_memory_image_of<vil_byte> image2,
                const vcl_vector< vcl_pair<float,float> > &corners1,
                const vcl_vector< vcl_pair<float,float> > &corners2);

  virtual ~geml_matcher();

  // Operations----------------------------------------------------------------

  virtual vcl_vector< vcl_pair<int,int> > get_matches()= 0;

 protected:
  // Data Members--------------------------------------------------------------

  vil_memory_image_of<vil_byte> im1_;
  vil_memory_image_of<vil_byte> im2_;
  vcl_vector< vcl_pair<float,float> > corners1_;
  vcl_vector< vcl_pair<float,float> > corners2_;
};

#endif // geml_matcher_h_
