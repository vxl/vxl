// This is oxl/mvl/FMatrixCompute7Point.cxx
#include "FMatrixCompute7Point.h"
//:
// \file
// \author David N. McKinnon, UQ I.R.I.S
// \date   25 Nov 00
//-----------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h>

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_svd.h>

#include <mvl/HomgMetric.h>
#include <mvl/PairMatchSetCorner.h>
#include <mvl/FDesignMatrix.h>
#include <mvl/HomgNorm2D.h>

FMatrixCompute7Point::FMatrixCompute7Point(bool precondition, bool rank2_truncate)
: precondition_(precondition),
  rank2_truncate_(rank2_truncate)
{
}

//-----------------------------------------------------------------------------
//
//: Compute a fundamental matrix for a set of 7 point matches.
//
// Return false if the calculation fails or there are fewer than eight point
// matches in the list.

bool FMatrixCompute7Point::compute(PairMatchSetCorner& matches, vcl_vector<FMatrix*>& F)
{
  // Copy matching points from matchset.
  vcl_vector<HomgPoint2D> points1(matches.count());
  vcl_vector<HomgPoint2D> points2(matches.count());
  matches.extract_matches(points1, points2);
  return compute(points1, points2, F);
}

//-----------------------------------------------------------------------------
bool FMatrixCompute7Point::compute (vcl_vector<vgl_homg_point_2d<double> >& points1,
                                    vcl_vector<vgl_homg_point_2d<double> >& points2,
                                    vcl_vector<FMatrix*>& F)
{
  if (points1.size() < 7 || points2.size() < 7) {
    vcl_cerr << "FMatrixCompute7Point: Need at least 7 point pairs.\n"
             << "Number in each set: " << points1.size() << ", " << points2.size() << vcl_endl;
    return false;
  }

  if (precondition_) {
    // Condition points
    HomgNorm2D conditioned1(points1);
    HomgNorm2D conditioned2(points2);

    // Compute F with preconditioned points
    if (!compute_preconditioned(conditioned1.get_normalized_points(),
                                conditioned2.get_normalized_points(), F))
      return false;

    // De-condition F
    for (unsigned int i = 0; i < F.size(); i++) {
      FMatrix* oldF = F[i];
      F[i] = new FMatrix(HomgMetric::homg_to_image_F(*F[i], &conditioned1,
                                                            &conditioned2));
      delete oldF;
    }
  } else
    if (!compute_preconditioned(points1, points2, F))
      return false;

  return true;
}

//-----------------------------------------------------------------------------
bool FMatrixCompute7Point::compute(vcl_vector<HomgPoint2D>& points1,
                                   vcl_vector<HomgPoint2D>& points2,
                                   vcl_vector<FMatrix*>& F)
{
  if (points1.size() < 7 || points2.size() < 7) {
    vcl_cerr << "FMatrixCompute7Point: Need at least 7 point pairs.\n"
             << "Number in each set: " << points1.size() << ", " << points2.size() << vcl_endl;
    return false;
  }

  if (precondition_) {
    // Condition points
    HomgNorm2D conditioned1(points1);
    HomgNorm2D conditioned2(points2);

    // Compute F with preconditioned points
    if (!compute_preconditioned(conditioned1.get_normalized_points(),
                                conditioned2.get_normalized_points(), F))
      return false;

    // De-condition F
    for (unsigned int i = 0; i < F.size(); i++) {
      FMatrix* oldF = F[i];
      F[i] = new FMatrix(HomgMetric::homg_to_image_F(*F[i], &conditioned1,
                                                            &conditioned2));
      delete oldF;
    }
  } else
    if (!compute_preconditioned(points1, points2, F))
      return false;

  return true;
}

