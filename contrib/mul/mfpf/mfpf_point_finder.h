#ifndef mfpf_point_finder_h_
#define mfpf_point_finder_h_
//:
// \file
// \brief Base for classes which locate feature points
// \author Tim Cootes

#include <string>
#include <iostream>
#include <iosfwd>
#include <vimt/vimt_image_2d_of.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>

#include <mfpf/mfpf_pose.h>
#include <vxl_config.h>  // For vxl_byte

#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class vimt_image_pyramid;

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
  double step_size() const { return step_size_; }

  //: Define search region size
  //  During search, samples at points on grid [-ni,ni]x[-nj,nj],
  //  with axes defined by u.
  virtual void set_search_area(unsigned ni, unsigned nj);

  //: Define angle search parameters
  void set_angle_range(unsigned nA, double dA);

  //: Define scale search parameters
  void set_scale_range(unsigned ns, double ds);

  //: Number of points either side of centre to search along i
  int search_ni() const { return search_ni_; }

  //: Number of points either side of centre to search along j
  int search_nj() const { return search_nj_; }

  //: Set model to a new value when provided with a vector
  virtual bool set_model(const std::vector<double>& v);

  //: Number of dimensions in the model
  virtual unsigned model_dim();

  //: Radius of circle containing modelled region (in model frame units)
  //  Radius in world units given by step_size()*radius()
  virtual double radius() const = 0;

  //: Get sample of region around specified point in image
  virtual void get_sample_vector(const vimt_image_2d_of<float>& image,
                                 const vgl_point_2d<double>& p,
                                 const vgl_vector_2d<double>& u,
                                 std::vector<double>& v);

  //: Evaluate match at p, using u to define scale and orientation
  // Returns a quality of fit measure at the point (the smaller the better).
  virtual double evaluate(const vimt_image_2d_of<float>& image,
                          const vgl_point_2d<double>& p,
                          const vgl_vector_2d<double>& u) = 0;

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
  //  Returns a quality of fit measure at that
  //  point (the smaller the better).
  virtual double search_one_pose(const vimt_image_2d_of<float>& image,
                                 const vgl_point_2d<double>& p,
                                 const vgl_vector_2d<double>& u,
                                 vgl_point_2d<double>& new_p)=0;

  //: Search given image around p, using u to define scale and angle
  //  Evaluates responses on a grid, finds the best point on the
  //  grid, then optimises its position by fitting a parabola.
  //
  //  On exit, new_p defines position of the best nearby match.
  //  Returns a quality of fit measure at that
  //  point (the smaller the better).
  virtual double search_one_pose_with_opt(
                        const vimt_image_2d_of<float>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u,
                        vgl_point_2d<double>& new_p);

  //: Search given image around p, using u to define scale and orientation
  //  On exit, new_p and new_u define position, scale and orientation of
  //  the best nearby match.  Returns a quality of fit measure at that
  //  point (the smaller the better).
  //
  //  Default implementation calls search_one_pose(...) at multiple
  //  angles and scales. Result will be at a grid position
  //  and one of the given angle/scales.  True optima can then
  //  be found by further optimisation of the point.
  virtual double search(const vimt_image_2d_of<float>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u,
                        vgl_point_2d<double>& new_p,
                        vgl_vector_2d<double>& new_u);

  //: Search given image around p, using u to define scale and orientation
  //  On exit, new_p and new_u define position, scale and orientation of
  //  the best nearby match.  Returns a quality of fit measure at that
  //  point (the smaller the better).
  //  Parabolic fit used to estimate optimal position.
  //
  //  Default implementation calls search_one_pose(...) at multiple
  //  angles and scales. Result will be at a grid position
  //  and one of the given angle/scales.  True optima can then
  //  be found by further optimisation of the point.
  virtual double search_with_opt(
                        const vimt_image_2d_of<float>& image,
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
                                    std::vector<mfpf_pose>& pts,
                                    std::vector<double>& fit);

  //: Search for local optima around given point/scale/angle
  //  Search in a grid around p (defined by search_ni and search_nj).
  //  Find local minima on this grid.
  //  Perform single sub-grid optimisation by fitting a parabola
  //  in x and y and testing resulting point.
  //  Append each to pts.
  //  Note: pts is not resized, so empty beforehand if necessary.
  virtual void multi_search_one_pose(
                           const vimt_image_2d_of<float>& image,
                           const vgl_point_2d<double>& p,
                           const vgl_vector_2d<double>& u,
                           std::vector<mfpf_pose>& pts,
                           std::vector<double>& fit);

  //: Search for local optima around given point/scale/angle
  //  For each angle and scale (defined by internal nA,dA,ns,ds)
  //  search in a grid around p (defined by search_ni and search_nj).
  //  Find local minima on this grid and return them in poses.
  //  Responses lie on grid in (x,y,ds,dA)
  //
  //  Note that an object in an image may lead to multiple responses,
  //  one at each scale and angle near to the optima.  Thus the
  //  poses defined in pts should be further refined to eliminate
  //  such multiple responses.
  virtual void grid_search(const vimt_image_2d_of<float>& image,
                           const vgl_point_2d<double>& p,
                           const vgl_vector_2d<double>& u,
                           std::vector<mfpf_pose>& poses,
                           std::vector<double>& fit);

  //: Search for local optima around given point/scale/angle
  //  For each angle and scale (defined by internal nA,dA,ns,ds)
  //  search in a grid around p (defined by search_ni and search_nj).
  //  Find local minima on this grid.
  //  Sub-grid estimation using parabolic fitting included.
  //  poses[i] defines result i, with corresponding fit fits[i]
  //
  //  Note that an object in an image may lead to multiple responses,
  //  one at each scale and angle near to the optima.  Thus the
  //  poses defined in pts should be further refined to eliminate
  //  such multiple responses.
  virtual void multi_search(const vimt_image_2d_of<float>& image,
                            const vgl_point_2d<double>& p,
                            const vgl_vector_2d<double>& u,
                            std::vector<mfpf_pose>& poses,
                            std::vector<double>& fits);

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
  virtual void get_outline(std::vector<vgl_point_2d<double> >& pts) const=0;

  //: Computes the aligned bounding box for feature with given pose
  //  On exit box_pose.p() gives the centre, corners are given by
  //  box_pose(+/-0.5*wi, +/-0.5*wj).
  virtual void aligned_bounding_box(const mfpf_pose& pose,
                                    mfpf_pose& box_pose,
                                    double& wi, double& wj) const;

  //: Create an image summarising the average model (where possible)
  //  For instance, creates an image of the mean template used for
  //  search. image.world2im() gives mapping from reference frame
  //  into raw image co-ords (including the step size).
  //  Default implementation does nothing - returns an empty image.
  virtual void get_image_of_model(vimt_image_2d_of<vxl_byte>& image) const;

  //: Select best level for searching around pose
  //  Selects pyramid level with pixel sizes best matching
  //  the model pixel size at given pose.
  unsigned image_level(const mfpf_pose& pose,
                       const vimt_image_pyramid& im_pyr) const;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual std::string is_a() const;

  //: Create a copy on the heap and return base class pointer
  virtual mfpf_point_finder* clone() const = 0;

  //: Print class to os
  virtual void print_summary(std::ostream& os) const;

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
std::ostream& operator<<(std::ostream& os,const mfpf_point_finder& b);

//: Stream output operator for class pointer
std::ostream& operator<<(std::ostream& os,const mfpf_point_finder* b);

#endif // mfpf_point_finder_h_
