#include "CardinalSpline.h"

// Return a list of points on the boundary of the curve. This is
// intended for drawing the curve as a list of line segments.
// \param int num_points: the number of points to return.
// \return vcl_vector<Vector3D>: a list of points.
vcl_vector<CardinalSpline::Vector3D> CardinalSpline::getPoints(int num_points)
{
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
    const vnl_matrix_fixed<double, 1, 4> &uvec, int pk)
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
    controlPoints[pk]*weightMatrix(0,1)+
    controlPoints[pk1]*weightMatrix(0,2)+
    controlPoints[pk2]*weightMatrix(0,3);
  return res;
}

// Returns the relevant point on the spline parameterised by t. t
// should be between 0 and 1, 0 being the start of the curve, 1 at the
// end. Actually, in this implementation, it is a closed curve, so 0
// and 1 will return the same point.
CardinalSpline::Vector3D CardinalSpline::getPoint(double t)
{
  // first find the relevant control points associated with t
  int n = controlPoints.size();
  t = convert_t(t);
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
CardinalSpline::Vector3D CardinalSpline::firstDerivative(double t)
{
  // first find the relevant control points associated with t
  int n = controlPoints.size();
  t = convert_t(t);
  int pk = ((int)(t*n))%n;
  // calculate the parameter u which indicates how far between pk
  // and pk1 the wanted point is.
  double u = t*n-(int)(t*n);
  vnl_matrix<double> uvec(1, 4);
  uvec(0,3) = 0;
  uvec(0,2) = 1;
  uvec(0,1) = 2*u;
  uvec(0,0) = 3*u*u;
  return (double)n*getVal(uvec, pk);
}

// Second derivative of the spline functions -
// ie [d^2_c(x)/du^2 d^2_c(y)/du^2 d^2_c(z)/du^2]^T.
CardinalSpline::Vector3D CardinalSpline::secondDerivative(double t)
{
  // first find the relevant control points associated with t
  int n = controlPoints.size();
  t = convert_t(t);
  int pk = ((int)(t*n))%n;
  // calculate the parameter u which indicates how far between pk
  // and pk1 the wanted point is.
  double u = t*n-(int)(t*n);
  vnl_matrix<double> uvec(1, 4);
  uvec(0,3) = 0;
  uvec(0,2) = 0;
  uvec(0,1) = 2;
  uvec(0,0) = 6*u;
  return (double)(n*n)*getVal(uvec, pk);
}

// This is the derivative of the distance function from a point to the
// curve at parameter t. Useful for finding the closest point to the curve.
double CardinalSpline::distanceFunctionFirstDerivative(double t,
                                                       const Vector3D &point)
{
  Vector3D curvePt = getPoint(t);
  Vector3D firstDeriv = firstDerivative(t);
  Vector3D diff = curvePt-point;
  return 2*dot_product(firstDeriv, diff);
}

// This is the second derivative of the distance function from a point to the
// curve at parameter t. Useful for finding the closest point to the curve.
double CardinalSpline::distanceFunctionSecondDerivative(double t,
                                                        const Vector3D &point)
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
double CardinalSpline::closest_point_t(const Vector3D &point)
{
  // first bracket the t value
    // do 10 steps per spline segment
  int n = controlPoints.size()*10;
  double delta = 1.0/(double)n;
  double t1=0.0, t2=delta;
  double d1 = (point-getPoint(t1)).magnitude();
  double d2 = (point-getPoint(t2)).magnitude();
  if (d2<d1)
  {
    double tmp=d1;
    d1 = d2; d2 = tmp;
    t1 = delta; t2=0.0;
  }
  double prev_dist=d2;
  for (double t=2*delta; t<1.0; t+=delta)
  {
    double dist = (point-getPoint(t)).magnitude();
    // replace d1 and arbitrarily set d2 to the point behind. It
    // could be the point in front but we'll check that next
    // round.
    if (dist<d1)
    {
      d1 = dist; t1 = t;
      d2 = prev_dist; t2 = t-delta;
    }
    // replace d2 only if it's one ahead of d1 (must keep the
    // brackets neighbouring)
    else if ((dist<d2)&&(t==t1+delta))
    {
      d2 = dist; t2 = t;
    }
    prev_dist = dist;
  }

  // now do a newton's iteration until we converge
  double t = (t1+t2)/2.0;
  delta = 1.0;
  while (delta>1e-4)
  {
    delta = distanceFunctionFirstDerivative(t, point)/
            distanceFunctionSecondDerivative(t, point);
    t -= delta;
  }

  return t;
}
