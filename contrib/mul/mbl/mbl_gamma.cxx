#include <vcl_iostream.h>
#include<mbl/mbl_gamma.h>
#include<math.h>
#include <vcl_cstdlib.h> // abort()

const int MAX_ITS = 100;
const double EPS = 3.0e-7;
const double FPMIN = 1.0e-30;

double mbl_log_gamma(double xx)
{
  double x,y,tmp,ser;
  static double cof[6]={76.18009172947146,-86.50532032941677,
                        24.01409824083091,-1.231739572450155,
                        0.1208650973866179e-2,-0.5395239384953e-5};
  int j;

  y=x=xx;
  tmp=x+5.5;
  tmp -= (x+0.5)*log(tmp);
  ser=1.000000000190015;
  for (j=0;j<=5;j++) ser += cof[j]/++y;
  return -tmp+log(2.5066282746310005*ser/x);
}


static double mbl_gamma_ser(double a, double x)
{
  int n;
  double sum,del,ap,gln;

  gln=mbl_log_gamma(a);
  if (x>0)
  {
    ap=a;
    del=sum=1.0/a;
    for (n=1;n<=MAX_ITS;n++)
    {
      ++ap;
      del *= x/ap;
      sum += del;
      if (fabs(del) < fabs(sum)*EPS)
      {
        return sum*exp(-x+a*log(x)-(gln));
      }
    }
    vcl_cerr<<"mbl_gamma_ser : Failed to converge."<<vcl_endl;
    vcl_cerr<<"a = "<<a<<"   x= "<<x<<vcl_endl;
    vcl_cerr<<"Returning best guess."<<vcl_endl;
    // vcl_abort();
  }
  else
  {
    if (x < 0.0)
    {
      vcl_cerr<<"mbl_gamma_ser : x less than 0"<<vcl_endl;
      vcl_abort();
    }
  }

  return 0.0;
}


static double NR_log_gamma_cf(double a, double x)
{
  int i;
  double an,b,c,d,del,h,gln;

  gln=mbl_log_gamma(a);
  b=x+1.0-a;
  c=1.0/FPMIN;
  d=1.0/b;
  h=d;
  for (i=1;i<=MAX_ITS;i++)
  {
    an = -i*(i-a);
    b += 2.0;
    d=an*d+b;
    if (fabs(d) < FPMIN) d=FPMIN;
    c=b+an/c;
    if (fabs(c) < FPMIN) c=FPMIN;
    d=1.0/d;
    del=d*c;
    h *= del;
    if (fabs(del-1.0) < EPS) break;
  }
  if (i > MAX_ITS)
  {
    vcl_cerr<<"NR_log_gamma_cf : Failed to converge."<<vcl_endl;
    vcl_abort();
  }
  return -x+a*log(x)-(gln)+log(h);
}

static double mbl_gamma_cf(double a, double x)
{
  int i;
  double an,b,c,d,del,h,gln;

  gln=mbl_log_gamma(a);
  b=x+1.0-a;
  c=1.0/FPMIN;
  d=1.0/b;
  h=d;
  for (i=1;i<=MAX_ITS;i++)
  {
    an = -i*(i-a);
    b += 2.0;
    d=an*d+b;
    if (fabs(d) < FPMIN) d=FPMIN;
    c=b+an/c;
    if (fabs(c) < FPMIN) c=FPMIN;
    d=1.0/d;
    del=d*c;
    h *= del;
    if (fabs(del-1.0) < EPS) break;
  }
  if (i > MAX_ITS)
  {
    vcl_cerr<<"mbl_gamma_cf : Failed to converge. a="<<a<<" x="<<x<<vcl_endl;
    return 1.0; // Arbitrary
  }
  return exp(-x+a*log(x)-(gln))*h;
}

double mbl_gamma_p(double a, double x)
{
  if ((x < 0.0) || (a <= 0.0))
  {
    vcl_cerr<<"mbl_gamma_p : Invalid arguments."<<vcl_endl;
    vcl_abort();
  }

  if (x < (a+1.0))
  {
    return mbl_gamma_ser(a,x); // Use series representation
  }
  else
  {
    return 1 - mbl_gamma_cf(a,x); // Use continued fraction representation
  }
}

double mbl_gamma_q(double a, double x)
{
  if ((x < 0.0) || (a <= 0.0))
  {
    vcl_cerr<<"mbl_gamma_q : Invalid arguments."<<vcl_endl;
    vcl_abort();
  }

  if (x < (a+1.0))
  {
    return 1.0-mbl_gamma_ser(a,x); // Use series representation
  }
  else
  {
    return mbl_gamma_cf(a,x); // Use continued fraction representation
  }
}

double mbl_log_gamma_q(double a, double x)
{
  if ((x < 0.0) || (a <= 0.0))
  {
    vcl_cerr<<"mbl_log_gamma_q : Invalid arguments."<<vcl_endl;
    vcl_abort();
  }

  if (x < (a+1.0))
  {
    return log(1.0 - mbl_gamma_ser(a,x)); // Use series representation
  }
  else
  {
    return NR_log_gamma_cf(a,x); // Use continued fraction representation
  }
}
