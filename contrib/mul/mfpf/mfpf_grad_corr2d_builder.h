#ifndef mfpf_grad_corr2d_builder_h_
#define mfpf_grad_corr2d_builder_h_
//:
// \file
// \brief Builder for mfpf_grad_corr2d objects.
// \author Tim Cootes

#include <mfpf/mfpf_point_finder_builder.h>
#include <vcl_iosfwd.h>
#include <vgl/vgl_fwd.h>

//: Builder for mfpf_grad_corr2d objects.
class mfpf_grad_corr2d_builder : public mfpf_point_finder_builder
{
 private:
  //: Kernel reference point (in kni_ x knj_ grid)
  double ref_x_;
  //: Kernel reference point (in kni_ x knj_ grid)
  double ref_y_;

  //: Kernel mask is ni_ x nj_
  unsigned ni_;
  //: Kernel mask is ni_ x nj_
  unsigned nj_;

  //: Number of angles either side of 0 to sample at
  unsigned nA_;

  //: Angle displacement
  double dA_;

  //: Relative size of region used for estimating overlap
  //  If 0.5, then overlap requires pt inside central 50% of region.
  double overlap_f_;

  //: Workspace for sum over x
  vil_image_view<double> sum_x_;

  //: Workspace for sum over y
  vil_image_view<double> sum_y_;

  //: Number of examples added
  unsigned n_added_;

  //: Define default values
  void set_defaults();

  //: Add one example to the model
  void add_one_example(const vimt_image_2d_of<float>& image,
                       const vgl_point_2d<double>& p,
                       const vgl_vector_2d<double>& u);

  //: Compute gradient image around a patch
  void diff_image(const vimt_image_2d_of<float>& image,
                  const vgl_point_2d<double>& p,
                  const vgl_vector_2d<double>& u,
                  vil_image_view<double>& grad_x,
                  vil_image_view<double>& grad_y);

 public:

  // Dflt ctor
  mfpf_grad_corr2d_builder();

  // Destructor
  virtual ~mfpf_grad_corr2d_builder();

  //: Define size of mask
  void set_kernel_size(unsigned ni, unsigned nj,
                       double ref_x, double ref_y);

  //: Define size of mask
  // Ref point is centre
  void set_kernel_size(unsigned ni, unsigned nj);

  //: Define region size in world co-ordinates
  //  Sets up ROI to cover given box (with samples at step_size()),
  //  with ref point at centre.
  //  Currently just defines as a box.
  virtual void set_region_size(double wi, double wj);

  //: Kernel mask is ni x nj
  unsigned ni() const { return ni_; }

  //: Kernel mask is ni x nj
  unsigned nj() const { return nj_; }

  //: Number of dimensions in the model
  virtual unsigned model_dim();

  //: Create new mfpf_grad_corr2d on heap
  virtual mfpf_point_finder* new_finder() const;

  //: Initialise building
  // Must be called before any calls to add_example(...)
  virtual void clear(unsigned n_egs);

  //: Get sample of region around specified point in image
  virtual void get_sample_vector(const vimt_image_2d_of<float>& image,
                                 const vgl_point_2d<double>& p,
                                 const vgl_vector_2d<double>& u,
                                 vcl_vector<double>& v);

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

  //: Version number for I/O
  short version_no() const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#endif
