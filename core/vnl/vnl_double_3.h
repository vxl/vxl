//-*- c++ -*-------------------------------------------------------------------
#ifndef vnl_double_3_h_
#define vnl_double_3_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : vnl_double_3
//
// .SECTION Description
//    A vnl_vector of 3 doubles.
//
// .NAME        vnl_double_3 - vnl_vector of 3 doubles. 
// .LIBRARY     vnl
// .HEADER	Numerics package
// .INCLUDE     vnl/vnl_double_3.h
// .FILE        vnl/vnl_double_3.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 04 Aug 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_vector_fixed.h>

class vnl_double_3 : public vnl_vector_fixed<double,3> {
public:
  vnl_double_3() {}
  vnl_double_3(const vnl_vector<double>& rhs): vnl_vector_fixed<double,3>(rhs) {}
  vnl_double_3(double px, double py, double pz) {
    data[0] = px;
    data[1] = py;
    data[2] = pz;
  }
};

#ifdef VCL_GCC_27
inline bool operator==(const vnl_double_3& a, const vnl_double_3& b)
{
  return *(vnl_vector<double> const*)&a == *(vnl_vector<double> const*)&b;
}
inline ostream& operator<<(ostream& s, const vnl_double_3& v)
{
  return s << *(vnl_vector<double> const*)&v;
}
#endif

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_double_3.
