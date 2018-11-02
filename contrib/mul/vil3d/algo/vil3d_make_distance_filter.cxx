//:
// \file
//  \brief Compute filter for an exp. distance transform.
// \author Tim Cootes

#include <iostream>
#include <cmath>
#include "vil3d_make_distance_filter.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

//: Create filter such that d[a] is distance from origin of se[a]
//  Pixel widths are (width_i,width_j,width_k).  Elements are
//  selected so that only voxels visited before in a raster scan
//  of the image are included.  Distance transforms require two
//  passes, a forward and reverse pass.
//
//  Consider only voxels within 1 voxel (1 norm) of origin.
//  Indexes (27-1)/2 = 13 voxels.
void vil3d_make_distance_filter_r1(
                           double width_i,
                           double width_j,
                           double width_k,
                           vil3d_structuring_element& se,
                           std::vector<double>& d)
{
  std::vector<int> pi,pj,pk;
  d.resize(0);
  double wi2=width_i*width_i;
  double wj2=width_j*width_j;
  double wk2=width_k*width_k;

  for (int k=-1;k<=0;++k)
  {
    int jhi=0;
    if (k<0) jhi=1;
    for (int j=-1;j<=jhi;++j)
    {
      int ihi=0;
      if (j<0 || k<0) ihi=1;
      for (int i=-1;i<=ihi;++i)
        if (!(i==0 && j==0 && k==0))
        {
          pi.push_back(i);
          pj.push_back(j);
          pk.push_back(k);
          d.push_back(std::sqrt(double(i*i*wi2+j*j*wj2+k*k*wk2)));
        }
    }
  }
  se.set(pi,pj,pk);
}

static void vil3d_make_voxel_block(int r,
                              std::vector<int>& pi,
                              std::vector<int>& pj,
                              std::vector<int>& pk,
                              std::vector<double>& d)
{
  for (int k=-r;k<=0;++k)
  {
    int jhi=0;
    if (k<0) jhi=r;
    for (int j=-r;j<=jhi;++j)
    {
      int ihi=0;
      if (j<0 || k<0) ihi=r;
      for (int i=-r;i<=ihi;++i)
      {
        pi.push_back(i);
        pj.push_back(j);
        pk.push_back(k);
        d.push_back(std::sqrt(double(i*i+j*j+k*k)));
      }
    }
  }
}

// Return true if offset(i,j,k) cannot be accurately approximated
// by combinations of two (pi,pj,pk)
static bool vil3d_offset_is_prime(int i,int j,int k,
                               const std::vector<int>& pi,
                               const std::vector<int>& pj,
                               const std::vector<int>& pk,
                               const std::vector<double>& d)
{
  double d_true = std::sqrt(double(i*i+j*j+k*k));
  for (unsigned a =0;a<pi.size();++a)
  {
    for (unsigned b=0;b<pi.size();++b)
    {
      if ((pi[a]+pi[b]==i) &&
          (pj[a]+pj[b]==j) &&
          (pk[a]+pk[b]==k) )
      {
        // (i,j,k) is reached by element a + element b
        // Check if distance sum is a good approximation
        if (std::fabs(d[a]+d[b]-d_true)<1e-5) return false;
      }
    }
  }
  return true;
}

//: Create filter such that d[a] is distance from origin of se[a]
//  Pixel widths are (width_i,width_j,width_k).  Elements are
//  selected so that only voxels visited before in a raster scan
//  of the image are included.  Distance transforms require two
//  passes, a forward and reverse pass.
//
//  Consider only voxels within r voxels (1 norm) of origin.
void vil3d_make_distance_filter(
                           double width_i,
                           double width_j,
                           double width_k,
                           int r,
                           vil3d_structuring_element& se,
                           std::vector<double>& d)
{
  assert(r>0);
  if (r==1)
  {
    vil3d_make_distance_filter_r1(width_i,width_j,width_k,se,d);
    return;
  }

  // Compute 'prime' elements, ie offsets which cannot be
  // constructed accurately as a sum of two smaller offsets
  vil3d_structuring_element se1;
  std::vector<double> d1;
  vil3d_make_distance_filter_r1(1,1,1,se1,d1);
  std::vector<int> pi=se1.p_i();
  std::vector<int> pj=se1.p_j();
  std::vector<int> pk=se1.p_k();

  for (int r1=2;r1<=r;++r1)
  {
    // Process shell of voxels at inf norm distance r1 from origin

    // Create block of all offsets within (r-1),
    // used to check if new offset can be constructed from old ones.
    std::vector<int> pir,pjr,pkr;
    std::vector<double> dr;
    vil3d_make_voxel_block(r1-1,pir,pjr,pkr,dr);

    for (int k=-r1;k<=0;++k)
    {
      int jhi=0;
      if (k<0) jhi=r1;
      for (int j=-r1;j<=jhi;++j)
      {
        int ihi=0;
        if (j<0 || k<0) ihi=r1;
        for (int i=-r1;i<=ihi;++i)
        if (i==-r1 || i==r1 || j==-r1 || j==r1 || k==-r1)
        {
          if (vil3d_offset_is_prime(i,j,k,pir,pjr,pkr,dr))
          {
            pi.push_back(i);
            pj.push_back(j);
            pk.push_back(k);
          }
        }
      }
    }
  }

  d.resize(pi.size());
  for (unsigned a=0;a<pi.size();++a)
  {
    double dx=width_i*pi[a];
    double dy=width_j*pj[a];
    double dz=width_k*pk[a];
    d[a]=std::sqrt(dx*dx+dy*dy+dz*dz);
  }
  se.set(pi,pj,pk);
}
