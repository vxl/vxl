// This is gel/geml/geml_matcher_correlation.h
#ifndef geml_matcher_correlation_h_
#define geml_matcher_correlation_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Geoffrey Cross, CRD, ${DATE}
//
//-----------------------------------------------------------------------------

#include "geml_matcher.h"

class geml_matcher_correlation : public geml_matcher
{
 public:
  // Constructors/Destructors--------------------------------------------------

  geml_matcher_correlation( const vil1_memory_image_of<vxl_byte> image1,
                            const vil1_memory_image_of<vxl_byte> image2,
                            const vcl_vector< vcl_pair<float,float> > &corners1,
                            const vcl_vector< vcl_pair<float,float> > &corners2);

  // Operations----------------------------------------------------------------

  vcl_vector< vcl_pair<int,int> > get_matches();

 protected:
  // Data Members--------------------------------------------------------------

  vcl_pair<double,double> best_local_correlation_score( const int c1, const int c2);
};

#endif // geml_matcher_correlation_h_
