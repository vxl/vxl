#include "CardinalSpline.h"
#include <vsl/vsl_vector_io.txx>
#include <vcl_cassert.h>

// Return a list of points on the boundary of the curve. This is
// intended for drawing the curve as a list of line segments.
// \param int num_points: the number of points to return.
// \return vcl_vector<Vector3D>: a list of points.
vcl_vector<CardinalSpline::Vector3D> CardinalSpline::getPoints(int num_points) const
{
  assert(num_points!=0);
  vcl_vector<Vector3D> points;
  Vector3D current_point;
  for (int i=0; i<num_points; i++)
  {
    current_point = getPoint((double)i/(double)num_points);
    points.push_back(current_point);
  }
  return points;
}

// Internal helper function. This is a commonly computed value which
// I've just put in it's own method for convenience.
CardinalSpline::Vector3D CardinalSpline::getVal(
    const vnl_matrix_fixed<double, 1, 4> &uvec, int pk) const
{
  // first find the relevant control points associated with t
  int n = controlPoints.size();
  int pkn1 = (pk+n-1)%n;
  int pk1 = (pk+1)%n;
  int pk2 = (pk+2)%n;

  vnl_matrix_fixed<double, 1, 4> weightMatrix;
  weightMatrix = uvec*Mc;

  // now weight the control points
  Vector3D res = controlPoints[pkn1]*weightMatrix(0,0)+
                 controlPoints[pk  ]*weightMatrix(0,1)+
                 controlPoints[pk1 ]*weightMatrix(0,2)+
                 controlPoints[pk2 ]*weightMatrix(0,3);
  return res;
}

// Returns the relevant point on the spline parameterised by t. t
// should be between 0 and 1, 0 being the start of the curve, 1 at the
// end. Actually, in this implementation, it is a closed curve, so 0
// and 1 will return the same point.

// I am currently double checking this, I'm not convinced getPoint,
// firstDeriv etc work near t=0.0 and t=1.0
CardinalSpline::Vector3D CardinalSpline::getPoint(double t) const
{
  assert(t>=0.0);
  assert(t<=1.0);
  // first find the relevant control points associated with t
  int n = controlPoints.size();
  assert(n!=0);
  int pk = ((int)(t*n))%n;
  // calculate the parameter u which indicates how far between pk
  // and pk1 the wanted point is.
  double u = t*n-(int)(t*n);
  vnl_matrix<double> uvec(1, 4);
  uvec(0,3) = 1;
  for (int i=2; i>=0; i--) uvec(0,i) = uvec(0,i+1)*u;
  return getVal(uvec, pk);
}

// Gradient of the spline functions - ie [d_c(x)/du d_c(y)/du d_c(z)/du]^T.
CardinalSpline::Vector3D CardinalSpline::firstDerivative(double t) const
{
  assert(t>=0.0);
  assert(t<=1.0);
  // first find the relevant control points associated with t
  int n = controlPoints.size();
  int pk = ((int)(t*n))%n;
  // calculate the parameter u which indicates how far between pk
  // and pk1 the wanted point is.
  double u = t*n-(int)(t*n);

  vnl_matrix<double> uvec(1, 4);
  uvec(0,3) = 0;
  uvec(0,2) = 1;
  uvec(0,1) = 2*u;
  uvec(0,0) = 3*u*u;
  //for (int i=1; i>=0; i--) uvec(0,i) = uvec(0,i+1)*u;
  return ((double)n)*getVal(uvec, pk);
}

// Second derivative of the spline functions -
// ie [d^2_c(x)/du^2 d^2_c(y)/du^2 d^2_c(z)/du^2]^T.
CardinalSpline::Vector3D CardinalSpline::secondDerivative(double t) const
{
  assert(t>=0.0);
  assert(t<=1.0);
  // first find the relevant control points associated with t
  int n = controlPoints.size();
  int pk = ((int)(t*n))%n;
  // calculate the parameter u which indicates how far between pk
  // and pk1 the wanted point is.
  double u = t*n-(int)(t*n);

  vnl_matrix<double> uvec(1, 4);
  uvec(0,3) = 0;
  uvec(0,2) = 0;
  uvec(0,1) = 2;
  uvec(0,0) = 6*u;
  return ((double)(n*n))*getVal(uvec, pk);
}