//-----------------------------------------------------------------------------
bool FMatrixCompute7Point::compute_preconditioned(vcl_vector<vgl_homg_point_2d<double> >& points1,
                                                  vcl_vector<vgl_homg_point_2d<double> >& points2,
                                                  vcl_vector<FMatrix*>& F)
{
  // Create design matrix from conditioned points
  FDesignMatrix design(points1, points2);

  // Normalize rows for better conditioning
  design.normalize_rows();

  // Extract vnl_svd<double> of design matrix
  vnl_svd<double> svd(design);

  vnl_matrix<double> W = svd.nullspace();

  // Take the first and second nullvectors from the nullspace
  // Since rank 2 these should be the only associated with non-zero
  // root (Probably need conditioning first to be actually rank 2)
  FMatrix F1(vnl_double_3x3(W.get_column(0).data_block()));
  FMatrix F2(vnl_double_3x3(W.get_column(1).data_block()));

  // Using the fact that Det(alpha*F1 +(1 - alpha)*F2) == 0
  // find the real roots of the cubic equation that satisfy
  vcl_vector<double> a = FMatrixCompute7Point::GetCoef(F1, F2);
  vcl_vector<double> roots = FMatrixCompute7Point::solve_cubic(a);

  if (roots.empty())
    return false;

  for (unsigned int i = 0; i < roots.size(); i++) {
    vnl_matrix<double> F_temp =
      F1.get_matrix().as_ref()*roots[0] + F2.get_matrix()*(1 - roots[i]);
    F.push_back(new FMatrix(F_temp));
  }
  // Rank-truncate F
  if (rank2_truncate_) {
    for (unsigned int h = 0; h < F.size(); ++h) {
      F[h]->set_rank2_using_svd();
    }
  }
  return true;
}

//-----------------------------------------------------------------------------
bool FMatrixCompute7Point::compute_preconditioned(vcl_vector<HomgPoint2D>& points1,
                                                  vcl_vector<HomgPoint2D>& points2,
                                                  vcl_vector<FMatrix*>& F)
{
  // Create design matrix from conditioned points
  FDesignMatrix design(points1, points2);

  // Normalize rows for better conditioning
  design.normalize_rows();

  // Extract vnl_svd<double> of design matrix
  vnl_svd<double> svd(design);

  vnl_matrix<double> W = svd.nullspace();

  // Take the first and second nullvectors from the nullspace
  // Since rank 2 these should be the only associated with non-zero
  // root (Probably need conditioning first to be actually rank 2)
  FMatrix F1(vnl_double_3x3(W.get_column(0).data_block()));
  FMatrix F2(vnl_double_3x3(W.get_column(1).data_block()));

  // Using the fact that Det(alpha*F1 +(1 - alpha)*F2) == 0
  // find the real roots of the cubic equation that satisfy
  vcl_vector<double> a = FMatrixCompute7Point::GetCoef(F1, F2);
  vcl_vector<double> roots = FMatrixCompute7Point::solve_cubic(a);

  for (unsigned int i = 0; i < roots.size(); i++) {
    vnl_matrix<double> F_temp =
      F1.get_matrix().as_ref()*roots[0] + F2.get_matrix()*(1 - roots[i]);
    F.push_back(new FMatrix(F_temp));
  }
  // Rank-truncate F
  if (rank2_truncate_) {
    for (unsigned int h = 0; h < F.size(); ++h) {
      F[h]->set_rank2_using_svd();
    }
  }
  return true;
}

//
// Det(alpha*F1 +(1 - alpha)*F2) == 0
// (I obtained these coefficients from Maple (fingers crossed!!!))
//
vcl_vector<double> FMatrixCompute7Point::GetCoef(FMatrix const& F1, FMatrix const& F2)
{
  double a=F1.get(0,0), j=F2.get(0,0), aa=a-j,
         b=F1.get(0,1), k=F2.get(0,1), bb=b-k,
         c=F1.get(0,2), l=F2.get(0,2), cc=c-l,
         d=F1.get(1,0), m=F2.get(1,0), dd=d-m,
         e=F1.get(1,1), n=F2.get(1,1), ee=e-n,
         f=F1.get(1,2), o=F2.get(1,2), ff=f-o,
         g=F1.get(2,0), p=F2.get(2,0), gg=g-p,
         h=F1.get(2,1), q=F2.get(2,1), hh=h-q,
         i=F1.get(2,2), r=F2.get(2,2), ii=i-r;

  double a1=ee*ii-ff*hh, b1=ee*r+ii*n-ff*q-hh*o, c1=r*n-o*q;
  double d1=bb*ii-cc*hh, e1=bb*r+ii*k-cc*q-hh*l, f1=r*k-l*q;
  double g1=bb*ff-cc*ee, h1=bb*o+ff*k-cc*n-ee*l, i1=o*k-l*n;

  vcl_vector<double> v;
  v.push_back(aa*a1-dd*d1+gg*g1);
  v.push_back(aa*b1+a1*j-dd*e1-d1*m+gg*h1+g1*p);
  v.push_back(aa*c1+b1*j-dd*f1-e1*m+gg*i1+h1*p);
  v.push_back(c1*j-f1*m+i1*p);

  return v;
}

