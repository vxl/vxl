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
// Class : HMatrix1D
//
// .SECTION Description:
//
// A class to hold a line-to-line projective transformation matrix
// and to perform common operations using it e.g. transfer point.
//
// .NAME HMatrix1D
// .LIBRARY MViewBasics
// .HEADER MultiView package
// .INCLUDE mvl/HMatrix1D.h
// .FILE HMatrix1D.cxx
//
#ifndef _HMatrix1D_h
#define _HMatrix1D_h

#ifdef __GNUC__
#pragma interface
#endif

#include <vnl/vnl_double_2x2.h>
#include <mvl/HomgPoint1D.h>    
#include <vcl/vcl_iosfwd.h>


class HMatrix1D {
  
  // PUBLIC INTERFACE----------------------------------------------------------
  
public:

  // Constructors/Initializers/Destructors-------------------------------------
  
  HMatrix1D();
  HMatrix1D(const HMatrix1D& M);
  HMatrix1D(const HMatrix1D&,const HMatrix1D&);// product of two HMatrix1Ds
  HMatrix1D(const vnl_matrix<double>& M);
  HMatrix1D(const double* t_matrix);
  HMatrix1D(istream& s);
 ~HMatrix1D();
 static HMatrix1D read(char const* filename);
 static HMatrix1D read(istream&);
  
  // Operations----------------------------------------------------------------

 // deprecated. also misnomers :  
 HomgPoint1D transform_to_plane2(const HomgPoint1D& x1) const;
 HomgPoint1D transform_to_plane1(const HomgPoint1D& x2) const;

 HomgPoint1D operator()(const HomgPoint1D& x1) const;
 HomgPoint1D preimage(const HomgPoint1D& x2) const;
  
  // Data Access---------------------------------------------------------------
  
  double get (unsigned int row_index, unsigned int col_index) const;
  void get (double *t_matrix) const;
  void get (vnl_matrix<double>* t_matrix) const;
  const vnl_double_2x2& get_matrix () const { return _t12_matrix; }
  const vnl_double_2x2& get_inverse () const { return _t21_matrix; }
    			
  void set (const double *t_matrix);
  void set (const vnl_matrix<double>& t_matrix);
  void set_inverse (const vnl_matrix<double>& t21_matrix);

  // INTERNALS-----------------------------------------------------------------

  // Data Members--------------------------------------------------------------
private:

  vnl_double_2x2 _t12_matrix;
  vnl_double_2x2 _t21_matrix;
};

ostream& operator << (ostream& s, const HMatrix1D& H);
istream& operator >> (istream& s, HMatrix1D& H);

#endif
