#ifdef __GNUC__
#pragma implementation
#endif

#include "HomgConic.h"

#include <vcl_iostream.h>
#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_rpoly_roots.h>
#include <mvl/HomgPoint2D.h>
#include <mvl/HomgLine2D.h>

//: @{ Construct from implicit parameters. @}
HomgConic::HomgConic(double Axx, double Axy, double Ayy, double Ax, double Ay, double Ao)
{
  set(Axx, Axy, Ayy, Ax, Ay, Ao);
}

//: @{ Construct from SpatialObjects conic. Not implemented. @}
HomgConic::HomgConic(const Conic&)
{
  assert(!"I am undone!");
}

//: @{ Set from implicit parameters. @}
void HomgConic::set(double Axx, double Axy, double Ayy, double Ax, double Ay, double Ao)
{
  _matrix(0,0) = Axx;   _matrix(0,1) = Axy/2;    _matrix(0,2) = Ax/2;
  _matrix(1,0) = Axy/2; _matrix(1,1) = Ayy;      _matrix(1,2) = Ay/2;
  _matrix(2,0) = Ax /2; _matrix(2,1) = Ay /2;    _matrix(2,2) = Ao;
}

//: Extract the implict parameters from the matrix.
void HomgConic::get(double& A, double& B, double& C, double& D, double& E, double& F)
{
  A = _matrix(0,0);
  B = _matrix(1,0) + _matrix(0,1);
  C = _matrix(1,1);
  D = _matrix(0,2) + _matrix(2,0);
  E = _matrix(1,2) + _matrix(2,1);
  F = _matrix(2,2);
}

//: Set from matrix M.  M is assumed to be symmetric, although this is not
// enforced.
void HomgConic::set(const vnl_matrix<double>& M)
{
  _matrix = M;
}

//: Copy parameter matrix into matrix M.
void HomgConic::get(vnl_matrix<double>& M)
{
  M = _matrix;
}

// Computations--------------------------------------------------------------

// @{ OPERATIONS @}

//: @{ Return algebraic distance from point to conic $D_{\cal A} = q(p)$. @}
double HomgConic::F(const HomgPoint2D& p)
{
  double x = p.get_x();
  double y = p.get_y();
  double w = p.get_w();

  double A = _matrix(0,0);
  double B = _matrix(1,0) + _matrix(0,1);
  double C = _matrix(1,1);
  double D = _matrix(0,2) + _matrix(2,0);
  double E = _matrix(1,2) + _matrix(2,1);
  double F = _matrix(2,2);

  return A*x*x + B*x*y + C*y*y + D*x*w + E*y*w + F*w*w;
}

//: @{ Return polar as homogeneous line $\matx M \vect p$.
// If $\vect p$ is on the conic, this is the tangent at $\vect p$. @}
HomgLine2D HomgConic::polar(const HomgPoint2D& p)
{
  return HomgLine2D(_matrix * p.get_vector());
}

//: @{ Return approximate geometric distance $q(\vect p) / \| \nabla_x q(\vect p) \| $ @}
double HomgConic::sampson_distance(const HomgPoint2D& p)
{
  return F(p) / (4*(_matrix * p.get_vector()).magnitude());
}

//: Return geometric distance of point to conic.
double HomgConic::distance(const HomgPoint2D& p)
{
  double d;
  closest_point(p, 0, &d);
  return d;
}

//: Return closest point on conic.
HomgPoint2D HomgConic::closest_point(const HomgPoint2D& p)
{
  HomgPoint2D d;
  closest_point(p, &d, 0);
  return d;
}

// @{ HELPER FUNCTIONS @}

