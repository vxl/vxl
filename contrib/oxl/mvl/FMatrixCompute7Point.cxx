#ifdef __GNUC__
#pragma implementation
#endif

//-----------------------------------------------------------------------------
//
// Class: FMatrixCompute7Point
// Author: David N. McKinnon, UQ I.R.I.S
// Created: 25 Nov 00
//
//-----------------------------------------------------------------------------

#include "FMatrixCompute7Point.h"

#include <vcl/vcl_cassert.h>
#include <vcl/vcl_vector.h>
#include <vcl/vcl_iostream.h>
#include <vcl/vcl_cmath.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_rpoly_roots.h>

#include <mvl/HomgMetric.h>
#include <mvl/PairMatchSetCorner.h>
#include <mvl/FDesignMatrix.h>
#include <mvl/HomgNorm2D.h>

FMatrixCompute7Point::FMatrixCompute7Point(bool precondition, bool rank2_truncate):
  precondition_(precondition),
  rank2_truncate_(rank2_truncate)
{
}

//-----------------------------------------------------------------------------
//
// - Compute a fundamental matrix for a set of 7 point matches.
// 
// Return false if the calculation fails or there are fewer than eight point
// matches in the list.
//

bool FMatrixCompute7Point::compute(PairMatchSetCorner& matches, vcl_vector<FMatrix*>& F)
{
  // Copy matching points from matchset.
  vcl_vector<HomgPoint2D> points1(matches.count()); 
  vcl_vector<HomgPoint2D> points2(matches.count());
  matches.extract_matches(points1, points2); 
  compute(points1, points2, F);
  return true;
}

//-----------------------------------------------------------------------------
bool FMatrixCompute7Point::compute (vcl_vector<HomgPoint2D>& points1, vcl_vector<HomgPoint2D>& points2, vcl_vector<FMatrix*>& F)
{
  
  if (points1.size() < 7 || points2.size() < 7) {
    vcl_cerr << "FMatrixCompute7Point: Need at least 7 point pairs." << vcl_endl;
    vcl_cerr << "Number in each set: " << points1.size() << ", " << points2.size() << vcl_endl;
    return false;
  }

  if (precondition_) {
    // Condition points
    HomgNorm2D conditioned1(points1);
    HomgNorm2D conditioned2(points2);

    // Compute F with preconditioned points
    if(!compute_preconditioned(conditioned1.get_normalized_points(), conditioned2.get_normalized_points(), F))
      return false;

    // De-condition F
    for(int i = 0; i < F.size(); i++) {
      F[i] = new FMatrix(HomgMetric::homg_to_image_F(*F[i], &conditioned1, &conditioned2));
    }
    
  } else
    if(!compute_preconditioned(points1, points2, F))
      return false;

  return true;
}

//-----------------------------------------------------------------------------
bool FMatrixCompute7Point::compute_preconditioned (vcl_vector<HomgPoint2D>& points1, vcl_vector<HomgPoint2D>& points2, vcl_vector<FMatrix*>& F)
{
  // Create design matrix from conditioned points
  FDesignMatrix design(points1, points2);
  vnl_matrix<double> F_temp;
  vnl_vector<double> *a = new vnl_vector<double>(4);
  FMatrix *F1 = new FMatrix(), *F2 = new FMatrix();
  
  // Normalize rows for better conditioning
  design.normalize_rows();
  
  // Extract vnl_svd<double> of design matrix
  vnl_svd<double> svd(design);
  
  vnl_matrix<double> W = svd.nullspace();

  // Take the first and second nullvectors from the nullspace
  // Since rank 2 these should be the only associated with non-zero
  // root (Probably need conditioning first to be actually rank 2) 
  F1->set(vnl_double_3x3(W.get_column(0).data_block()));
  F2->set(vnl_double_3x3(W.get_column(1).data_block()));
  vnl_vector<double> roots(3);
  
  // Using the fact that Det(alpha*F1 +(1 - alpha)*F2) == 0
  // find the real roots of the cubic equation that satisfy
  // There will either be 1 real and 2 complex conjugate or
  // 3 real - note only real components taken
  // Or there will be to real roots forming a quadratic
  // (Special form)
  int no_solns = 0;
  double one, two, three;
  if(FMatrixCompute7Point::GetCoef(F1, F2, a))   
    no_solns = solve_cubic(a->get(0), a->get(1), a->get(2), a->get(3), &one, &two, &three);
  else
    return false;
  if(one == 0.0 && two == 0.0 && three == 0.0)
    no_solns = 0;
  
  roots.put(0, one);
  roots.put(1, two);
  roots.put(2, three);
  for(int i = 0; i < no_solns; i++) {
    // F = alpha*F1 + (1 - alpha)*F2
    FMatrix *temp = new FMatrix(); 
    F_temp = F1->get_matrix()*roots.get(0) + F2->get_matrix()*(1 - roots.get(i)); 
    temp->set(F_temp);
    F.push_back(temp);
  }
  // Rank-truncate F
  if (rank2_truncate_) {
    for(int h = 0; h < F.size(); h++) {
      F[h]->set_rank2_using_svd();   
    }
  }
  
  delete F1, F2;
  return true;
}

