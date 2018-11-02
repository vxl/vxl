#ifndef vil_find_4con_boundary_h_
#define vil_find_4con_boundary_h_

#include <vector>
#include <vil/vil_image_view.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//:
// \file
// \brief Function to find 4-connected boundary around thresholded region
// \author Tim Cootes

//: Move (i,j) to next point below threshold
//  Start looking in direction dir (0=++x,1=++y,2=--x,3=--y)
//  *p is current point (i,j).
//  On exit (i,j) and p are updated to move to neighbour
template<class T>
inline void vil_next_point_below_thresh4(int& i,int& j,int& dir, const T* &p,
                                         int ni1, int nj1,
                                         std::ptrdiff_t istep,std::ptrdiff_t jstep,
                                         T threshold)
{
  for (int k=0;k<4;++k)
  {
    switch ((dir+k)%4)
    {
      case 0:   // Try at (i+1,j)
        if (i<ni1 && p[istep]<=threshold) { ++i; p+=istep; dir=3; return; }
      case 1:   // Try at (i,j+1)
        if (j<nj1 && p[jstep]<=threshold) { ++j; p+=jstep; dir=0; return; }
      case 2:   // Try at (i-1,j)
        if (i>0 && p[-istep]<=threshold)  { --i; p-=istep; dir=1; return; }
      case 3:   // Try at (i,j-1)
        if (j>0 && p[-jstep]<=threshold)  { --j; p-=jstep; dir=2; return; }
      default:
        break;
    }
  }
}

//: Move (i,j) to next point above threshold
//  Start looking in direction dir (0=++x,1=++y,2=--x,3=--y)
//  *p is current point (i,j).
//  On exit (i,j) and p are updated to move to neighbour
template<class T>
inline void vil_next_point_above_thresh4(int& i,int& j,int& dir, const T* &p,
                                         int ni1, int nj1, std::ptrdiff_t istep, std::ptrdiff_t jstep,
                                         T threshold)
{
  for (int k=0;k<4;++k)
  {
    switch ((dir+k)%4)
    {
      case (0):   // Try at (i+1,j)
        if (i<ni1 && p[istep]>=threshold) { ++i; p+=istep; dir=3; return; }
      case (1):   // Try at (i,j+1)
        if (j<nj1 && p[jstep]>=threshold) { ++j; p+=jstep; dir=0; return; }
      case (2):   // Try at (i-1,j)
        if (i>0 && p[-istep]>=threshold)  { --i; p-=istep; dir=1; return; }
      case (3):   // Try at (i,j-1)
        if (j>0 && p[-jstep]>=threshold)  { --j; p-=jstep; dir=2; return; }
      default:
        break;
    }
  }
}


//: Find 4-connected boundary around thresholded region containing point
//  Assumes that (p0_i,p0_j) is a point in the image which satisfies
//  the threshold (ie image(p0_i,p0_j)<=threshold).
//  Searches for the boundary pixels (ie points which satisfy threshold
//  next to ones which don't) and runs around until it gets back to beginning.
//  On exit the boundary points are given by (bi[k],bj[k])
template <class T>
inline void vil_find_4con_boundary_below_threshold(std::vector<int>& bi,
                                                   std::vector<int>& bj,
                                                   const vil_image_view<T>& image,
                                                   const T& threshold,
                                                   int p0_i, int p0_j)
{
  bi.resize(0); bj.resize(0);
  int ni1 = image.ni()-1;
  int nj1 = image.nj()-1;
  std::ptrdiff_t istep = image.istep(), jstep=image.jstep();

  int i = p0_i, j = p0_j;
  const T* p = &image(i,j);
  assert(*p<=threshold);

  // Move to extremal point on boundary
  while (i<ni1 && p[istep]<=threshold) {i++;p+=istep;}
  int dir = 1;

  if (i==p0_i)
  {
    // Initial point already on boundary - move to extreme j
    while (j<nj1 && p[jstep]<=threshold) {j++;p+=jstep;}
    dir = 2;
  }

  int i0 = i, j0=j;

  do
  {
    bi.push_back(i); bj.push_back(j);
    vil_next_point_below_thresh4(i,j,dir,p,ni1,nj1,istep,jstep,threshold);
  }
  while (i!=i0 || j!=j0);
}

//: Find 4-connected boundary around thresholded region containing point
//  Assumes that (p0_i,p0_j) is a point in the image which satisfies
//  the threshold (ie image(p0_i,p0_j)>=threshold).
//  Searches for the boundary pixels (ie points which satisfy threshold
//  next to ones which don't) and runs around until it gets back to beginning.
//  On exit the boundary points are given by (bi[k],bj[k])
template <class T>
inline void vil_find_4con_boundary_above_threshold(std::vector<int>& bi,
                                                   std::vector<int>& bj,
                                                   const vil_image_view<T>& image,
                                                   const T& threshold,
                                                   int p0_i, int p0_j)
{
  bi.resize(0); bj.resize(0);
  int ni1 = image.ni()-1;
  int nj1 = image.nj()-1;
  std::ptrdiff_t istep = image.istep(), jstep=image.jstep();

  int i = p0_i, j = p0_j;
  const T* p = &image(i,j);
  assert(*p>=threshold);

  // Move to extremal point on boundary
  while (i<ni1 && p[istep]>=threshold) {i++;p+=istep;}
  int dir = 1;

  if (i==p0_i)
  {
    // Initial point already on boundary - move to extreme j
    while (j<nj1 && p[jstep]>=threshold) {j++;p+=jstep;}
    dir = 2;
  }

  int i0 = i, j0=j;

  do
  {
    bi.push_back(i); bj.push_back(j);
    vil_next_point_above_thresh4(i,j,dir,p,ni1,nj1,istep,jstep,threshold);
  }
  while (i!=i0 || j!=j0);
}

#endif // vil_find_4con_boundary_h_
