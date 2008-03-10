#ifndef mfpf_edge_finder_h_
#define mfpf_edge_finder_h_

//:
// \file
// \brief Locates strongest edge along a profile
// \author Tim Cootes

#include <mfpf/mfpf_point_finder.h>
#include <vcl_iosfwd.h>

//: Locates strongest edge along a profile.
class mfpf_edge_finder : public mfpf_point_finder
{
 public:

  //: Dflt ctor
  mfpf_edge_finder();

  //: Destructor
  virtual ~mfpf_edge_finder();

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

  //: Search given image around p, using u to define scale and orientation
  //  On exit, new_p and new_u define position, scale and orientation of
  //  the best nearby match.  Returns a qualtity of fit measure at that
  //  point (the smaller the better).
  virtual double search_one_pose(const vimt_image_2d_of<float>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u,
                        vgl_point_2d<double>& new_p);

  //: Generate points in ref frame that represent boundary
  //  Points of a closed contour around the shape.
  //  Used for display purposes.
  virtual void get_outline(vcl_vector<vgl_point_2d<double> >& pts) const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Create a copy on the heap and return base class pointer
  virtual mfpf_point_finder* clone() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Version number for I/O
  short version_no() const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#endif
