#ifndef vnl_float_2_h_
#define vnl_float_2_h_
#ifdef __GNUC__
#pragma interface
#endif

// vnl_float_2:  A vnl_vector of 2 floats.
//
// Author:  Andrew W. Fitzgibbon, Oxford RRG, 04 Aug 96

#include <vnl/vnl_vector_fixed.h>

class vnl_float_2 : public vnl_vector_fixed<float,2> {
public:
  vnl_float_2() {}
  vnl_float_2(const vnl_vector<float>& rhs): vnl_vector_fixed<float,2>(rhs) {}
  vnl_float_2(float xx, float yy) {
    data[0] = xx;
    data[1] = yy;
  }
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_float_2.

