#ifndef mfpf_lin_clsfy_finder_builder_h_
#define mfpf_lin_clsfy_finder_builder_h_
//:
// \file
// \brief Builds mfpf_region_finder objects which use a linear classifier
// \author Tim Cootes

#include <mfpf/mfpf_point_finder_builder.h>
#include <mfpf/mfpf_vec_cost_builder.h>
#include <mfpf/mfpf_region_form.h>
#include <mbl/mbl_chord.h>
#include <vgl/vgl_fwd.h>
#include <vcl_iosfwd.h>

//: Builds mfpf_region_finder objects which use a linear classifier.
//  Resulting mfpf_region_finder is set up with a mfpf_log_lin_class_cost
//  function, which returns log(prob(patch)) based on a linear classifier,
//  trained with positive examples at supplied points and negative
//  examples in an annulus around the supplied points.
//
// Text for configuring:
// \verbatim
// mfpf_lin_clsfy_finder_builder { shape: ellipse { ri: 5 rj: 3  }
//  // Samples within r1 pixels of true pt are considered positive examples
//  r1: 1.5
//  // Samples in range (r2,r3] of true pt are negative examples
//  r2: 3 r3: 7
//  norm: linear
//  search_ni: 5 search_nj: 4
// }
// Alternative for shape:
//   shape: box { ni: 5 nj: 3 ref_x: 2.5 ref_y: 1.5 }
// \endverbatim
class mfpf_lin_clsfy_finder_builder : public mfpf_point_finder_builder
{
 private:
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

  // Samples within r1 pixels of true pt are considered positive examples
  double r1_;
  // Samples in range (r2,r3] of true pt are negative examples
  double r2_;
  // Samples in range (r2,r3] of true pt are negative examples
  double r3_;

  //: Training samples
  vcl_vector<vnl_vector<double> > samples_;

  //: Indicate whether sample is positive (1) or negative (0) example
  vcl_vector<unsigned> class_id_;

  //: Which normalisation to use (0=none, 1=linear)
  short norm_method_;

  //: Number of angles either side of 0 to sample at
  unsigned nA_;

  //: Angle displacement
  double dA_;

  //: Relative size of region used for estimating overlap
  //  If 0.5, then overlap requires pt inside central 50% of region.
  double overlap_f_;

  //: lower bound on variance used in normalisation
  double var_min_;

  //: lowest variance found so far in training set
  double tvar_min_;

  //: If true reset var_min based on min in training set
  bool estimate_var_min_;

    //: Number of examples added
  unsigned num_examples_;

  //: Define default values
  void set_defaults();

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
  mfpf_lin_clsfy_finder_builder();

  // Destructor
  virtual ~mfpf_lin_clsfy_finder_builder();

  //: Define model region as an ni x nj box
  void set_as_box(unsigned ni, unsigned nj,
                  double ref_x, double ref_y);

  //: Define model region as an ni x nj box
  //  Ref. point in centre.
  void set_as_box(unsigned ni, unsigned nj);

  //: Define model region as an ellipse with radii ri, rj
  //  Ref. point in centre.
  void set_as_ellipse(double ri, double rj);

  //: Define model region using description in form
  //  Assumes form defined in world-coords.
  //  Sets step_size() to form.pose().scale().
  void set_region(const mfpf_region_form& form);

  //: Which normalisation to use (0=none, 1=linear)
  void set_norm_method(short norm_method);


  //: Define region size in world co-ordinates
  //  Sets up ROI to cover given box (with samples at step_size()),
  //  with ref point at centre.
  //  Currently just defines as a box
  virtual void set_region_size(double wi, double wj);


  //: Number of pixels in region
  unsigned n_pixels() const { return n_pixels_; }

  //: String defining shape of region, eg "box" or "ellipse"
  const vcl_string& shape() const { return shape_; }

  //: Number of dimensions in the model
  virtual unsigned model_dim();

  //: Create new mfpf_region_finder on heap
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

  //: Version number for I/O
  short version_no() const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#endif // mfpf_lin_clsfy_finder_builder_h_
