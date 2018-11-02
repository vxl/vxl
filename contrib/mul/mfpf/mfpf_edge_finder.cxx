#include <iostream>
#include <cmath>
#include "mfpf_edge_finder.h"
//:
// \file
// \brief Locates strongest edge along a profile
// \author Tim Cootes

#include <vsl/vsl_binary_loader.h>
#include <vnl/vnl_vector.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vimt/vimt_bilin_interp.h>
#include <vimt/vimt_sample_profile_bilin.h>

//=======================================================================
// Dflt ctor
//=======================================================================

mfpf_edge_finder::mfpf_edge_finder() = default;

//=======================================================================
// Destructor
//=======================================================================

mfpf_edge_finder::~mfpf_edge_finder() = default;

//: Radius of circle containing modelled region
double mfpf_edge_finder::radius() const
{
  return 1.0;
}

//: Generate points in ref frame that represent boundary
//  Points of a closed contour around the shape.
//  Used for display purposes.
void mfpf_edge_finder::get_outline(std::vector<vgl_point_2d<double> >& pts) const
{
  pts.resize(2);
  pts[0]=vgl_point_2d<double>(-0.5,0);
  pts[1]=vgl_point_2d<double>( 0.5,0);
}


//: Evaluate match at p, using u to define scale and orientation
// Returns -1*edge strength at p along direction u
double mfpf_edge_finder::evaluate(const vimt_image_2d_of<float>& image,
                                  const vgl_point_2d<double>& p,
                                  const vgl_vector_2d<double>& u)
{
  double v1 = vimt_bilin_interp_safe(image,p+0.5*step_size_*u);
  double v2 = vimt_bilin_interp_safe(image,p-0.5*step_size_*u);
  return -1.0*std::fabs(v1-v2);
}

   //: Evaluate match at in a region around p
   // Returns a quality of fit at a set of positions.
   // response image (whose size and transform is set inside the
   // function), indicates the points at which the function was
   // evaluated.  response(i,j) is the fit at the point
// response.world2im().inverse()(i,j).  The world2im() transformation
// may be affine.
void mfpf_edge_finder::evaluate_region(
                        const vimt_image_2d_of<float>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u,
                        vimt_image_2d_of<double>& response)
{
  int n=1+2*search_ni_;
  vnl_vector<double> v(n+1);
  vgl_vector_2d<double> u1=step_size_*u;
  const vgl_point_2d<double> p0 = p-(search_ni_+0.5)*u1;
  vimt_sample_profile_bilin(v,image,p0,u1,n+1);
  response.image().set_size(n,1);
  double* r = response.image().top_left_ptr();
  for (int i=0;i<n;++i,++r)
  {
    *r = -1*std::fabs(v[i+1]-v[i]);
  }

  // Set up transformation parameters

  // Point (i,j) in dest corresponds to p1+i.u+j.v,
  // an affine transformation for image to world
  const vgl_point_2d<double> p1 = p-search_ni_*u1;

  vimt_transform_2d i2w;
  i2w.set_similarity(vgl_point_2d<double>(u1.x(),u1.y()),p1);
  response.set_world2im(i2w.inverse());
}

   //: Search given image around p, using u to define scale and orientation
   //  On exit, new_p and new_u define position, scale and orientation of
   //  the best nearby match.  Returns a quality of fit measure at that
   //  point (the smaller the better).
double mfpf_edge_finder::search_one_pose(
                                const vimt_image_2d_of<float>& image,
                                const vgl_point_2d<double>& p,
                                const vgl_vector_2d<double>& u,
                                vgl_point_2d<double>& new_p)
{
  int n=1+2*search_ni_;
  vnl_vector<double> v(n+1);
  vgl_vector_2d<double> u1=step_size_*u;
  const vgl_point_2d<double> p0 = p-(search_ni_+0.5)*u1;
  vimt_sample_profile_bilin(v,image,p0,u1,n+1);
  int best_i=0;
  double best_e = std::fabs(v[1]-v[0]);
  for (int i=1;i<n;++i)
  {
    double e = std::fabs(v[i+1]-v[i]);
    if (e>best_e) { best_e=e; best_i=i; }
  }
  new_p = p+(best_i-search_ni_)*u1;
  return -1.0 * best_e;
}

//=======================================================================
// Method: is_a
//=======================================================================

std::string mfpf_edge_finder::is_a() const
{
  return std::string("mfpf_edge_finder");
}

//: Create a copy on the heap and return base class pointer
mfpf_point_finder* mfpf_edge_finder::clone() const
{
  return new mfpf_edge_finder(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void mfpf_edge_finder::print_summary(std::ostream& os) const
{
  os<<"{ ";
  mfpf_point_finder::print_summary(os);
  os<<" }";
}

short mfpf_edge_finder::version_no() const
{
  return 1;
}


void mfpf_edge_finder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  mfpf_point_finder::b_write(bfs);  // Save baseclass
}

//=======================================================================
// Method: load
//=======================================================================

void mfpf_edge_finder::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case 1:
      mfpf_point_finder::b_read(bfs);  // Load in baseclass
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << std::endl;
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}