//
// Det(alpha*F1 +(1 - alpha)*F2) == 0 (I obtained these co-efficients from Maple (fingers crossed!!!))
//
bool FMatrixCompute7Point::GetCoef(FMatrix* F1, FMatrix* F2, vnl_vector<double> *vect)
{ 
  double a=F1->get(0,0), b=F1->get(0,1), c=F1->get(0,2), d=F1->get(1,0), e=F1->get(1,1), f=F1->get(1,2),g=F1->get(2,0),h=F1->get(2,1),i=F1->get(2,2);
  double j=F2->get(0,0), k=F2->get(0,1), l=F2->get(0,2), m=F2->get(1,0), n=F2->get(1,1), o=F2->get(1,2),p=F2->get(2,0),q=F2->get(2,1),r=F2->get(2,2);
  
  double aa=a-j, bb=b-k, cc=c-l, dd=d-m, ee=e-n, ff=f-o, gg=g-p, hh=h-q, ii=i-r;
  
  double a1=ee*ii-ff*hh, b1=ee*r+ii*n-ff*q-hh*o, c1=r*n-o*q;
  double d1=bb*ii-cc*hh, e1=bb*r+ii*k-cc*q-hh*l, f1=r*k-l*q;
  double g1=bb*ff-cc*ee, h1=bb*o+ff*k-cc*n-ee*l, i1=o*k-l*n;
  
  double aaa = aa*a1-dd*d1+gg*g1;
  double bbb = aa*b1+a1*j-dd*e1-d1*m+gg*h1+g1*p;
  double ccc = aa*c1+b1*j-dd*f1-e1*m+gg*i1+h1*p;
  double ddd = c1*j-f1*m+i1*p;
  
  vect->put(0, aaa);
  vect->put(1, bbb);
  vect->put(2, ccc);
  vect->put(3, ddd);

  return true;
}

/*******************
*   int @solve_quadratic ( double a, double b, double c,
*                             double *x1, double *x2 )
*
*   Gives solns x1 and x2 as given by Numerical Recipes to ax^2 + bx + c = 0.
*   Returns the number of real solutions, so if both are imaginary it returns
*   zero, otherwise two.
********************/
int FMatrixCompute7Point::solve_quadratic (double a, double b, double c, double *x1, double *x2)
{
   double q, s, d;

   if (b > 0.0) s = 1.0;
   else         s = -1.0;

   d = b * b - 4 * a * c;


   // round off error
   if ( (d > -0.001) && (d < 0))
	   d = 0.0;
	   
   if (d < 0.0) /* doesn't work for complex roots */
   {
      return 0;
   }
   else q = -0.5 * ( b + s * vcl_sqrt(d));
 
   *x1 = q/a;
   *x2 = c/q;
   return 2;
}

double FMatrixCompute7Point::cbrt (double x)
{
   return vcl_exp(vcl_log(x)/3.0);
}

#define PI 3.141592967
/*******************
*   int @solve_cubic ( double a, double b, double c, double d,
*                         double *x1, double *x2, double *x3 )
*
*   Solves a cubic by formula given in Numerical Recipes and
*   returns the number of real solutions.
*   Thus it returns 1 if there are 2 complex roots and 1 real solution
*   2 if it is in fact a fact quadratic with 2 solns
*   3 if there are 3 real solns 
*      a x^3 + b x^2 + c x + d = 0
*   puts the solns into x1, x2, x3 respectively
*   (the last of these are unset if there aren't enough solns).
********************/
int FMatrixCompute7Point::solve_cubic (double a, double b, double c, double d, double *x1, double *x2, double *x3)
{
   double q, r, theta, e, f, len;

   /* firstly check to see if we have approximately a quadratic */
   len = a*a + b*b + c*c + d*d;
   if (vcl_abs(a*a/len) < 0.000001 )
      return ( solve_quadratic ( b, c, d, x1, x2 ) );

   b /= a; c /= a; d /= a; a = 1.0;
   q = (b*b - 3.0*c)/9.0;
   r = (2.0*b*b*b - 9.0*b*c + 27.0*d)/54.0;

   if ( r*r >= q*q*q )
   {
      e  = cbrt(vcl_abs(r) + vcl_sqrt(r*r - q*q*q));
      if ( e*r > 0.0) e *= -1.0; /* e has to be opposite sign of r? */

      if ( e == 0.0) f = 0.0;
      else           f = q/e;

      *x1 = e + f - b/3.0; 
      return 1;
   }

   theta = vcl_acos( r/vcl_sqrt(q*q*q) );
   *x1 = -2.0*vcl_sqrt(q)*vcl_cos(theta/3.0)              - b/3.0;
   *x2 = -2.0*vcl_sqrt(q)*vcl_cos((theta + 2.0*PI)/3.0) - b/3.0;
   *x3 = -2.0*vcl_sqrt(q)*vcl_cos((theta - 2.0*PI)/3.0) - b/3.0;
   return 3;
}
