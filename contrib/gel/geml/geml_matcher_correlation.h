//-*- c++ -*-------------------------------------------------------------------
#ifndef geml_matcher_correlation_h_
#define geml_matcher_correlation_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .SECTION Description
//    geml_matcher_correlation is a class that Geoff hasn't documented properly. FIXME
//
// .NAME        geml_matcher_correlation - Undocumented class FIXME
// .LIBRARY     geml
// .HEADER      gel package
// .INCLUDE     geml/geml_matcher_correlation.h
// .FILE        geml_matcher_correlation.cxx
// .SECTION Author
//     Geoffrey Cross, CRD, ${DATE}
//
//-----------------------------------------------------------------------------

#include <geml/geml_matcher.h>

class geml_matcher_correlation : public geml_matcher {
public:
  // Constructors/Destructors--------------------------------------------------

  geml_matcher_correlation( const vil_memory_image_of<vil_byte> image1, 
			    const vil_memory_image_of<vil_byte> image2, 
			    const vcl_vector< vcl_pair<float,float> > &corners1,
			    const vcl_vector< vcl_pair<float,float> > &corners2);

  // Operations----------------------------------------------------------------

  vcl_vector< vcl_pair<int,int> > get_matches();

protected:
  // Data Members--------------------------------------------------------------

  vcl_pair<double,double> best_local_correlation_score( const int c1, const int c2);
};

#endif // geml_matcher_correlation_h_
