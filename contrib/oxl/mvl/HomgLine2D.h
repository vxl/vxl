// This is oxl/mvl/HomgLine2D.h
#ifndef HomgLine2D_h_
#define HomgLine2D_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif

//--------------------------------------------------------------
//:
// \file
// \brief Homogeneous 3-vector for a 2D line
//
// A class to hold a homogeneous 3-vector for a 2D line.
//
// \verbatim
// Modifications:
//   Peter Vanroose - 11 Mar 97 - added operator==
// \endverbatim

#include <vcl_iosfwd.h>
#include <mvl/Homg2D.h>

class HomgLineSeg2D;
class HomgPoint2D;

class HomgLine2D : public Homg2D
{
 public:

  HomgLine2D () {}
  HomgLine2D (const HomgLine2D& that): Homg2D(that) {}
  HomgLine2D (double px, double py, double pw): Homg2D (px, py, pw) {}
  explicit HomgLine2D (const vnl_vector<double>& vector_ptr): Homg2D (vector_ptr) {}
 ~HomgLine2D () {}

  HomgLine2D& operator=(const HomgLine2D& that)
  {
    Homg2D::operator=(that);
    return *this;
  }

  // Return true if exactly at infinity
  bool check_infinity() const;

  // Return true if $min(|x|,|y|) < \mbox{tol} \times |z|$.
  bool check_infinity(double tol) const;

  inline bool ideal(double tol = 1e-12) const { return check_infinity(tol); }

  // Clip the infinite line to the given bounding box and return
  HomgLineSeg2D clip(int x0, int y0, int x1, int y1) const;

  // Return 2 points on the line.
  void get_2_points_on_line(HomgPoint2D* p1, HomgPoint2D* p2) const;
};

vcl_ostream& operator<<(vcl_ostream& s, const HomgLine2D& );

#endif // HomgLine2D_h_
