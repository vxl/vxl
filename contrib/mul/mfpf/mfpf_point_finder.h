#ifndef mfpf_point_finder_h_
#define mfpf_point_finder_h_
//:
// \file
// \brief Base for classes which locate feature points
// \author Tim Cootes

#include <vcl_string.h>
#include <vsl/vsl_binary_io.h>
#include <vcl_memory.h>

#include <vimt/vimt_image_2d_of.h>
#include <vgl/vgl_vector_2d.h>

#include <mfpf/mfpf_pose.h>

//: Base for classes which locate feature points
//  The object will be set up by an 
//  associated mfpf_point_finder_builder object
class mfpf_point_finder
{
 protected:
  //: Size of step between sample points
  double step_size_;

  //: Number of points either side of centre to search
  int search_ni_;

  //: Number of points either side of centre to search
  int search_nj_;

  //: Define N. angles (ie try at A+idA, i in [-nA,+nA]) 
  unsigned nA_;

  //: Angle step size (ie try at A+idA, i in [-nA,+nA])
  double dA_;

  //: Number of scales to try at
  unsigned ns_;

  //: Scaling factor (ie try at ((ds)^i), i in [-ns,+ns]
  double ds_;

  //: Return true if base class parameters are the same in pf
  bool base_equality(const mfpf_point_finder& pf) const;
 public:

  //: Dflt ctor
  mfpf_point_finder();

  //: Destructor
  virtual ~mfpf_point_finder();

  //: Size of step between sample points
  virtual void set_step_size(double);

  //: Size of step between sample points
  double step_size() const { return step_size_; };

  //: Define search region size
  //  During search, samples at points on grid [-ni,ni]x[-nj,nj],
  //  with axes defined by u.
  virtual void set_search_area(unsigned ni, unsigned nj);

  //: Define angle search parameters
  void set_angle_range(unsigned nA, double dA);

  //: Define scale search parameters
  void set_scale_range(unsigned ns, double ds);

  int search_ni() const { return search_ni_; }
  int search_nj() const { return search_nj_; }

  //: Radius of circle containing modelled region (in model frame units)
  //  Radius in world units given by step_size()*radius()
  virtual double radius() const = 0;

  //: Evaluate match at p, using u to define scale and orientation
  // Returns a qualtity of fit measure at the point (the smaller the better).
  virtual double evaluate(const vimt_image_2d_of<float>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u)=0;

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
                               vimt_image_2d_of<double>& response)=0;

  //: Search given image around p, using u to define scale and angle 
  //  On exit, new_p defines position of the best nearby match.
  //  Returns a qualtity of fit measure at that
  //  point (the smaller the better).
  virtual double search_one_pose(const vimt_image_2d_of<float>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u,
                        vgl_point_2d<double>& new_p)=0;

  //: Search given image around p, using u to define scale and orientation
  //  On exit, new_p and new_u define position, scale and orientation of
  //  the best nearby match.  Returns a quality of fit measure at that
  //  point (the smaller the better).
  //
  //  Default impementation calls search_one_pose(...) at multiple
  //  angles and scales. Result will be at a grid position
  //  and one of the given angle/scales.  True optima can then
  //  be found by further optimisation of the point.
  virtual double search(const vimt_image_2d_of<float>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u,
                        vgl_point_2d<double>& new_p,
                        vgl_vector_2d<double>& new_u);

  //: Search for local optima around given point/scale/angle
  //  Search in a grid around p (defined by search_ni and search_nj).
  //  Find local minima on this grid and return append each to
  //  pts.  Note: pts is not resized, so empty beforehand if necessary.
  virtual void grid_search_one_pose(const vimt_image_2d_of<float>& image,
                           const vgl_point_2d<double>& p,
                           const vgl_vector_2d<double>& u,
                           vcl_vector<mfpf_pose>& pts,
                           vcl_vector<double>& fit);

  //: Search for local optima around given point/scale/angle
  //  For each angle and scale (defined by internal nA,dA,ns,ds)
  //  search in a grid around p (defined by search_ni and search_nj).
  //  Find local minima on this grid and return them in pts.
  //
  //  Note that an object in an image may lead to multiple responses,
  //  one at each scale and angle near to the optima.  Thus the
  //  poses defined in pts should be further refined to eliminate
  //  such multiple responses.
  virtual void grid_search(const vimt_image_2d_of<float>& image,
                           const vgl_point_2d<double>& p,
                           const vgl_vector_2d<double>& u,
                           vcl_vector<mfpf_pose>& pts,
                           vcl_vector<double>& fit);

  //: Perform local optimisation to refine position,scale and angle
  //  On input fit is match at p,u.  On exit p,u and fit are updated.
  //  Baseclass implementation uses simplex optimisation.
  virtual void refine_match(const vimt_image_2d_of<float>& image,
                           vgl_point_2d<double>& p,
                           vgl_vector_2d<double>& u,
                           double& fit);

  //: Return true if modelled regions at pose1 and pose2 overlap
  virtual bool overlap(const mfpf_pose& pose1,
                       const mfpf_pose& pose2) const;

  //: Generate points in ref frame that represent boundary
  //  Points of a contour around the shape.
  //  Used for display purposes.  Join the points with an open
  //  contour to get a representation.
  virtual void get_outline(vcl_vector<vgl_point_2d<double> >& pts) const=0;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Create a copy on the heap and return base class pointer
  virtual mfpf_point_finder* clone() const = 0;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

//: Allows derived class to be loaded by base-class pointer
void vsl_add_to_binary_loader(const mfpf_point_finder& b);

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mfpf_point_finder& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mfpf_point_finder& b);

//: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const mfpf_point_finder& b);

//: Stream output operator for class pointer
vcl_ostream& operator<<(vcl_ostream& os,const mfpf_point_finder* b);

#endif // mfpf_point_finder_h_
