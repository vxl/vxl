//:
// \file
// \brief Function to trace 8-connected boundary around region in bool image
// \author Tim Cootes

#include "vil_trace_8con_boundary.h"

//: Move (i,j) to next boundary point
//  Start looking in direction dir (0=++x,1=++y,2=--x,3=--y)
//  *p is current point (i,j).
//  On exit (i,j) and p are updated to move to neighbour
inline void vil_next_8con_boundary_point(int& i, int& j, int& dir, const bool* &p,
                                         int ni1, int nj1,
                                         vcl_ptrdiff_t istep, vcl_ptrdiff_t jstep)
{
  for (int k=0;k<8;++k)
  {
    switch ((dir+k)%8)
    {
     case 0:   // Try at (i+1,j)
      if (i<ni1 && p[istep]) { ++i; p+=istep; dir=5; return; }
     case 1:   // Try at (i+1,j+1)
      if (i<ni1 && j<nj1 && p[istep+jstep]) {
        ++i; ++j; p+=(istep+jstep); dir=6; return; }
     case 2:   // Try at (i,j+1)
      if (j<nj1 && p[jstep]) { ++j; p+=jstep; dir=7; return; }
     case 3:   // Try at (i-1,j+1)
      if (i>0 && j<nj1 && p[jstep-istep]) {
        --i; ++j; p+=(jstep-istep); dir=0; return; }
     case 4:   // Try at (i-1,j)
      if (i>0 && p[-istep])  { --i; p-=istep; dir=1; return; }
     case 5:   // Try at (i-1,j-1)
      if (i>0 && j>0 && p[-jstep-istep]) {
        --i; --j; p+=(-jstep-istep); dir=2; return; }
     case 6:   // Try at (i,j-1)
      if (j>0 && p[-jstep])  { --j; p-=jstep; dir=3; return; }
     case 7:   // Try at (i+1,j-1)
      if (i<ni1 && j>0 && p[istep-jstep]) {
        ++i; --j; p+=(istep-jstep); dir=4; return; }
    }
  }
}

static inline int vil_first_direction(unsigned int i, unsigned int j, const vil_image_view<bool>& image)
{
  if (i>=image.ni() || j>=image.nj() || !image(i,j)) return -1;

  // Find first neighbour outside

  // Check for border pixels
  if (i+1>=image.ni()) return 0;
  if (j+1>=image.nj()) return 2;
  if (i==0) return 4;
  if (j==0) return 6;

  if (!image(i+1,j))   return 0;
  if (!image(i+1,j+1)) return 1;
  if (!image(i,j+1))   return 2;
  if (!image(i-1,j+1)) return 3;
  if (!image(i-1,j))   return 4;
  if (!image(i-1,j-1)) return 5;
  if (!image(i,j-1))   return 6;
  if (!image(i+1,j-1)) return 7;

  return -1; // No neighbours are outside
}


//: Trace 8-connected boundary around region in boolean image
//  Assumes that (i0,j0) is a boundary point.
//  Searches for the boundary pixels and runs around until it gets back to beginning.
//  On exit the boundary points are given by (bi[k],bj[k])
void vil_trace_8con_boundary(vcl_vector<int>& bi, vcl_vector<int>& bj,
                             const vil_image_view<bool>& image,
                             int i0, int j0)
{
  bi.resize(0); bj.resize(0);
  unsigned int ni1 = image.ni()-1;
  unsigned int nj1 = image.nj()-1;
  vcl_ptrdiff_t istep = image.istep(), jstep=image.jstep();

  int i = i0, j = j0;
  const bool* p = &image(i,j);

  // Check that p is a boundary pixel
  int dir = vil_first_direction(i,j,image);

  if (dir<0) return;  // Not a boundary point!

  do
  {
    bi.push_back(i); bj.push_back(j);
    vil_next_8con_boundary_point(i,j,dir,p,ni1,nj1,istep,jstep);
  }
  while (i!=i0 || j!=j0);

  if (bi.size()==1) return;  // Isolated pixel (how sad).

  // Got back to start.
  // However, if start is part of a 1 pixel wide line, we need to 
  // investigate the other side of the line
  // To check for this, find the next boundary point and check that it
  // is the same as was found during the first pass
  // This can happen up to four times (I think) in worst case
  vil_next_8con_boundary_point(i,j,dir,p,ni1,nj1,istep,jstep);
  while (i!=bi[1] || j!=bj[1])
  {
    // Second pass is different from first
    // Investigate the other side of the blob
    bi.push_back(i0); bj.push_back(j0);
    do
    {
      bi.push_back(i); bj.push_back(j);
      vil_next_8con_boundary_point(i,j,dir,p,ni1,nj1,istep,jstep);
    }
    while (i!=i0 || j!=j0);

    vil_next_8con_boundary_point(i,j,dir,p,ni1,nj1,istep,jstep);
  }
}
