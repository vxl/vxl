#ifndef mfpf_norm_corr1d_h_
#define mfpf_norm_corr1d_h_

//:
// \file
// \brief Searches along a profile using normalised correlation
// \author Tim Cootes

#include <iostream>
#include <iosfwd>
#include <mfpf/mfpf_point_finder.h>
#include <vnl/vnl_vector.h>
#include <vgl/vgl_fwd.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Searches along a profile using normalised correlation.
class mfpf_norm_corr1d : public mfpf_point_finder
{
 private:
  //: Kernel mask is [ilo_,ihi_]
  int ilo_;
  //: Kernel mask is [ilo_,ihi_]
  int ihi_;

  //: Filter kernel to search with
  vnl_vector<double> kernel_;

  //: Define default values
  void set_defaults();
 public:

  //: Dflt ctor
  mfpf_norm_corr1d();

  //: Destructor
  ~mfpf_norm_corr1d() override;

  //: Define filter kernel to search with
  void set(int ilo, int ihi, const vnl_vector<double>& k);

  //: Kernel mask is [ilo_,ihi_]
  int ilo() const { return ilo_; }

  //: Kernel mask is [ilo_,ihi_]
  int ihi() const { return ihi_; }

  //: Filter kernel to search with
  const vnl_vector<double>& kernel() const { return kernel_; }

  //: Number of dimensions in the model
  unsigned model_dim() override;

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

  //: Search given image around p, using u to define scale and orientation
  //  On exit, new_p and new_u define position, scale and orientation of
  //  the best nearby match.  Returns a quality of fit measure at that
  //  point (the smaller the better).
  double search_one_pose(const vimt_image_2d_of<float>& image,
                                 const vgl_point_2d<double>& p,
                                 const vgl_vector_2d<double>& u,
                                 vgl_point_2d<double>& new_p) override;

  //: Generate points in ref frame that represent boundary
  //  Points of a contour around the shape.
  //  Used for display purposes.
  void get_outline(std::vector<vgl_point_2d<double> >& pts) const override;

  //: Name of the class
  std::string is_a() const override;

  //: Create a copy on the heap and return base class pointer
  mfpf_point_finder* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Version number for I/O
  short version_no() const;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;

  //: Test equality
  bool operator==(const mfpf_norm_corr1d& nc) const;
};

#endif