//: @{Return closest point {\bf x}, distance to {\bf x} and gradient $\nabla q_x$, at {\bf x}.
// If any of the output pointers are null, do not compute the corresponding quantity.
// @}
void HomgConic::closest_point(const HomgPoint2D& p, HomgPoint2D* pout, double *dout, vnl_vector<double>* gout)
{
  // *** Canonicalize to Axx x^2 + Ayy y^2 = 1

  double Axx = _matrix(0,0);
  double Axy = _matrix(1,0) + _matrix(0,1);
  double Ayy = _matrix(1,1);
  double Ax = _matrix(0,2) + _matrix(2,0);
  double Ay = _matrix(1,2) + _matrix(2,1);
  double Ao = _matrix(2,2);

  // Rotate the conic
  double theta = 0.5*vcl_atan2(Axy,Axx - Ayy);
  double cost = vcl_cos(theta);
  double sint = vcl_sin(theta);
  double sin2 = vnl_math_sqr(sint);
  double cos2 = vnl_math_sqr(cost);

  // rotation-free
  double Auu = Axx * cos2 + Ayy * sin2 + Axy * sint * cost;
  double Avv = Axx * sin2 + Ayy * cos2 - Axy * sint * cost;
  double Au =   Ax * cost + Ay * sint;
  double Av = - Ax * sint + Ay * cost;

  // calculate translation
  double tu = Au/(2*Auu);
  double tv = Av/(2*Avv);
  double C = Ao - Auu*tu*tu - Avv*tv*tv;

  // rotate and translate start point
  double x0 = p.get_x() / p.get_w();
  double y0 = p.get_y() / p.get_w();

  double u0 = cost * x0 + sint * y0 + tu;
  double v0 =-sint * x0 + cost * y0 + tv;

  // scale to C == -1
  Auu = -Auu / C;
  Avv = -Avv / C;

  // temps
  double u02 = u0*u0;   double v02 = v0*v0;
  double ai = 1 / Auu;  double bi = 1 / Avv;

  // set up quartic polynomial
  vnl_vector<double> poly(5);
  poly[0] = 1;
  poly[1] = -2.0*(ai+bi);
  poly[2] = -(u02*ai+v02*bi-ai*ai-4.0*ai*bi-bi*bi);
  poly[3] = 2.0*(u02+v02-ai-bi)*ai*bi;
  poly[4] = -(u02*bi+v02*ai-ai*bi)*ai*bi;
  vnl_rpoly_roots roots(poly);

  double ans[4];
  int nsols = 0;
  {
    for(int i = 0; i < 4; i++)
      if (vnl_math_abs(roots.imag(i)) < 1e-10)
        ans[nsols++] = roots.real(i);
  }

  // Find closest.  length = vcl_sqrt(lambda|G|)
  double minu=0,minv=0; // needs some initialisation, to make the compiler happy - PVR
  double distance = 1e20; // large enough to be sure it will be lowered
  for(int k = 0; k < nsols; k++) {
    double lambda = ans[k];

    // Calculate point and gradient
    double u = u0 / (1 - lambda*Auu);
    double v = v0 / (1 - lambda*Avv);

    double du = u-u0;
    double dv = v-v0;
    double dist = du*du + dv*dv;

    if (dist < distance) {
      distance = dist;
      minu = u;
      minv = v;
    }
  }
  if (dout)
    *dout = distance;

  if (pout) {
    // Translate back
    double u = minu - tu;
    double v = minv - tv;

    // Rotate point and gradient back onto conic
    double x = cost * u - sint * v;
    double y = sint * u + cost * v;

    // Assign to o/p variable
    *pout = HomgPoint2D(x, y, 1);

    // Display the distance, and check that the point is indeed on the conic
    double F = this->F(*pout);
    if (vcl_fabs(F) > 1e-9) {
      vcl_cerr << "HomgConic::get_closest_point() F = " << F << "  distance = " << vcl_sqrt(distance) << vcl_endl;
      vcl_cerr << "WARNING: F = " << F << ".  It should be real tiny\n";
    }
  }

  // Rotate gradient back if required
  if (gout) {
    double Gu = -Auu * minu;
    double Gv = -Avv * minv;
    vnl_vector<double>& G = *gout;
    G[0] = cost * Gu - sint * Gv;
    G[1] = sint * Gu + cost * Gv;
    G[2] = 0;
  }
}

int HomgConic::closest_points(const HomgPoint2D& p, HomgPoint2D pout[4])
{
  // *** Canonicalize to Axx x^2 + Ayy y^2 = 1

  double Axx = _matrix(0,0);
  double Axy = _matrix(1,0) + _matrix(0,1);
  double Ayy = _matrix(1,1);
  double Ax = _matrix(0,2) + _matrix(2,0);
  double Ay = _matrix(1,2) + _matrix(2,1);
  double Ao = _matrix(2,2);

  // Rotate the conic
  double theta = 0.5*vcl_atan2(Axy,Axx - Ayy);
  double cost = vcl_cos(theta);
  double sint = vcl_sin(theta);
  double sin2 = vnl_math_sqr(sint);
  double cos2 = vnl_math_sqr(cost);

  // rotation-free
  double Auu = Axx * cos2 + Ayy * sin2 + Axy * sint * cost;
  double Avv = Axx * sin2 + Ayy * cos2 - Axy * sint * cost;
  double Au =   Ax * cost + Ay * sint;
  double Av = - Ax * sint + Ay * cost;

  // calculate translation
  double tu = Au/(2*Auu);
  double tv = Av/(2*Avv);
  double C = Ao - Auu*tu*tu - Avv*tv*tv;

  // rotate and translate start point
  double x0 = p.get_x() / p.get_w();
  double y0 = p.get_y() / p.get_w();

  double u0 = cost * x0 + sint * y0 + tu;
  double v0 =-sint * x0 + cost * y0 + tv;

  // scale to C == -1
  Auu = -Auu / C;
  Avv = -Avv / C;

  // temps
  double u02 = u0*u0;   double v02 = v0*v0;
  double ai = 1 / Auu;  double bi = 1 / Avv;

  // set up quartic polynomial
  vnl_vector<double> poly(5);
  poly[0] = 1;
  poly[1] = -2.0*(ai+bi);
  poly[2] = -(u02*ai+v02*bi-ai*ai-4.0*ai*bi-bi*bi);
  poly[3] = 2.0*(u02+v02-ai-bi)*ai*bi;
  poly[4] = -(u02*bi+v02*ai-ai*bi)*ai*bi;
  vnl_rpoly_roots roots(poly);

  double ans[4];
  int nsols = 0;
  {
    for(int i = 0; i < 4; i++)
      if (vnl_math_abs(roots.imag(i)) < 1e-10)
        ans[nsols++] = roots.real(i);
  }

  // Find closest.  length = vcl_sqrt(lambda|G|)
  for(int k = 0; k < nsols; k++) {
    double lambda = ans[k];

    // Calculate point and gradient
    double u = u0 / (1 - lambda*Auu);
    double v = v0 / (1 - lambda*Avv);

    // Translate back
    u = u - tu;
    v = v - tv;

    // Rotate point and gradient back onto conic
    double x = cost * u - sint * v;
    double y = sint * u + cost * v;

    // Assign to o/p variable
    pout[k] = HomgPoint2D(x, y, 1);
  }

  return nsols;
}
