#ifdef __GNUC__
#pragma implementation
#endif

//:
//  \file

#include "vsl_conic_points.h"

#include <vcl_vector.h>
#include <vcl_algorithm.h>
#include <vcl_cassert.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_diag_matrix.h>

#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x4.h>

#include <oxp/vsl_conic_as_matrix.h>

struct theta_pair {
  double theta1;
  double theta2;
};

//: Intersect a line with the unit circle.
static theta_pair inter_line_circle(vnl_vector<double> const& l)
{
  double a=l(0);
  double b=l(1);
  double c=l(2);
  float a2=a * a;
  float b2=b * b;
  float c2=c * c;
  theta_pair phis;
  if (c2<=(a2+b2)) {
    double alpha=vcl_acos(vcl_sqrt(c2/(a2+b2)));
    if (c<0) {
      a= -a;
      b= -b;
      c= -c;
    }
    double beta=vcl_atan2(b,a)+vnl_math::pi;
    phis.theta1=beta-alpha;
    phis.theta2=beta+alpha;
  } else {
    phis.theta1=-1;
    phis.theta2=0;
  }
  return phis;
}

/*------------------------------------------------------------------*/

//: Sample the unit circle, with rate determined by distance to $l_\infty$.
static int
spaced_angles(double alpha1,
              double alpha2,
              vnl_vector<double> const& l,
              vnl_matrix<double> const& Hinv,
              vcl_vector<vnl_double_2 >* out)
{
  vcl_vector<double> ang;
  ang.push_back(alpha1);

  double k=0.05;
  double offset=0.02;

  for (double alpha = alpha1;;) {
    double d = l(0) * vcl_cos(alpha) + l(1) * vcl_sin(alpha) + l(2);
    alpha += k*d*d + offset;

    if (alpha >= alpha2)
      break;

    ang.push_back(alpha);
  }
  ang.push_back(alpha2);

  unsigned npoints = ang.size();

  vnl_double_3 point;
  vnl_double_3 tpoint;
  vnl_double_2 nonhomg;
  for (unsigned int j=0;j<ang.size();j++)
    {
      point(0)=vcl_cos(ang[j]);
      point(1)=vcl_sin(ang[j]);
      point(2)=1;
      tpoint=Hinv*point;
      out->push_back(vnl_double_2(tpoint(0)/tpoint(2), tpoint(1)/tpoint(2)));
    }
  return npoints;
}

//:
vsl_conic_points::vsl_conic_points(vsl_conic_as_matrix const& conic,
                                   double xmin, double xmax, double ymin, double ymax)
{
  vnl_double_3 left(1,0,-xmin);
  vnl_double_3 right(1,0,-xmax);
  vnl_double_3 bottom(0,1,-ymin);
  vnl_double_3 top(0,1,-ymax);

  vnl_double_3x4 boundarylines;
  boundarylines.set_column(0,left);
  boundarylines.set_column(1,right);
  boundarylines.set_column(2,bottom);
  boundarylines.set_column(3,top);

  // Presize output arrays
  points.reserve(100);
  segment_ends.reserve(4);

  // Compute homography mapping conic to unit circle.
  vnl_symmetric_eigensystem<double>  eig(conic.A());

  if (eig.D(0,0) > 0 || eig.D(2,2) < 0)
    // Imaginary
    return;

  if (eig.D(1,1) > 0) {
    // Reverse the eigensystem

    // swap d11 <-> d33
    double temp = eig.D(0,0);
    eig.D(0,0) = eig.D(2,2);
    eig.D(2,2) = temp;

    // Swap columns of V
    eig.V.inplace_transpose();
    eig.V.flipud();
    eig.V.inplace_transpose();
  }

  vnl_diag_matrix<double> Q(3);
  for (unsigned int i=0; i<3; i++)
    Q(i,i) = vcl_sqrt(vcl_fabs(eig.D(i,i)));

  vnl_matrix<double> H = Q*eig.V.transpose();

  vnl_double_3x3 Hinv = vnl_svd<double>(H).inverse();

  vcl_vector<double> alphas = find_angles(boundarylines,Hinv);

  vnl_double_3 l(0,0,1);
  vnl_double_3 l_infty = Hinv.transpose() * l;
  for (unsigned int m=0;m<alphas.size();m+=2) {
    spaced_angles(alphas[m],alphas[m+1],l_infty,Hinv, &points);
    segment_ends.push_back(points.size());
  }
}

vcl_vector<double>
vsl_conic_points::find_angles(vnl_matrix<double> const& b, vnl_matrix<double> const& Hinv)
{
  vnl_matrix<double> boundary_lines=Hinv.transpose()*b;
  vnl_double_3 l(0,0,1);
  vnl_double_3 l_infty = Hinv.transpose() * l;
  vcl_vector<double> phi;
  theta_pair phis;
  for (unsigned int i=0;i<4;i++)
   {
     phis = inter_line_circle(boundary_lines.get_column(i));
     if (phis.theta1 != -1)
       {
         if (phis.theta1<0)
           {
             phis.theta1 +=2*vnl_math::pi;
           }
         if (phis.theta1>2*vnl_math::pi)
           {
             phis.theta1 -= 2*vnl_math::pi;
           }
         if (phis.theta2<0)
           {
             phis.theta2  += 2*vnl_math::pi;
           }
         if (phis.theta2 >2*vnl_math::pi)
           {
             phis.theta2 -= 2*vnl_math::pi;
           }
         phi.push_back(phis.theta1);
         phi.push_back(phis.theta2); // place phi values in array
       }
   }
  if (phi.size()==0)
    {
      // fully inside
      phi.push_back(1);//atan2(l_infty[1], l_infty[0])); gsgs
    }
  vcl_sort(phi.begin(), phi.end());

  for (unsigned int n=0;n<phi.size();n++)
    {
    phi[n]+=2*vnl_math::pi;
    }
  phi.push_back(phi[0]+2*vnl_math::pi);

  vcl_vector<double> alphas;

  // Check that b is of the special form
  //  1     1     0     0
  //  0     0     1     1
  // -xmin -xmax -ymin -ymax
  assert(b(0,0) == 1 && b(0,1) == 1 && b(0,2) == 0 && b(0,3) == 0);
  assert(b(1,0) == 0 && b(1,1) == 0 && b(1,2) == 1 && b(1,3) == 1);
  assert(-b(2,0) < -b(2,1)); // xmin < xmax
  assert(-b(2,2) < -b(2,3)); // ymin < ymax

  for (unsigned int j=0;j<(phi.size()-1);j++)
    {
      double alpha = (phi[j]+phi[j+1])/2;
      vnl_double_3  a(vcl_cos(alpha),vcl_sin(alpha),1);
      vnl_double_3 H_mp=Hinv*a;
      double Hmx = H_mp[0]/H_mp[2];
      double Hmy = H_mp[1]/H_mp[2];

      // FIXME assumes b is of very special form
      if ((Hmx>-b(2,0))&& (Hmx<-b(2,1))&&
          (Hmy>-b(2,2))&& (Hmy<-b(2,3)))// then accept
        {
          alphas.push_back(phi[j]);
          alphas.push_back(phi[(j+1)]);
        }
    }
  return alphas;
}
