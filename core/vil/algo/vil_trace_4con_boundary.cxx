//:
// \file
// \brief Function to trace 4-connected boundary around region in bool image
// \author Tim Cootes

#include "vil_trace_4con_boundary.h"

//: Move (i,j) to next boundary point
//  Start looking in direction dir (0=++x,1=++y,2=--x,3=--y)
//  *p is current point (i,j).
//  On exit (i,j) and p are updated to move to neighbour
inline void vil_next_4con_boundary_point(int& i,int& j,int& dir, const bool* &p,
                                         int ni1, int nj1,int istep,int jstep)
{
  for (int k=0;k<4;++k)
  {
    switch ((dir+k)%4)
    {
      case (0):   // Try at (i+1,j)
        if (i<ni1 && p[istep]) { ++i; p+=istep; dir=3; return; }
      case (1):   // Try at (i,j+1)
        if (j<nj1 && p[jstep]) { ++j; p+=jstep; dir=0; return; }
      case (2):   // Try at (i-1,j)
        if (i>0 && p[-istep])  { --i; p-=istep; dir=1; return; }
      case (3):   // Try at (i,j-1)
        if (j>0 && p[-jstep])  { --j; p-=jstep; dir=2; return; }
    }
  }
}

static inline int vil_first_direction(unsigned int i, unsigned int j, const vil_image_view<bool>& image)
{
  if (i>=image.ni() || j>=image.nj() || !image(i,j)) return -1;

  // Find first neighbour outside
  if (i+1>=image.ni() || !image(i+1,j)) return 0;
  if (j+1>=image.nj() || !image(i,j+1)) return 1;
  if (i==0 || !image(i-1,j)) return 2;
  if (j==0 || !image(i,j-1)) return 3;

  return -1; // No neighbours are outside
}


//: Trace 4-connected boundary around region in boolean image
//  Assumes that (i0,j0) is a boundary point.
//  Searches for the boundary pixels and runs around until it gets back to beginning.
//  On exit the boundary points are given by (bi[k],bj[k])
void vil_trace_4con_boundary(vcl_vector<int>& bi, vcl_vector<int>& bj,
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
    vil_next_4con_boundary_point(i,j,dir,p,ni1,nj1,istep,jstep);
  }
  while (i!=i0 || j!=j0);
}
