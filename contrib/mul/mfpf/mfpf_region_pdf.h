#ifndef mfpf_region_pdf_h_
#define mfpf_region_pdf_h_
//:
// \file
// \brief Searches with a PDF of an arbitrary region
// \author Tim Cootes

#include <mfpf/mfpf_point_finder.h>
#include <vpdfl/vpdfl_pdf_base.h>
#include <mbl/mbl_cloneable_ptr.h>
#include <mbl/mbl_chord.h>
#include <vcl_iosfwd.h>

//: Searches with a PDF of an arbitrary region.
//  Records a PDF of the normalised intensities in a
//  region of interest, defined by the set of mbl_chords roi_.
//  These are in the bounding box [0,roi_ni_)x[0,roi_nj_).
class mfpf_region_pdf : public mfpf_point_finder
{
 private:
  //: Size of step between sample points
  double step_size_;

  //: Kernel reference point (in roi_ni_ x roi_nj_ grid)
  double ref_x_;
  //: Kernel reference point (in roi_ni_ x roi_nj_ grid)
  double ref_y_;

  //: Number of angles to try at
  unsigned nA_;

  //: Angle step size (ie try at A+idA, i in [-nA,+nA]
  double dA_;

  //: Number of scales to try at
  unsigned ns_;

  //: Scaling factor (ie try at (s^i), i in [-ns,+ns]
  double s_;


  //: Chords defining the region of interest
  vcl_vector<mbl_chord> roi_;

  //: Size of bounding box of region of interest
  unsigned roi_ni_;
  //: Size of bounding box of region of interest
  unsigned roi_nj_;

  //: Number of pixels in region
  unsigned n_pixels_;

  //: PDf for vector sampled over ROI
  mbl_cloneable_ptr<vpdfl_pdf_base> pdf_;

  //: Number of points either side of centre to search
  int search_ni_;

  //: Number of points either side of centre to search
  int search_nj_;

  //: Define default values
  void set_defaults();


  //: Search given image around p, using u to define scale and angle 
  //  On exit, new_p defines position of the best nearby match.  
  //  Returns a qualtity of fit measure at that
  //  point (the smaller the better).
  virtual double search_one_pose(const vimt_image_2d_of<float>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u,
                        vgl_point_2d<double>& new_p);

 public:

  // Dflt ctor
  mfpf_region_pdf();

  // Destructor
  virtual ~mfpf_region_pdf();

  //: Size of step between sample points
  virtual void set_step_size(double);

  //: Define region and PDF of region
  void set(const vcl_vector<mbl_chord>& roi,
           double ref_x, double ref_y,
           const vpdfl_pdf_base& pdf);

  //: Define search size
  virtual void set_search_area(unsigned ni, unsigned nj);

  //: Define angle search parameters
  void set_angle_range(unsigned nA, double dA);

  //: Define scale search parameters
  void set_scale_range(unsigned ns, double s);

  int search_ni() const { return search_ni_; }
  int search_nj() const { return search_nj_; }

  //: PDf for region vector
  const vpdfl_pdf_base& pdf() const { return pdf_; }

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

  //: Search given image around p, using u to define scale and orientation
  //  On exit, new_p and new_u define position, scale and orientation of
  //  the best nearby match.  Returns a qualtity of fit measure at that
  //  point (the smaller the better).
  virtual double search(const vimt_image_2d_of<float>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u,
                        vgl_point_2d<double>& new_p,
                        vgl_vector_2d<double>& new_u);

  //: Initialise from a string stream
  virtual bool set_from_stream(vcl_istream &is);

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
  bool operator==(const mfpf_region_pdf& nc) const;
};

#endif
