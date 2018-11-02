#ifndef mfpf_norm_corr2d_h_
#define mfpf_norm_corr2d_h_
//:
// \file
// \brief Searches over a grid using normalised correlation
// \author Tim Cootes

#include <iostream>
#include <iosfwd>
#include <mfpf/mfpf_point_finder.h>
#include <vgl/vgl_fwd.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Searches over a grid using normalised correlation.
class mfpf_norm_corr2d : public mfpf_point_finder
{
 private:
  //: Kernel reference point (in kni_ x knj_ grid)
  double ref_x_;
  //: Kernel reference point (in kni_ x knj_ grid)
  double ref_y_;

  //: Relative size of region used for estimating overlap
  //  If 0.5, then overlap requires pt inside central 50% of region.
  double overlap_f_;

  //: Filter kernel to search with
  vil_image_view<double> kernel_;

  //: Define default values
  void set_defaults();

 public:

  // Dflt ctor
  mfpf_norm_corr2d();

  // Destructor
  ~mfpf_norm_corr2d() override;

  //: Define filter kernel to search with.
  //  Reference point set to the centre
  void set(const vil_image_view<double>& k);

  //: Define filter kernel to search with
  void set(const vil_image_view<double>& k,
           double ref_x, double ref_y);

  //: Define filter kernel to search with, expressed as a vector
  bool set_model(const std::vector<double>& v) override;

  //: Number of dimensions in the model
  unsigned model_dim() override;

  //: Relative size of region used for estimating overlap
  //  If 0.5, then overlap requires pt inside central 50% of region.
  void set_overlap_f(double);

  //: Filter kernel to search with
  const vil_image_view<double>& kernel() const { return kernel_; }

  //: Filter kernel to search with, expressed as a vector
  void get_kernel_vector(std::vector<double>& v) const;

  //: Get sample of region around specified point in image
  void get_sample_vector(const vimt_image_2d_of<float>& image,
                                 const vgl_point_2d<double>& p,
                                 const vgl_vector_2d<double>& u,
                                 std::vector<double>& v) override;

  //: Radius of circle containing modelled region
  double radius() const override;

  //: Evaluate match at p, using u to define scale and orientation
  // Returns -1*edge strength at p along direction u
  double evaluate(const vimt_image_2d_of<float>& image,
                          const vgl_point_2d<double>& p,
                          const vgl_vector_2d<double>& u) override;

  //: Evaluate match at in a region around p
  // Returns a quality of fit at a set of positions.
  // response image (whose size and transform is set inside the
  // function), indicates the points at which the function was
  // evaluated.  response(i,j) is the fit at the point
  // response.world2im().inverse()(i,j).  The world2im() transformation
  // may be affine.
  void evaluate_region(const vimt_image_2d_of<float>& image,
                               const vgl_point_2d<double>& p,
                               const vgl_vector_2d<double>& u,
                               vimt_image_2d_of<double>& response) override;

  //: Search given image around p, using u to define scale and angle
  //  On exit, new_p defines position of the best nearby match.
  //  Returns a quality of fit measure at that
  //  point (the smaller the better).
  double search_one_pose(const vimt_image_2d_of<float>& image,
                                 const vgl_point_2d<double>& p,
                                 const vgl_vector_2d<double>& u,
                                 vgl_point_2d<double>& new_p) override;

  // Returns true if p is inside region at given pose
  // Actually only checks if p is inside bounding box,
  // scaled by a factor f about the reference point.
  bool is_inside(const mfpf_pose& pose,
                 const vgl_point_2d<double>& p,
                 double f=1.0) const;

  //: Return true if modelled regions at pose1 and pose2 overlap
  //  Checks if reference point of one is inside region of other
  bool overlap(const mfpf_pose& pose1,
                       const mfpf_pose& pose2) const override;

  //: Generate points in ref frame that represent boundary
  //  Points of a contour around the shape.
  //  Used for display purposes.
  void get_outline(std::vector<vgl_point_2d<double> >& pts) const override;

  //: Return an image of the kernel
  void get_image_of_model(vimt_image_2d_of<vxl_byte>& image) const override;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  std::string is_a() const override;

  //: Create a copy on the heap and return base class pointer
  mfpf_point_finder* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;

  //: Test equality
  bool operator==(const mfpf_norm_corr2d& nc) const;
};

#endif