// This is the derivative of the distance function from a point to the
// curve at parameter t. Useful for finding the closest point to the curve.
double CardinalSpline::distanceFunctionFirstDerivative(double t,
                                                       const Vector3D &point) const
{
  Vector3D curvePt = getPoint(t);
  Vector3D firstDeriv = firstDerivative(t);
  Vector3D diff = curvePt-point;
  return 2*dot_product(firstDeriv, diff);
}

double CardinalSpline::distanceFunctionSecondDerivative(double t,
                                                        const Vector3D &point) const
{
  Vector3D curvePt = getPoint(t);
  Vector3D diff = curvePt-point;
  Vector3D firstDeriv = firstDerivative(t);
  Vector3D secondDeriv = secondDerivative(t);
  return 2*dot_product(secondDeriv, diff)
       + 2*dot_product(firstDeriv, firstDeriv);
}

// Return the t value of the closest point to the input point. This is
// calculated using Newton's method after an initial estimate is
// bracketed using the control points of the spline.
double CardinalSpline::closest_point_t(const Vector3D &point) const
{
  // first bracket the t value
  // do 10 steps per spline segment
  int n = controlPoints.size()*10;
  double delta = 1.0/(double)n;
  double t1=0.0;
  double d1 = (point-getPoint(t1)).magnitude();
  for (double t=delta; t<1.0; t+=delta)
  {
    double dist = (point-getPoint(t)).magnitude();
    // replace d1
    if (dist<d1)
    {
      d1 = dist; t1 = t;
    }
  }

  // now we need to find the second closest neighbouring pt
  double t2 = t1+delta;
  if (t2>1.0) t2 = 0.0; // wraparound
  double t3 = t1-delta;
  if (t3<0.0) t3 = 1.0-delta;

  // find closest neighbour
  double d2 = (point-getPoint(t2)).magnitude();
  double d3 = (point-getPoint(t3)).magnitude();
  if (d3<d2)
  {
    t2 = t3; d2 = d3;
  }

  // now if the neighbours happen to be pt 0 and pt 1-delta, then
  // set pt 0 to pt 1
  // can only happen if there's a wraparound
  if (vcl_fabs(t2-t1)>2.0*delta)
  {
    if (t2>t1) t1 = 1.0;
    else t2 = 1.0;
  }

  // now do a Newton's iteration until we converge
  // do a simple interpolation
  double t = t1; // = (d2*t1+d1*t2)/(d1+d2);
  delta = 1.0;
  int numloops=0;
  delta = distanceFunctionFirstDerivative(t, point)/
          distanceFunctionSecondDerivative(t, point);
  while (vcl_fabs(delta)>1e-8)
  {
    t -= delta;
    if (t<0.0) t += 1.0;
    if (t>1.0) t -= 1.0;
    double dist = (point-getPoint(t)).magnitude();
    // if the following condition is true, then Newton's method
    // is not converging (or at least not my implementation),
    // so just return the closest point found so far
    if (dist>d1)
    {
      t = t1;
      vcl_cerr << "Closest point not converging:\n"
               << " t = " << t
               << ", dist = " << dist << ", d1 = " << d1
               << ", delta = " << delta << vcl_endl;
      break;
    }
    if (dist<d1)
    {
      t1 = t; d1 = dist;
    }
    if (++numloops>50)
      assert(false);
    delta = distanceFunctionFirstDerivative(t, point)/
            distanceFunctionSecondDerivative(t, point);
  }

  return t;
}

//
// binary I/O
//

VSL_VECTOR_IO_INSTANTIATE(CardinalSpline::Vector3D);

void CardinalSpline::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, controlPoints);
  vsl_b_write(os, Mc);
  vsl_b_write(os, s);
}

void CardinalSpline::b_read(vsl_b_istream &is)
{
  vsl_b_read(is, controlPoints);
  vsl_b_read(is, Mc);
  vsl_b_read(is, s);
}

void vsl_b_write(vsl_b_ostream &os, const CardinalSpline &e)
{
  e.b_write(os);
}

void vsl_b_read(vsl_b_istream &is, CardinalSpline &e)
{
  e.b_read(is);
}

void vsl_print_summary(vcl_ostream &os, const CardinalSpline &e)
{
  e.print_summary(os);
}
