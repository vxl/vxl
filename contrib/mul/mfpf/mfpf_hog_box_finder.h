#ifndef mfpf_hog_box_finder_h_
#define mfpf_hog_box_finder_h_
//:
// \file
// \brief Searches for rectangular region using HOG features
// \author Tim Cootes

#include <mfpf/mfpf_point_finder.h>
#include <mipa/mipa_vector_normaliser.h>
#include <mfpf/mfpf_vec_cost.h>
#include <mbl/mbl_cloneable_ptr.h>
#include <vcl_iosfwd.h>

//: Searches for rectangular region using HOG features.
//  Features are combinations of histograms of orientations of
//  gradients, pooled over nested regions in a rectangle.
class mfpf_hog_box_finder : public mfpf_point_finder
{
 private:
  //: Kernel reference point (usually centre of sampled region [0,ni)
  double ref_x_;
  //: Kernel reference point (usually centre of sampled region) [0,nj)
  double ref_y_;

  //: Number of angle bins in histogram of orientations
  unsigned nA_bins_;

  //: When true, angles are 0-360, else 0-180
  bool full360_;

  //: Size of each cell for basic histogram is nc x nc
  unsigned nc_;

  //: Size of region is 2*ni by 2*nj cells (each cell is nc*nc)
  unsigned ni_;
  //: Size of region is 2*ni by 2*nj cells (each cell is nc*nc)
  unsigned nj_;

  //: Cost for vector sampled over ROI
  mbl_cloneable_ptr<mfpf_vec_cost> cost_;

  ////: Which normalisation to use (0=none, 1=linear)
  //short norm_method_;

    //: The normaliser
  mbl_cloneable_nzptr<mipa_vector_normaliser> normaliser_;

  //: Relative size of region used for estimating overlap
  //  If 0.5, then overlap requires pt inside central 50% of region.
  double overlap_f_;

  //: Define default values
  void set_defaults();

 public:

  // Dflt ctor
  mfpf_hog_box_finder();

  // Destructor
  virtual ~mfpf_hog_box_finder();

  //: Define region and cost of region
  void set(unsigned nA_bins, bool full360,
           unsigned ni, unsigned nj, unsigned nc,
           double ref_x, double ref_y,
           const mfpf_vec_cost& cost,
           const mbl_cloneable_nzptr<mipa_vector_normaliser>& normaliser);

  //: Relative size of region used for estimating overlap
  //  If 0.5, then overlap requires pt inside central 50% of region.
  void set_overlap_f(double);


  //: Radius of circle containing modelled region
  virtual double radius() const;

  //: Cost function for region vector
  const mfpf_vec_cost& cost() const { return cost_; }

  //: Cost function for region vector
  mfpf_vec_cost& cost() { return cost_; }

  //: Evaluate match at p, using u to define scale and orientation
  // Returns -1*log(p(region)) at p along direction u
  virtual double evaluate(const vimt_image_2d_of<float>& image,
                          const vgl_point_2d<double>& p,
                          const vgl_vector_2d<double>& u);

  //: Evaluate match at in a region around p
  // Returns a quality of fit at a set of positions.
  // response image (whose size and transform is set inside the
  // function), indicates the points at which the function was
  // evaluated.  response(i,j) is the fit at the point
  // response.world2im().inverse()(i,j).  The world2im() transformation
  // may be affine.
  virtual void evaluate_region(const vimt_image_2d_of<float>& image,
                               const vgl_point_2d<double>& p,
                               const vgl_vector_2d<double>& u,
                               vimt_image_2d_of<double>& response);

  //: Search given image around p, using u to define scale and angle
  //  On exit, new_p defines position of the best nearby match.
  //  Returns a quality of fit measure at that
  //  point (the smaller the better).
  virtual double search_one_pose(const vimt_image_2d_of<float>& image,
                                 const vgl_point_2d<double>& p,
                                 const vgl_vector_2d<double>& u,
                                 vgl_point_2d<double>& new_p);

  // Returns true if p is inside region at given pose
  // Actually only checks if p is inside bounding box,
  // scaled by a factor f about the reference point.
  bool is_inside(const mfpf_pose& pose,
                 const vgl_point_2d<double>& p,
                 double f=1.0) const;

  //: Return true if modelled regions at pose1 and pose2 overlap
  //  Checks if reference point of one is inside region of other
  virtual bool overlap(const mfpf_pose& pose1,
                       const mfpf_pose& pose2) const;

  //: Generate points in ref frame that represent boundary
  //  Points of a contour around the shape.
  //  Used for display purposes.
  virtual void get_outline(vcl_vector<vgl_point_2d<double> >& pts) const;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Create a copy on the heap and return base class pointer
  virtual mfpf_point_finder* clone() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);

  //: Test equality
  bool operator==(const mfpf_hog_box_finder& nc) const;
};

#endif
