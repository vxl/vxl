#ifndef ProjectiveBasis2D_h_
#define ProjectiveBasis2D_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME    ProjectiveBasis2D - Canonical basis of 4 points
// .LIBRARY MViewBasics
// .HEADER  MultiView Package
// .INCLUDE mvl/ProjectiveBasis2D.h
// .FILE    ProjectiveBasis2D.cxx
// .EXAMPLE examples/exampleProjectiveBasis2D.cxx
//
// .SECTION Description
//    Compute planar homography that maps 4 points into the canonical projective basis.
//    The returned H is such that
//    @{
//    \[ H \mmatrix{4}{& & & \\ p_1 & p_2 & p_3 & p_4 \\  &  & & } \sim
//    \mmatrix{4}{1 & 0 & 0 & 1 \cr
//                0 & 1 & 0 & 1 \cr
//                0 & 0 & 1 & 1}
//    \] @}
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 26 Sep 96
//
// .SECTION Modifications:
//     Peter Vanroose - 3 dec 1998 - "collinear_" implemented
//
//-----------------------------------------------------------------------------

//////////////#include <cool/decls.h>
#include <mvl/HMatrix2D.h>

class HomgPoint2D;

class ProjectiveBasis2D {
public:
  // Constructors/Destructors--------------------------------------------------

  // tm_get_canonical_proj
  ProjectiveBasis2D(const HomgPoint2D&, const HomgPoint2D&, const HomgPoint2D&, const HomgPoint2D&);
  ProjectiveBasis2D(const vcl_vector<HomgPoint2D>&);
  ProjectiveBasis2D(const ProjectiveBasis2D& that);
 ~ProjectiveBasis2D();

  ProjectiveBasis2D& operator=(const ProjectiveBasis2D& that);

  // Operations----------------------------------------------------------------

  // Data Access---------------------------------------------------------------

//: Return the planar homography that maps the points to the canonical frame.
  HMatrix2D& get_T() { return T_; }
  const vnl_double_3x3& get_T_matrix() const { return T_.get_matrix(); }

//: Were three of the four given basis points collinear ?
  bool collinear() const { return collinear_; }

  // Data Control--------------------------------------------------------------

protected:
  // Computations--------------------------------------------------------------
  void compute(const HomgPoint2D&, const HomgPoint2D&, const HomgPoint2D&, const HomgPoint2D&);

  // Data Members--------------------------------------------------------------
  HMatrix2D T_;
  bool collinear_;

private:
  // Helpers-------------------------------------------------------------------
};

#endif // ProjectiveBasis2D_h_
