#ifndef mfpf_region_pdf_builder_h_
#define mfpf_region_pdf_builder_h_
//:
// \file
// \brief Builder for mfpf_region_pdf objects.
// \author Tim Cootes

#include <mfpf/mfpf_point_finder_builder.h>
#include <vpdfl/vpdfl_builder_base.h>
#include <mbl/mbl_cloneable_ptr.h>
#include <mbl/mbl_chord.h>
#include <vcl_iosfwd.h>

//: Builder for mfpf_region_pdf objects.
// Text for configuring:
// \verbatim
// mfpf_region_pdf_builder { shape: ellipse { ri: 5 rj: 3  }
//  pdf_builder: vpdfl_axis_gaussian_builder { }
//  search_ni: 5 search_nj: 4
// }
// Alternative for shape:
//   shape: box { ni: 5 nj: 3 ref_x: 2.5 ref_y: 1.5 }
// \endverbatim
class mfpf_region_pdf_builder : public mfpf_point_finder_builder
{
 private:
  //: Size of step between sample points
  double step_size_;

  //: Kernel reference point (in roi_ni_ x roi_nj_ grid)
  double ref_x_;
  //: Kernel reference point (in roi_ni_ x roi_nj_ grid)
  double ref_y_;

  //: String defining shape of region, eg "box" or "ellipse"
  vcl_string shape_;

  //: Chords defining the region of interest
  vcl_vector<mbl_chord> roi_;

  //: Size of bounding box of region of interest
  unsigned roi_ni_;
  //: Size of bounding box of region of interest
  unsigned roi_nj_;

  //: Number of pixels in region
  unsigned n_pixels_;

  //: Builder for PDf for profile vector
  mbl_cloneable_ptr<vpdfl_builder_base> pdf_builder_;

  //: Samples added in calls to add_example()
  vcl_vector<vnl_vector<double> > data_;

  //: Number of points either side of centre to search
  int search_ni_;

  //: Number of points either side of centre to search
  int search_nj_;

  //: Number of angles either side of 0 to sample at
  unsigned nA_;

  //: Angle displacement
  double dA_;

  //: Define default values
  void set_defaults();

  //: Define model region as an ni x nj box
  void set_as_box(unsigned ni, unsigned nj,
                  double ref_x, double ref_y);

  //: Define model region as an ellipse with radii ri, rj
  //  Ref. point in centre.
  void set_as_ellipse(double ri, double rj);

  //: Parse stream to set up as a box shape.
  // Expects: "{ ni: 3 nj: 5 ref_x: 1.0 ref_y: 2.0 }
  void config_as_box(vcl_istream &is);

  //: Parse stream to set up as an ellipse shape.
  // Expects: "{ ri: 2.1 rj: 5.2 }
  void config_as_ellipse(vcl_istream &is);

  //: Add one example to the model
  void add_one_example(const vimt_image_2d_of<float>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u);

 public:

  // Dflt ctor
  mfpf_region_pdf_builder();

  // Destructor
  virtual ~mfpf_region_pdf_builder();

  //: Size of step between sample points
  virtual void set_step_size(double);

  //: Define model region as an ni x nj box
  void set_as_box(unsigned ni, unsigned nj,
                  double ref_x, double ref_y,
                  const vpdfl_builder_base& builder);

  //: Define model region as an ni x nj box
  //  Ref. point in centre.
  void set_as_box(unsigned ni, unsigned nj,
                  const vpdfl_builder_base& builder);

  //: Define model region as an ellipse with radii ri, rj
  //  Ref. point in centre.
  void set_as_ellipse(double ri, double rj,
                  const vpdfl_builder_base& builder);

  //: Number of pixels in region
  unsigned n_pixels() const { return n_pixels_; }

  //: String defining shape of region, eg "box" or "ellipse"
  const vcl_string& shape() const { return shape_; }

  //: Builder for PDF
  vpdfl_builder_base& pdf_builder() { return pdf_builder_; }

  int search_ni() const { return search_ni_; }
  int search_nj() const { return search_nj_; }

  //: Create new mfpf_region_pdf on heap
  virtual mfpf_point_finder* new_finder() const;

  //: Initialise building
  // Must be called before any calls to add_example(...)
  virtual void clear(unsigned n_egs);

  //: Add one example to the model
  virtual void add_example(const vimt_image_2d_of<float>& image,
                           const vgl_point_2d<double>& p,
                           const vgl_vector_2d<double>& u);

  //: Build object from the data supplied in add_example()
  virtual void build(mfpf_point_finder&);

  //: Initialise from a string stream
  virtual bool set_from_stream(vcl_istream &is);

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Create a copy on the heap and return base class pointer
  virtual mfpf_point_finder_builder* clone() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Prints ASCII representation of shape to os
  void print_shape(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#endif
