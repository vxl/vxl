#ifdef __GNUG__
#pragma implementation
#endif

#include "HomgOperator1D.h"
#include <vcl_cmath.h>
#include <vcl_iostream.h>

//-----------------------------------------------------------------------------
// -- Calculates the crossratio of four collinear points p1, p2, p3 and p4.
// This number is projectively invariant, and it is the coordinate of p4
// in the reference frame where p2 is the origin (coordinate 0), p3 is
// the unity (coordinate 1) and p1 is the point at infinity.
// This cross ratio is often denoted as ((p1, p2; p3, p4)) (which also
// equals ((p3, p4; p1, p2)) or ((p2, p1; p4, p3)) or ((p4, p3; p2, p1)) )
// and is calculated as
//                      p1 - p3   p2 - p3      (p1-p3)(p2-p4)
//                      ------- : --------  =  --------------
//                      p1 - p4   p2 - p4      (p1-p4)(p2-p3)
//
// In principle, any single nonhomogeneous coordinate from the four points
// can be used as parameters for CrossRatio (but of course the same for all
// points). The most reliable answer will be obtained when the coordinate with
// the largest spacing is used, i.e., the one with smallest slope.
//
double HomgOperator1D::CrossRatio(const Homg1D& a, const Homg1D& b, const Homg1D& c, const Homg1D& d)
{
  double x1 = a.get_x(), w1 = a.get_w();
  double x2 = b.get_x(), w2 = b.get_w();
  double x3 = c.get_x(), w3 = c.get_w();
  double x4 = d.get_x(), w4 = d.get_w();
  double n = (x1*w3-x3*w1)*(x2*w4-x4*w2);
  double m = (x1*w4-x4*w1)*(x2*w3-x3*w2);
  if (n == 0 && m == 0)
    cerr << "CrossRatio not defined: three of the given points coincide" << endl;
  return n/m;
}

//-----------------------------------------------------------------------------
// -- Cross product of two Homg1Ds
double HomgOperator1D::cross(const Homg1D& a, const Homg1D& b)
{
  double x1 = a.get_x(), w1 = a.get_w();
  double x2 = b.get_x(), w2 = b.get_w();
  return x1*w2-w1*x2;
}

//-----------------------------------------------------------------------------
// -- Dot product of two Homg1Ds
double HomgOperator1D::dot(const Homg1D& a, const Homg1D& b)
{
  double x1 = a.get_x(), w1 = a.get_w();
  double x2 = b.get_x(), w2 = b.get_w();
  return x1*x2 + w1*w2;
}

//-----------------------------------------------------------------------------
// -- Normalize Homg1D to unit magnitude

void HomgOperator1D::unitize(Homg1D* a)
{
  double x = a->get_x(), w = a->get_w();
  double norm = vcl_sqrt (x*x + w*w);
  if (norm == 0.0) {
    cerr << "HomgOperator1D::unitize() -- Zero length vector\n";
    return;
  }
  norm = 1.0/norm;
  a->set(x*norm, w*norm);
}

// -- Get the distance between the two points.
double HomgOperator1D::distance (const HomgPoint1D& a,
				 const HomgPoint1D& b)
{
  double x1 = a.get_x(), w1 = a.get_w();
  double x2 = b.get_x(), w2 = b.get_w();
  if (w1 == 0 || w2 == 0) {
    cerr << "HomgOperator1D::distance() -- point at infinity";
    return Homg::infinity;
  }
  return (x1 / w1 - x2 / w2);
}

// -- Get the square of the distance between the two points.
double HomgOperator1D::distance_squared (const HomgPoint1D& point1,
					 const HomgPoint1D& point2)
{
  double d = distance(point1,point2);
  return d*d;
}

// -- Return the midpoint of two homogeneous points
HomgPoint1D HomgOperator1D::midpoint (const HomgPoint1D& a, const HomgPoint1D& b)
{
  double x1 = a.get_x(), w1 = a.get_w();
  double x2 = b.get_x(), w2 = b.get_w();
  return HomgPoint1D(x1*w2+x2*w1, 2*w1*w2);
}

// -- Calculate the projective conjugate point, or more generally, the point
// with a given crossratio w.r.t. three other points:
// The cross ratio ((x1,x2;x3,answer)) is cr (default -1). When cr is -1,
// the returned value and x3 are conjugate points w.r.t. the pair (x1,x2).
// Because this function is transitive on coordinates, it is sufficient to
// implement it for 1-dimensional points.
// 
double HomgOperator1D::Conjugate(double x1, double x2, double x3, double cr)
// Default for cr is -1.
{
  double a = x1 - x3;  double b = x2 - x3; double c = a-cr*b;
  if (c == 0) return (x2*a == cr*x1*b) ? 1 : Homg::infinity;
  return (x2*a-cr*x1*b)/c;
}

Homg1D HomgOperator1D::Conjugate(const Homg1D& a, const Homg1D& b, const Homg1D& c, double cr)
// Default for cr is -1.
{
  double x1 = a.get_x(), w1 = a.get_w();
  double x2 = b.get_x(), w2 = b.get_w();
  double x3 = c.get_x(), w3 = c.get_w();
  double k = x1*w3 - x3*w1, m = x2*w3 - x3*w2;
  return Homg1D(x2*k-cr*x1*m, k*w2-cr*m*w1); // could be (0,0) !!  not checked.
}

