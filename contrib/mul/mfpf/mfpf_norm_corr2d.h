#ifndef mfpf_norm_corr2d_h_
#define mfpf_norm_corr2d_h_
//:
// \file
// \brief Searches over a grid using normalised correllation
// \author Tim Cootes

#include <mfpf/mfpf_point_finder.h>
#include <vcl_iosfwd.h>

//: Searches over a grid using normalised correllation.
class mfpf_norm_corr2d : public mfpf_point_finder
{
 private:
  //: Kernel reference point (in kni_ x knj_ grid)
  double ref_x_;
  //: Kernel reference point (in kni_ x knj_ grid)
  double ref_y_;

  //: Filter kernel to search with
  vil_image_view<double> kernel_;

  //: Define default values
  void set_defaults();


public:

  // Dflt ctor
  mfpf_norm_corr2d();

  // Destructor
  virtual ~mfpf_norm_corr2d();

  //: Define filter kernel to search with.
  //  Reference point set to the centre
  void set(const vil_image_view<double>& k);

  //: Define filter kernel to search with
  void set(const vil_image_view<double>& k,
           double ref_x, double ref_y);

  //: Filter kernel to search with
  const vil_image_view<double>& kernel() const { return kernel_; }

  //: Radius of circle containing modelled region
  virtual double radius() const;

  //: Evaluate match at p, using u to define scale and orientation
  // Returns -1*edge strength at p along direction u
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

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);

  //: Test equality
  bool operator==(const mfpf_norm_corr2d& nc) const;
};

#endif
