#include "utils.h"
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>

#define ARCLENSAMPLE 0.1
#define SMALL_VALUE 1E-5

double maxof(double a, double b, double c)
{
  if (a>b && a>c)
    return a;
  else if (b>c)
    return b;
  else
    return c;
}

bool almostEqual(double a, double b, double eps)
{
  return vcl_fabs(a-b)<eps;
}

bool almostEqual(double a, double b)
{
  return vcl_fabs(a-b)<SMALL_VALUE;
}

double fixAngleMPiPi(double a)
{
  a = vcl_fmod(a,2*vnl_math::pi);
  if (a < -vnl_math::pi)
    return a+2*vnl_math::pi;
  else if (a >= vnl_math::pi)
    return a-2*vnl_math::pi;
  else
    return a;
}

double fixAngleZTPi(double a)
{
  a = vcl_fmod(a,2*vnl_math::pi);
  if (a < 0)
    return a+2*vnl_math::pi;
  else
    return a;
}

#if 0 // unused function
vcl_vector<double> smoothVector(vcl_vector<double> a, vcl_vector<double> kernel)
{
  int N=kernel.size();
  int M=a.size();
  int mid=(N-1)/2;
  vcl_vector<double> sa(M);

  for (int i=0;i<M;i++)
  {
    sa[i]=0;
    for (int j=-mid;j<=mid;j++)
    {
      double b;
      if (i-j < 0)
      {
        b=angleFixForAdd(a[i],a[0]);
        sa[i]+=(kernel[j+mid]*b);
      }
      else if (i-j >= a.size())
      {
        b=angleFixForAdd(a[i],a[M-1]);
        sa[i]+=(kernel[j+mid]*b);
      }
      else
      {
        b=angleFixForAdd(a[i],a[i-j]);
        sa[i]+=(kernel[j+mid]*b);
      }
    }
  }
  return sa;
}
#endif

double angleFixForAdd(double ref, double a)
{
  double d=a-ref;

  if (d > vnl_math::pi)
    return a-2*vnl_math::pi;
  else if (d < -vnl_math::pi)
    return a+2*vnl_math::pi;
  else
    return a;
}