//--------------------
//:
//  Gives solutions to 0x^3 + ax^2 + bx + c = 0.
//  Returns the set of real solutions, so if both are imaginary it returns an
//  empty list, otherwise a list of length two.
//  v is a 4-vector of which the first element is ignored.
//-------------------
vcl_vector<double> FMatrixCompute7Point::solve_quadratic (vcl_vector<double> v)
{
   double a = v[1], b = v[2], c = v[3];
   double s = (b > 0.0) ? 1.0 : -1.0;
   double d = b * b - 4 * a * c;

   // round off error
   if ( d > -1e-5 && d < 0)
     d = 0.0;

   if (d < 0.0) // doesn't work for complex roots
      return vcl_vector<double>(); // empty list

   double q = -0.5 * ( b + s * vcl_sqrt(d));

   vcl_vector<double> l; l.push_back(q/a); l.push_back(c/q);
   return l;
}

// Compute cube root of a positive or a negative number
inline double my_cbrt(double x)
{
   return (x>=0) ? vcl_exp(vcl_log(x)/3.0) : -vcl_exp(vcl_log(-x)/3.0);
}

//------------------
//:
//  Solves a cubic and returns the real solutions.
//  Thus it returns a list of length 1 if there are 2 complex roots and 1 real,
//  of length 2 if it is in fact a quadratic with 2 solutions,
//  of length 3 if there are 3 real solutions
//     a x^3 + b x^2 + c x + d = 0
//-------------------
// Rewritten and documented by Peter Vanroose, 23 October 2001.

vcl_vector<double> FMatrixCompute7Point::solve_cubic(vcl_vector<double> v)
{
   double a = v[0], b = v[1], c = v[2], d = v[3];

   /* firstly check to see if we have approximately a quadratic */
   double len = a*a + b*b + c*c + d*d;
   if (vcl_abs(a*a/len) < 1e-6 )
      return FMatrixCompute7Point::solve_quadratic(v);

   b /= a; c /= a; d /= a; b /= 3;
   // With the substitution x = y-b, the equation becomes y^3-3qy+2r = 0:
   double q = b*b - c/3;
   double r = b*(b*b-c/2) + d/2;
   // At this point, a, c and d are no longer needed (c and d will be reused).

   if (q == 0) {
      vcl_vector<double> v; v.push_back(my_cbrt(-2*r) - b); return v;
   }

   // With the Vieta substitution y = z+q/z this becomes z^6+2rz^3+q^3 = 0
   // which is essentially a quadratic equation:

   d = r*r - q*q*q;
   if ( d >= 0.0 )
   {
      double z  = my_cbrt(-r + vcl_sqrt(d));
      // The case z=0 is excluded since this is q==0 which is handled above
      vcl_vector<double> v; v.push_back(z + q/z - b); return v;
   }

   // And finally the "irreducible case" (with 3 solutions):
   c = vcl_sqrt(q);
   double theta = vcl_acos( r/q/c ) / 3;
   vcl_vector<double> l;
   l.push_back(-2.0*c*vcl_cos(theta)                    - b);
   l.push_back(-2.0*c*vcl_cos(theta + 2*vnl_math::pi/3) - b);
   l.push_back(-2.0*c*vcl_cos(theta - 2*vnl_math::pi/3) - b);
   return l;
}
