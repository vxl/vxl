#ifndef mfpf_dp_snake_h_
#define mfpf_dp_snake_h_
//:
// \file
// \author Tim Cootes
// \brief Basic snake, using dynamic programming to update.

#include <iostream>
#include <iosfwd>
#include <mfpf/mfpf_point_finder.h>
#include <mbl/mbl_cloneable_ptr.h>
#include <vgl/vgl_point_2d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Basic snake, using dynamic programming to update.
// Contains a single mfpf_point_finder, which is used to locate
// all candidate points along a profile.
class mfpf_dp_snake
{
 protected:
  //: Maximum number of iterations to use during search
  unsigned max_its_;

  //: Finder used to search for good points along profiles
  mbl_cloneable_ptr<mfpf_point_finder> finder_;

  //: Current set of boundary points (a closed curve)
  std::vector<vgl_point_2d<double> > pts_;

  //: Compute the average of each point and its neighbours
  void smooth_curve(std::vector<vgl_point_2d<double> >& src_pts,
                    std::vector<vgl_point_2d<double> >& dest_pts);

 public:

  //: Dflt ctor
  mfpf_dp_snake();

  //: Destructor
  virtual ~mfpf_dp_snake();

  //: Initialise as a circle of given radius about the given centre
  //  Clone taken of finder object
  void set_to_circle(const mfpf_point_finder& finder,
                     unsigned n_points,
                     const vgl_point_2d<double>& centre,
                     double r);

  //: Number of points
  unsigned size() const { return pts_.size(); }

  //: Finder used to search for good points along profiles
  mfpf_point_finder& finder();

  //: Current set of boundary points (a closed curve)
  const std::vector<vgl_point_2d<double> >& points() const { return pts_; }

  //: Perform one iteration of snake search algorithm
  //  Return the mean movement of each point
  double update_step(const vimt_image_2d_of<float>& image);

  //: Search image (running iterations until convergence)
  void search(const vimt_image_2d_of<float>& image);

  //: Replace each point with the average of it and its neighbours
  void smooth_curve();

  //: Centre of gravity of points
  vgl_point_2d<double> cog() const;

  //: Mean distance of points to cog()
  double mean_radius() const;

  //: Compute mean and sd of distance to cog()
  void radius_stats(double& mean, double& sd) const;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual std::string is_a() const;

  //: Print class to os
  virtual void print_summary(std::ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const mfpf_dp_snake& b);

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mfpf_dp_snake& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mfpf_dp_snake& b);

#endif // mfpf_dp_snake_h_
