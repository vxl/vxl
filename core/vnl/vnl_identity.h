//-*- c++ -*-------------------------------------------------------------------
#ifndef vnl_identity_h_
#define vnl_identity_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : Identity
//
// .SECTION Description
//    Identity is a class that awf hasn't documented properly. DOCME
//
// .NAME        Identity - Undocumented class DOCME
// .LIBRARY     vnl
// .HEADER	Numerics Package
// .INCLUDE     vnl/vnl_identity.h
// .FILE        vnl/vnl_identity.cxx
// .FILE        Identity.C
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 07 Dec 98
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_unary_function.h>

template <class T>
class vnl_identity : public vnl_unary_function<T,T> { public:
  vnl_unary_function<T,T>* Copy() const {
    vnl_identity<T>* copy = new vnl_identity<T>;
    *copy = *this;
    return copy;
  }
  
  T f(T const& x) {
    return x;
  }
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS Identity.

