//:
// \file
#include <vcl_cmath.h>
#include <vcl_cstdlib.h>
#include <vdgl/vdgl_digital_region.h>
#include <bdgl/bdgl_region_algs.h>

//:
//--------------------------------------------------------------------------
// Finds the Mahalanobis distance between the intensity distributions
// of two regions.  If there are not enough pixels in either or both
// regions to reliably determine the distance then -1 is returned.
//--------------------------------------------------------------------------
float bdgl_region_algs::
mahalanobis_distance(vdgl_digital_region_sptr const& r1,
                     vdgl_digital_region_sptr const& r2)
{
  //need this many points for standard deviation and mean to be valid
  const unsigned int min_npts = 5;
  const float SMALL = 1;
  if (!r1 || !r2)
    return -1.f;
  if (r1->Npix()<min_npts || r2->Npix()<min_npts)
    return -1.f;
  float m1 = r1->Io(), m2 = r2->Io();
  float s1 = r1->Io_sd(), s2 = r2->Io_sd();
  //make sure the standard deviations are well-behaved
  if (s1<SMALL) s1 = SMALL;
  if (s2<SMALL) s2 = SMALL;
  float s_sq = (s1*s1*s2*s2)/(s1*s1 + s2*s2);
  float d = vcl_sqrt((m1-m2)*(m1-m2)/s_sq);
  vcl_cout << "MDistance||(" << r1->Npix()
           << ")(Xo:" << r1->Xo() << " Yo:" << r1 ->Yo()
           << " Io:" << r1 ->Io() << ")::(" << r2->Npix()
           << " Xo:" << r2->Xo() << " Yo:" << r2 ->Yo()
           << " Io:" << r2 ->Io() <<")||= " << d << vcl_endl;
  return d;
}

float bdgl_region_algs::intensity_distance(vdgl_digital_region_sptr const& r1,
                                           vdgl_digital_region_sptr const& r2)
{
  const unsigned int min_npts = 5;
  const float SMALL = 1;
  if (!r1 || !r2)
    return -1.f;
  if (r1->Npix()<min_npts || r2->Npix()<min_npts)
    return -1.f;
  float m1 = r1->Io(), m2 = r2->Io();
  if (vcl_fabs(m1-m2)<SMALL)
    return 0;
  float msq = (m1+m2)*(m1+m2);
  double d = 2.0*vcl_sqrt((m1-m2)*(m1-m2)/msq);
  vcl_cout << "Intensity Distance||(" << r1->Npix()
           << ")(Xo:" << r1->Xo() << " Yo:" << r1 ->Yo()
           << " Io:" << r1 ->Io() << ")::(" << r2->Npix()
           << " Xo:" << r2->Xo() << " Yo:" << r2 ->Yo()
           << " Io:" << r2 ->Io() <<")||= " << d << vcl_endl;
  return float(d);
}

bool bdgl_region_algs::merge(vdgl_digital_region_sptr const& r1,
                             vdgl_digital_region_sptr const& r2,
                             vdgl_digital_region_sptr& rm)
{
  if (!r1 || !r2)
    return false;
  //trivial cases
  int n1 = r1->Npix(), n2 = r2->Npix();
  if (n1==0)
    return r2;
  if (n2==0)
    return r1;
  int n = n1 + n2;
  if (!n)
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

  for (int i = 0; i<n1; i++)
  {
    Xm[i] = X1[i];
    Ym[i] = Y1[i];
    Im[i] = I1[i];
  }
  int j = n1;
  for (int i = 0; i<n2; i++,j++)
  {
    Xm[j] = X2[i];
    Ym[j] = Y2[i];
    Im[j] = I2[i];
  }
  rm = new vdgl_digital_region(n, Xm, Ym, Im);
  delete[] Xm;
  delete[] Ym;
  delete[] Im;
  return true;
}

static int increasing_compare(const void *x1, const void *x2)
{
  const unsigned short* f1 = (const unsigned short*)x1;
  const unsigned short* f2 = (const unsigned short*)x2;
  if (*f1<*f2)
    return -1;
  else if (*f1==*f2)
    return 0;
  else
    return 1;
}

static int decreasing_compare(const void *x1, const void *x2)
{
  const unsigned short* f1 = (const unsigned short*)x1;
  const unsigned short* f2 = (const unsigned short*)x2;
  if (*f1>*f2)
    return -1;
  else if (*f1==*f2)
    return 0;
  else
    return 1;
}

//: Computes the maximum average distance between the intensity samples in two regions.
float
bdgl_region_algs::earth_mover_distance(vdgl_digital_region_sptr const& r1,
                                       vdgl_digital_region_sptr const& r2)
{
  unsigned int min_npts = 5;
  if (!r1 || !r2)
    return -1.f;
  unsigned int n1 = r1->Npix(), n2 = r2->Npix();
  if (n1<min_npts || n2<min_npts)
    return -1.f;
  unsigned short I1[n1], I2[n2];
  vcl_memcpy(I1, r1->Ij(), n1*sizeof(unsigned short));
  vcl_memcpy(I2, r2->Ij(), n2*sizeof(unsigned short));
  //Sort the intensities in each region
  vcl_qsort( (void*)I1, n1, sizeof(unsigned short), increasing_compare );
  vcl_qsort( (void*)I2, n2, sizeof(unsigned short), decreasing_compare );
  //Match up the smallest intensities in the smaller region with
  //the largest intensities in the larger region.  This provides a
  //measure of the distance between the two regions

  float sum = 0;
  unsigned int n_smaller = n1; if (n2<n_smaller) n_smaller=n2;
  for (unsigned int i = 0; i<n_smaller; ++i)
  {
    float d = I1[i] - I2[i];
    sum += vcl_sqrt(d*d);
  }
  sum /= n_smaller;
#ifdef DEBUG
  vcl_cout << "EarthMover Max Distance||(" << r1->Npix()
           << ")(Xo:" << r1->Xo() << " Yo:" << r1 ->Yo()
           << " Io:" << r1 ->Io() << ")::(" << r2->Npix()
           << " Xo:" << r2->Xo() << " Yo:" << r2 ->Yo()
           << " Io:" << r2 ->Io() <<")||= " << sum << vcl_endl;
#endif
  return sum;
}
