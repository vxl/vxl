#ifndef mfpf_norm_corr1d_builder_h_
#define mfpf_norm_corr1d_builder_h_
//:
// \file
// \brief Builder for mfpf_norm_corr1d objects.
// \author Tim Cootes

#include <iostream>
#include <iosfwd>
#include <mfpf/mfpf_point_finder_builder.h>
#include <vnl/vnl_vector.h>
#include <vgl/vgl_fwd.h>
#include <vcl_compiler.h>

//: Builder for mfpf_norm_corr1d objects.
class mfpf_norm_corr1d_builder : public mfpf_point_finder_builder
{
 private:
  //: Kernel mask is [ilo_,ihi_]
  int ilo_;
  //: Kernel mask is [ilo_,ihi_]
  int ihi_;

  //: Workspace for sum
  vnl_vector<double> sum_;

  //: Number of examples added
  unsigned n_added_;

  //: Define default values
  void set_defaults();
 public:

  // Dflt ctor
  mfpf_norm_corr1d_builder();

  // Destructor
  virtual ~mfpf_norm_corr1d_builder();

  void set_kernel_size(int ilo, int ihi);

  //: Define region size in world co-ordinates
  //  Sets up ROI to cover given box (with samples at step_size()),
  //  with ref point at centre.
  //  Sets ihi = ceil(wi/2*step_size), ilo=-ihi
  virtual void set_region_size(double wi, double wj);


  //: Kernel mask is [ilo_,ihi_]
  int ilo() const { return ilo_; }

  //: Kernel mask is [ilo_,ihi_]
  int ihi() const { return ihi_; }

  //: Number of dimensions in the model
  virtual unsigned model_dim();

  //: Create new mfpf_norm_corr1d on heap
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
  virtual bool set_from_stream(std::istream &is);

  //: Name of the class
  virtual std::string is_a() const;

  //: Create a copy on the heap and return base class pointer
  virtual mfpf_point_finder_builder* clone() const;

  //: Print class to os
  virtual void print_summary(std::ostream& os) const;

  //: Version number for I/O
  short version_no() const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#endif
