//:
// \file
#include <vcl_cmath.h>
#include <vcl_cstdlib.h> // for std::abs(int)
#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_math.h> // for pi
#include <vdgl/vdgl_digital_region.h>
#include <bdgl/bdgl_region_algs.h>


//: Destructor
bdgl_region_algs::~bdgl_region_algs()
{
}

//:
//--------------------------------------------------------------------------
// Finds the Mahanabolis distance between the intensity distributions
// of two regions.  If there are not enough pixels in either or both 
// regions to reliably determine the distance then max float is returned.
//--------------------------------------------------------------------------
float bdgl_region_algs::
mahanabolis_distance(vdgl_digital_region_sptr const& r1,
                     vdgl_digital_region_sptr const& r2)
{
  //need at this this many points for standard deviation and mean
  //to be valid
  int min_npts = 5;
  float MY_HUGE = vnl_numeric_traits<float>::maxval;
  float SMALL = 1;
  if(!r1||!r2)
    return MY_HUGE;
  if(r1->Npix()<min_npts||r2->Npix()<min_npts)
    return MY_HUGE;
  float m1 = r1->Io(), m2 = r2->Io();
  float s1 = r1->Io_sd(), s2 = r2->Io_sd();
  //make sure the standard deviations are well-behaved  
  if(s1<SMALL)
    s1 = SMALL;
  if(s2<SMALL)
    s2 = SMALL;
  float s_sq = (s1*s1*s2*s2)/(s1*s1 + s2*s2);
  float d = vcl_sqrt((m1-m2)*(m1-m2)/s_sq);
  return d;
}
bool bdgl_region_algs::merge(vdgl_digital_region_sptr const& r1,
                             vdgl_digital_region_sptr const& r2, 
                             vdgl_digital_region_sptr& rm)
{
  if(!r1||!r2)
    return false;
  //trivial cases
  int n1 = r1->Npix(), n2 = r2->Npix();
  if(n1==0)
    return r2;
  if(n2==0)
    return r1;
  int n = n1 + n2;
  if(!n)
    return false;
  float* Xm = new float[n];
  float* Ym = new float[n];
  unsigned short* Im = new unsigned short[n];

  float const* X1 = r1->Xj();
  float const* Y1 = r1->Yj();
  unsigned short const* I1 = r1->Ij();

  float const* X2 = r2->Xj();
  float const* Y2 = r2->Yj();
  unsigned short const* I2 = r2->Ij();

  for(int i = 0; i<n1; i++)
    {
      Xm[i] = X1[i];
      Ym[i] = Y1[i];
      Im[i] = I1[i];
    }

  for(int i = 0; i<n2; i++)
    {
      Xm[i] = X2[i];
      Ym[i] = Y2[i];
      Im[i] = I2[i];
    }
  rm = new vdgl_digital_region(n, Xm, Ym, Im);
  return true;
}                            
