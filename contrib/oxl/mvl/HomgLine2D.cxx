// This is oxl/mvl/HomgLine2D.cxx
//:
// \file

#include <iostream>
#include "HomgLine2D.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vnl/algo/vnl_svd.h>

#include <mvl/Homg2D.h>
#include <mvl/HomgLineSeg2D.h>
#include <mvl/HomgOperator2D.h>

//--------------------------------------------------------------
//
//: Construct an ImplicitLine by clipping against the given bounding rectangle.
//  The return line has been allocated using new.
HomgLineSeg2D HomgLine2D::clip(int rect1_x, int rect1_y, int rect2_x, int rect2_y) const
{
  return HomgOperator2D::clip_line_to_lineseg(*this, rect1_x, rect1_y, rect2_x, rect2_y);
}

//--------------------------------------------------------------
//
//: Return some two points which are on the line.
//  The algorithm actually returns an orthonormal basis for the nullspace of l.
void HomgLine2D::get_2_points_on_line(HomgPoint2D* p1, HomgPoint2D* p2) const
{
  vnl_matrix<double> M(get_vector().data_block(), 1, 3);
  vnl_svd<double> svd(M);
  p1->set(svd.V(0,1), svd.V(1,1), svd.V(2,1));
  p2->set(svd.V(0,2), svd.V(1,2), svd.V(2,2));
}

//-----------------------------------------------------------------------------
//
//: Print to std::ostream in the format "<HomgLine2D x y w>"
std::ostream& operator<<(std::ostream& s, const HomgLine2D& p)
{
  return s << "<HomgLine2D " << p.get_vector() << ">";
}
