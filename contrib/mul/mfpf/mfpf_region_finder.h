#ifndef mfpf_region_finder_h_
#define mfpf_region_finder_h_
//:
// \file
// \brief Searches for an arbitrary shaped region
// \author Tim Cootes

#include <mfpf/mfpf_point_finder.h>
#include <mfpf/mfpf_vec_cost.h>
#include <mbl/mbl_cloneable_ptr.h>
#include <mbl/mbl_chord.h>
#include <vcl_iosfwd.h>

//: Searches with a PDF of an arbitrary region.
//  Records a PDF of the normalised intensities in a
//  region of interest, defined by the set of mbl_chords roi_.
//  These are in the bounding box [0,roi_ni_)x[0,roi_nj_).
class mfpf_region_finder : public mfpf_point_finder
{
 private:
  //: Kernel reference point (in roi_ni_ x roi_nj_ grid)
  double ref_x_;
  //: Kernel reference point (in roi_ni_ x roi_nj_ grid)
  double ref_y_;

  //: Chords defining the region of interest
  vcl_vector<mbl_chord> roi_;

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

  //: Define default values
  void set_defaults();

 public:

  // Dflt ctor
  mfpf_region_finder();

  // Destructor
  virtual ~mfpf_region_finder();

  //: Define region and PDF of region
  void set(const vcl_vector<mbl_chord>& roi,
           double ref_x, double ref_y,
           const mfpf_vec_cost& cost,
           short norm_method=1);

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
  // Returns a qualtity of fit at a set of positions.
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
  //  Returns a qualtity of fit measure at that
  //  point (the smaller the better).
  virtual double search_one_pose(const vimt_image_2d_of<float>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u,
                        vgl_point_2d<double>& new_p);

  // Returns true if p is inside region at given pose
  // Actually only checks if p is inside bounding box
  bool is_inside(const mfpf_pose& pose,
                 const vgl_point_2d<double>& p) const;

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

  //: Prints ASCII representation of shape to os
  void print_shape(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);

  //: Test equality
  bool operator==(const mfpf_region_finder& nc) const;
};

#endif
