//:
// \file
// \brief Generate points and curves to display a grid
// \author Tim Cootes

#include <msm/utils/msm_make_grid.h>

static msm_curve msm_make_curve(unsigned i0, unsigned di, unsigned n)
{
  msm_curve curve;
  curve.set_open(true);
  curve.index().resize(n);
  for (unsigned i=0;i<n;++i) curve.index()[i]=i0+i*di;
  return curve;
}

//: Generate points and curves to display a grid
//  Grid contains nx x ny squares, each of size dx x dy.
//  Creates (nx+1)*(ny+1) points and 2+nx+ny curves (describing the horizontal
//  and vertical lines)
void msm_make_grid(msm_points& points, msm_curves& curves,
                   unsigned nx, double dx, unsigned ny, double dy)
{
  // Set up points
  std::vector<vgl_point_2d<double> > pts;
  for (unsigned j=0; j<= ny; ++j)
    for (unsigned i=0; i<= nx; ++i)
      pts.emplace_back(i*dx,j*dy);

  points.set_points(pts);

  curves.resize(0);

  // Set up vertical lines
  for (unsigned i=0;i<=nx;++i)
    curves.push_back(msm_make_curve(i,ny+1,ny+1));

  // Set up horizontal lines
  for (unsigned j=0;j<=ny;++j)
    curves.push_back(msm_make_curve(j*(nx+1),1,nx+1));
}

//: Generate points and curves to display a grid
//  Grid contains nx x ny squares, each of size dx x dy.
//  Draw in n_per_seg points along each line
//  Creates (nx+1)*(ny+1)*n_per_seg^2 points and 2+nx+ny curves
//  (describing the horizontal and vertical lines)
void msm_make_grid(msm_points& points, msm_curves& curves,
                   unsigned nx, double dx, unsigned ny, double dy, unsigned n_per_seg)
{
  unsigned ns = n_per_seg;
  if (ns<1) ns=1;

  // Set up points
  std::vector<vgl_point_2d<double> > pts;
  for (unsigned j=0; j<= ny*ns; ++j)
    for (unsigned i=0; i<= nx*ns; ++i)
      pts.emplace_back(i*dx/ns,j*dy/ns);
  points.set_points(pts);

  curves.resize(0);

  unsigned nx1 = 1+ns*nx;  // Number of points along x
  unsigned ny1 = 1+ns*ny;  // Number of points along y

  // Set up vertical lines
  for (unsigned i=0;i<=nx;++i)
    curves.push_back(msm_make_curve(i*ns,ny1,ny1));

  // Set up horizontal lines
  for (unsigned j=0;j<=ny;++j)
    curves.push_back(msm_make_curve(j*ns*nx1,1,nx1));
}
