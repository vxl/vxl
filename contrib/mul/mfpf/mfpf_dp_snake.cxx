//:
// \file
// \author Tim Cootes
// \brief Basic snake, using dynamic programming to update.

#include <iostream>
#include <cmath>
#include "mfpf_dp_snake.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>
#include <vsl/vsl_vector_io.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>

#include <mbl/mbl_dyn_prog.h>
#include <mbl/mbl_stats_1d.h>

//=======================================================================
// Dflt ctor
//=======================================================================

mfpf_dp_snake::mfpf_dp_snake()
  : max_its_(10)
{
}


//=======================================================================
// Destructor
//=======================================================================

mfpf_dp_snake::~mfpf_dp_snake() = default;

//: Finder used to search for good points along profiles
mfpf_point_finder& mfpf_dp_snake::finder()
{
  return finder_;
}

//: Initialise as a circle of given radius about the given centre
//  Clone taken of finder object
void mfpf_dp_snake::set_to_circle(const mfpf_point_finder& finder,
                                  unsigned n_points,
                                  const vgl_point_2d<double>& c,
                                  double r)
{
  finder_=finder;

  pts_.resize(n_points);
  for (unsigned iA=0;iA<n_points;++iA)
  {
    double A = iA*6.283/n_points;
    double dx=std::cos(A),dy=std::sin(A);
    pts_[iA] = vgl_point_2d<double>(c.x()+r*dx,c.y()+r*dy);
  }
}

//: Replace each point with the average of it and its neighbours
void mfpf_dp_snake::smooth_curve(
                    std::vector<vgl_point_2d<double> >& src_pts,
                    std::vector<vgl_point_2d<double> >& dest_pts)
{
  unsigned n = src_pts.size();
  dest_pts.resize(n);
  for (unsigned i=0;i<n;++i)
    dest_pts[i] = centre(src_pts[(i+n-1)%n],src_pts[i],src_pts[(i+1)%n]);
}


//: Perform one iteration of snake search algorithm
//  Return the mean movement of each point
double mfpf_dp_snake::update_step(const vimt_image_2d_of<float>& image)
{
  unsigned n = pts_.size();

  unsigned s_ni = finder().search_ni();
  unsigned nr = 1+2*s_ni;

  std::vector<vgl_vector_2d<double> > u(n); // Space for normals

  // Create reference shape,
  // which will be slightly smoothed version of pts_
  std::vector<vgl_point_2d<double> > ref_pts(n);

  // Space to hold response data
  vnl_matrix<double> D(n,nr);

  int dt = 3;  // Displacement index used to compute normals

  vimt_image_2d_of<double> response;

  for (unsigned i=0;i<n;++i)
  {
    // Tangent vector
    vgl_vector_2d<double> t = pts_[(i+n-dt)%n]-pts_[(i+dt)%n];
    t/=(t.length()+1e-6);
    double dx=-t.y(),dy=t.x();  // Unit normal
    u[i]=vgl_vector_2d<double>(dx,dy);

    // Search around the average of the neighbours of pt i
    ref_pts[i] = centre(pts_[(i+n-1)%n],pts_[(i+1)%n]);

    finder().evaluate_region(image,ref_pts[i],u[i],response);

    for (unsigned j=0;j<nr;++j)
    {
      double d= response.image()(j,0);
      D(i,j)=d;
    }
  }

  mbl_dyn_prog dp;
  vnl_vector<double> move_cost(nr);
  double shape_k=1.0;
  for (unsigned int i=0;i<nr;++i)
  {
    move_cost[i]=shape_k*i;
  }

  std::vector<int> x;
  dp.solve_loop(x,D,move_cost);

  // Compute new image positions
  double step_size = finder().step_size();
  double move_sum=0.0;

  for (unsigned i=0;i<n;++i)
  {
    // If x[i]==s_ni, then no movement (central position)
    vgl_point_2d<double> new_pt = ref_pts[i] + (x[i]-int(s_ni))*step_size*u[i];

    move_sum += (new_pt-pts_[i]).length();
    pts_[i] = new_pt;
  }

  return move_sum/n;
}

//: Search image (running iterations until convergence)
void mfpf_dp_snake::search(const vimt_image_2d_of<float>& image)
{
  // Just run fixed number of iterations
  for (unsigned i=0;i<max_its_;++i)
    update_step(image);
}


//: Replace each point with the average of it and its neighbours
void mfpf_dp_snake::smooth_curve()
{
  std::vector<vgl_point_2d<double> > old_pts = pts_;
  smooth_curve(old_pts,pts_);
}

//: Centre of gravity of points
vgl_point_2d<double> mfpf_dp_snake::cog() const
{
  unsigned n = pts_.size();
  if (n==0) return {0,0};

  double xsum=0,ysum=0;
  for (unsigned i=0;i<n;++i)
  {
    xsum+=pts_[i].x();
    ysum+=pts_[i].y();
  }
  return {xsum/n,ysum/n};
}

//: Mean distance of points to cog()
double mfpf_dp_snake::mean_radius() const
{
  unsigned n = pts_.size();
  if (n==0) return 0.0;

  vgl_point_2d<double> c = cog();
  double r_sum=0.0;
  for (unsigned i=0;i<n;++i)
    r_sum += (pts_[i]-c).length();
  return r_sum/n;
}

//: Compute mean and sd of distance to cog()
void mfpf_dp_snake::radius_stats(double& mean, double& sd) const
{
  mbl_stats_1d stats;
  unsigned n = pts_.size();

  vgl_point_2d<double> c = cog();
  for (unsigned i=0;i<n;++i)
    stats.obs((pts_[i]-c).length());

  mean=stats.mean();
  sd=stats.sd();
}


//=======================================================================
// Method: version_no
//=======================================================================

short mfpf_dp_snake::version_no() const
{
  return 1;
}


//=======================================================================
// Method: is_a
//=======================================================================

std::string mfpf_dp_snake::is_a() const
{
  return std::string("mfpf_dp_snake");
}

//: Print class to os
void mfpf_dp_snake::print_summary(std::ostream& os) const
{
  vsl_indent_inc(os);

  os<<vsl_indent()<<"n_points: "<<pts_.size()
    <<vsl_indent()<<"finder: "<<finder_<<'\n';
  vgl_point_2d<double> c = cog();
  os<<vsl_indent()<<" CoG: ("<<c.x()<<','<<c.y();

  double r_mean,r_sd;
  radius_stats(r_mean,r_sd);
  os<<") mean radius: "<<r_mean<<" SD: "<<r_sd<<'\n';
  vsl_indent_dec(os);
}

//=======================================================================
// Method: save
//=======================================================================

void mfpf_dp_snake::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,max_its_);
  vsl_b_write(bfs,finder_);
  vsl_b_write(bfs,pts_);
}

//=======================================================================
// Method: load
//=======================================================================

void mfpf_dp_snake::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,max_its_);
      vsl_b_read(bfs,finder_);
      vsl_b_read(bfs,pts_);
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << std::endl;
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

std::ostream& operator<<(std::ostream& os,const mfpf_dp_snake& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
  return os;
}

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mfpf_dp_snake& b)
{
  b.b_write(bfs);
}

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mfpf_dp_snake& b)
{
  b.b_read(bfs);
}
