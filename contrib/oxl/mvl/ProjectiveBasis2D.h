// This is oxl/mvl/ProjectiveBasis2D.h
#ifndef ProjectiveBasis2D_h_
#define ProjectiveBasis2D_h_
//:
//  \file
// \brief Canonical basis of 4 points
//
//    Compute planar homography that maps 4 points into the canonical projective basis.
//    The returned H is such that
//    \f[ H \begin{array}{cccc}&&& \\ p_1 & p_2 & p_3 & p_4 \\ &&& \end{array} \sim
//    \begin{array}{cccc}1 & 0 & 0 & 1 \cr
//                       0 & 1 & 0 & 1 \cr
//                       0 & 0 & 1 & 1 \end{array}
//    \f]
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 26 Sep 96
//
// \verbatim
//  Modifications:
//     Peter Vanroose - 3 dec 1998 - "collinear_" implemented
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <iostream>
#include <vector>
#include <mvl/HMatrix2D.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
class HomgPoint2D;

class ProjectiveBasis2D
{
 public:
  // Constructors/Destructors--------------------------------------------------

  // tm_get_canonical_proj
  ProjectiveBasis2D(const HomgPoint2D&, const HomgPoint2D&, const HomgPoint2D&, const HomgPoint2D&);
  ProjectiveBasis2D(const std::vector<HomgPoint2D>&);
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
