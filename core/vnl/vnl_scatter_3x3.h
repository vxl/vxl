//-*- c++ -*-------------------------------------------------------------------
#ifndef vnl_scatter_3x3_h_
#define vnl_scatter_3x3_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : vnl_scatter_3x3
//
// .SECTION Description
//    vnl_scatter_3x3 is a class that awf hasn't documented properly. DOCME
//
// .NAME        vnl_scatter_3x3 - Undocumented class DOCME
// .LIBRARY     vnl
// .HEADER	Numerics Package
// .INCLUDE     vnl/vnl_scatter_3x3.h
// .FILE        vnl/vnl_scatter_3x3.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 02 Oct 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>

template <class T>
class vnl_scatter_3x3 : public vnl_matrix_fixed<double,3,3> {
public:
  typedef vnl_matrix_fixed<T,3,3> base;
  typedef vnl_vector_fixed<T,3> vect;

  // -- Constructor.  Fills with zeros.
  vnl_scatter_3x3();
  
  // -- Add v*v' to scatter.
  void add_outer_product(const vnl_vector_fixed<T,3> & v);
  
  // -- Add v*u' to scatter.
  void add_outer_product(const vnl_vector_fixed<T,3> & u, const vnl_vector_fixed<T,3> & v);
  
  // -- Subtract v*v' from scatter.
  void sub_outer_product(const vnl_vector_fixed<T,3> & v);
  
  // -- Subtract v*u' from scatter.
  void sub_outer_product(const vnl_vector_fixed<T,3> & u, const vnl_vector_fixed<T,3> & v);
  
  // -- Replace S with $(S+S^\top)/2$.
  void force_symmetric();
  
protected:  
  bool symmetricp;
};


#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_scatter_3x3.

