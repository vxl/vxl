// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//                               
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//       
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>
//-*- c++ -*-------------------------------------------------------------------
//
// Class : HMatrix2D
//
// .SECTION Description:
//
// A class to hold a plane-to-plane projective transformation matrix
// and to perform common operations using it e.g. transfer point.
//
// .NAME HMatrix2D - 3x3 plane-to-plane projectivity.
// .LIBRARY MViewBasics
// .HEADER MultiView package
// .INCLUDE mvl/HMatrix2D.h
// .FILE HMatrix2D.cxx
//
#ifndef _HMatrix2D_h
#define _HMatrix2D_h

#ifdef __GNUC__
#pragma interface
#endif

#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_4.h>
#include <mvl/HomgPoint2D.h>    
#include <mvl/HomgLine2D.h>    
#include <mvl/HomgLineSeg2D.h>   
#include <vcl/vcl_iosfwd.h>


class HMatrix2D {
  
  // PUBLIC INTERFACE----------------------------------------------------------
  
public:

// -- Flags for reduced H matrices
  enum Type {
    Euclidean,
    Similarity,
    Affine,
    Projective
  };  
  
  // Constructors/Initializers/Destructors-------------------------------------
  
  HMatrix2D();
  HMatrix2D(const HMatrix2D& M);
  HMatrix2D(const vnl_matrix<double>& M);
  HMatrix2D(const double* t_matrix);
  HMatrix2D(istream& s);
  HMatrix2D(char const* filename);
 ~HMatrix2D();
  
  // Operations----------------------------------------------------------------
  
  // Deprecated Methods:
  HomgPoint2D transform_to_plane2(const HomgPoint2D& x1) const;
  HomgLine2D  transform_to_plane1(const HomgLine2D& l2) const;
  HomgLine2D  transform_to_plane2(const HomgLine2D& l1) const;
  HomgPoint2D transform_to_plane1(const HomgPoint2D& x2) const;

  HomgPoint2D operator*(const HomgPoint2D& x1) const { return transform_to_plane2(x1); }
  HomgPoint2D operator()(const HomgPoint2D& p) const { return transform_to_plane2(p); }
  HomgLine2D preimage(const HomgLine2D& l) const { return this->transform_to_plane1(l); }

  // WARNING. these cost one vnl_svd<double>, so are here for convenience only :
  HomgPoint2D preimage(const HomgPoint2D& p) const { return this->get_inverse().transform_to_plane2(p); }
  HomgLine2D operator()(const HomgLine2D& l) const { return this->get_inverse().transform_to_plane1(l); }

  vnl_double_4 transform_bounding_box(double x0, double y0, double x1, double y1);
  
  // HomgPoint2D transform_to_plane1(const HomgPoint2D& x2) const;
  // HomgLine2D transform_to_plane2(const HomgLine2D& l1) const;

  // -- Composition
  HMatrix2D operator*(const HMatrix2D& H2) { return HMatrix2D(_t12_matrix * H2._t12_matrix); }

  // Data Access---------------------------------------------------------------
  
  double get (unsigned int row_index, unsigned int col_index) const;
  void get (double *t_matrix) const;
  void get (vnl_matrix<double>* t_matrix) const;
  const vnl_double_3x3& get_matrix () const { return _t12_matrix; }
  HMatrix2D get_inverse() const;

  void set_identity();
  void set (const double *t_matrix);
  void set (const vnl_matrix<double>& t_matrix);

  bool read(istream& s);
  bool read(char const* filename);

  // INTERNALS-----------------------------------------------------------------

  // Data Members--------------------------------------------------------------
  
private:

  vnl_double_3x3 _t12_matrix;
};

ostream& operator << (ostream& s, const HMatrix2D& H);
istream& operator >> (istream& s, HMatrix2D& H);

#endif
