//-*- c++ -*-------------------------------------------------------------------
#ifndef vnl_double_4_h_
#define vnl_double_4_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : vnl_double_4
//
// .SECTION Description
//    A vnl_vector of 4 doubles.
//
// .NAME        vnl_double_4 - vnl_vector of 4 doubles. 
// .LIBRARY     vnl
// .HEADER	Numerics package
// .INCLUDE     vnl/vnl_double_4.h
// .FILE        vnl/vnl_double_4.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 04 Aug 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_vector_fixed.h>

class vnl_double_4 : public vnl_vector_fixed<double,4> {
public:
  vnl_double_4() {}
  vnl_double_4(double px, double py, double pz, double pw) {
    data[0] = px;
    data[1] = py;
    data[2] = pz;
    data[3] = pw;
  }
  vnl_double_4(const vnl_vector<double>& rhs): vnl_vector_fixed<double,4>(rhs) {}
};

#ifdef VCL_GCC_27
inline bool operator==(const vnl_double_4& a, const vnl_double_4& b)
{
  return *(vnl_vector<double> const*)&a == *(vnl_vector<double> const*)&b;
}
inline ostream& operator<<(ostream& s, const vnl_double_4& v)
{
  return s << *(vnl_vector<double> const*)&v;
}
#endif
 
#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_double_4.
