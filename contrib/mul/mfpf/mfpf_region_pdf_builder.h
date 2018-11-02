#ifndef mfpf_region_pdf_builder_h_
#define mfpf_region_pdf_builder_h_
//:
// \file
// \brief Builder for mfpf_region_pdf objects.
// \author Tim Cootes

#include <iostream>
#include <iosfwd>
#include <mfpf/mfpf_point_finder_builder.h>
#include <vpdfl/vpdfl_builder_base.h>
#include <mbl/mbl_cloneable_ptr.h>
#include <mbl/mbl_chord.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mfpf/mfpf_region_form.h>
#include <vgl/vgl_fwd.h>

//: Builder for mfpf_region_pdf objects.
// Text for configuring:
// \verbatim
// mfpf_region_pdf_builder { shape: ellipse { ri: 5 rj: 3  }
//  pdf_builder: vpdfl_axis_gaussian_builder { }
//  norm: linear
//  search_ni: 5 search_nj: 4
//  overlap_f: 0.5
// }
// Alternative for shape:
//   shape: box { ni: 5 nj: 3 ref_x: 2.5 ref_y: 1.5 }
// \endverbatim
class mfpf_region_pdf_builder : public mfpf_point_finder_builder
{
 private:
  //: Kernel reference point (in roi_ni_ x roi_nj_ grid)
  double ref_x_;
  //: Kernel reference point (in roi_ni_ x roi_nj_ grid)
  double ref_y_;

  //: String defining shape of region, eg "box" or "ellipse"
  std::string shape_;

  //: Chords defining the region of interest
  std::vector<mbl_chord> roi_;

  //: Size of bounding box of region of interest
  unsigned roi_ni_;
  //: Size of bounding box of region of interest
  unsigned roi_nj_;

  //: Number of pixels in region
  unsigned n_pixels_;

  //: Builder for PDf for profile vector
  mbl_cloneable_ptr<vpdfl_builder_base> pdf_builder_;

  //: Which normalisation to use (0=none, 1=linear)
  short norm_method_;

  //: Samples added in calls to add_example()
  std::vector<vnl_vector<double> > data_;

  //: Number of angles either side of 0 to sample at
  unsigned nA_;

  //: Angle displacement
  double dA_;

  //: Relative size of region used for estimating overlap
  //  If 0.5, then overlap requires pt inside central 50% of region.
  double overlap_f_;

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
  void config_as_box(std::istream &is);

  //: Parse stream to set up as an ellipse shape.
  // Expects: "{ ri: 2.1 rj: 5.2 }
  void config_as_ellipse(std::istream &is);

  //: Add one example to the model
  void add_one_example(const vimt_image_2d_of<float>& image,
                       const vgl_point_2d<double>& p,
                       const vgl_vector_2d<double>& u);

 public:

  // Dflt ctor
  mfpf_region_pdf_builder();

  // Destructor
  ~mfpf_region_pdf_builder() override;

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

  //: Define model region using description in form
  //  Assumes form defined in world-coords.
  //  Sets step_size() to form.pose().scale().
  void set_region(const mfpf_region_form& form);

  //: Define region size in world co-ordinates
  //  Sets up ROI to cover given box (with samples at step_size()),
  //  with ref point at centre.
  //  Currently just defines as a box
  void set_region_size(double wi, double wj) override;


  //: Number of pixels in region
  unsigned n_pixels() const { return n_pixels_; }

  //: String defining shape of region, eg "box" or "ellipse"
  const std::string& shape() const { return shape_; }

  //: Builder for PDF
  vpdfl_builder_base& pdf_builder() { return pdf_builder_; }

  //: Create new mfpf_region_pdf on heap
  mfpf_point_finder* new_finder() const override;

  //: Initialise building
  // Must be called before any calls to add_example(...)
  void clear(unsigned n_egs) override;

  //: Add one example to the model
  void add_example(const vimt_image_2d_of<float>& image,
                           const vgl_point_2d<double>& p,
                           const vgl_vector_2d<double>& u) override;

  //: Build object from the data supplied in add_example()
  void build(mfpf_point_finder&) override;

  //: Initialise from a string stream
  bool set_from_stream(std::istream &is) override;

  //: Name of the class
  std::string is_a() const override;

  //: Create a copy on the heap and return base class pointer
  mfpf_point_finder_builder* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Prints ASCII representation of shape to os
  void print_shape(std::ostream& os) const;

  //: Version number for I/O
  short version_no() const;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;
};

#endif
