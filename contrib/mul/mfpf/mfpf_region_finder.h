#ifndef mfpf_region_finder_h_
#define mfpf_region_finder_h_
//:
// \file
// \brief Searches for an arbitrary shaped region
// \author Tim Cootes

#include <iostream>
#include <iosfwd>
#include <mfpf/mfpf_point_finder.h>
#include <mfpf/mfpf_vec_cost.h>
#include <mbl/mbl_cloneable_ptr.h>
#include <mbl/mbl_chord.h>
#include <vgl/vgl_fwd.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Searches to minimise a cost of pixels in an arbitrary region.
//  Samples intensities in a region of interest, defined by the set of mbl_chords roi_.
//  These are in the bounding box [0,roi_ni_)x[0,roi_nj_).
//  These are optionally normalised, then fed into a mfpf_vec_cost function to
//  evaluate the quality.
class mfpf_region_finder : public mfpf_point_finder
{
 private:
  //: Kernel reference point (in roi_ni_ x roi_nj_ grid)
  double ref_x_;
  //: Kernel reference point (in roi_ni_ x roi_nj_ grid)
  double ref_y_;

  //: Chords defining the region of interest
  std::vector<mbl_chord> roi_;

  //: Size of bounding box of region of interest
  unsigned roi_ni_;
  //: Size of bounding box of region of interest
  unsigned roi_nj_;

  //: Number of pixels in region
  unsigned n_pixels_;

  //: Cost for vector sampled over ROI
  mbl_cloneable_ptr<mfpf_vec_cost> cost_;

  //: Which normalisation to use (0=none, 1=linear)
  short norm_method_;

  //: Relative size of region used for estimating overlap
  //  If 0.5, then overlap requires pt inside central 50% of region.
  double overlap_f_;

  //: Lower bound on variance used in normalisation
  double var_min_;

  //: When generating visual image of model with multi-planes, is only the first plane drawn or L2 norm of all planes
  bool draw_only_1st_plane_;

  //: Define default values
  void set_defaults();

 public:

  // Dflt ctor
  mfpf_region_finder();

  // Destructor
  ~mfpf_region_finder() override;

  //: Define region and cost function to be used
  void set(const std::vector<mbl_chord>& roi,
           double ref_x, double ref_y,
           const mfpf_vec_cost& cost,
           short norm_method=1);

  //: Relative size of region used for estimating overlap
  //  If 0.5, then overlap requires pt inside central 50% of region.
  void set_overlap_f(double);

  //: Minimum variance used when normalising patch
  void set_var_min(double var_min) {var_min_=var_min;}

  //: Minimum variance used when normalising patch
  double var_min() const {return var_min_;}

  //: Radius of circle containing modelled region
  double radius() const override;

  //: Cost function for region vector
  const mfpf_vec_cost& cost() const { return cost_; }

  //: Cost function for region vector
  mfpf_vec_cost& cost() { return cost_; }

  //: Evaluate match at p, using u to define scale and orientation
  // Returns cost()(region_sample) at p along direction u
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

  //: Create an image summarising the average model (where possible)
  //  Creates an image of the mean template used for search.
  //  image.world2im() gives mapping from reference frame
  //  into raw image co-ords (including the step size).
  void get_image_of_model(vimt_image_2d_of<vxl_byte>& image) const override;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  std::string is_a() const override;

  //: Create a copy on the heap and return base class pointer
  mfpf_point_finder* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Prints ASCII representation of shape to os
  void print_shape(std::ostream& os) const;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;

  //: Test equality
  bool operator==(const mfpf_region_finder& nc) const;
};

#endif
