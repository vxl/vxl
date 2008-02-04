#ifndef mfpf_norm_corr2d_builder_h_
#define mfpf_norm_corr2d_builder_h_
//:
// \file
// \brief Builder for mfpf_norm_corr2d objects.
// \author Tim Cootes

#include <mfpf/mfpf_point_finder_builder.h>
#include <vcl_iosfwd.h>

//: Builder for mfpf_norm_corr2d objects.
class mfpf_norm_corr2d_builder : public mfpf_point_finder_builder
{
 private:
  //: Size of step between sample points
  double step_size_;

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

  //: Workspace for sum
  vil_image_view<double> sum_;

  //: Number of examples added
  unsigned n_added_;

  //: Number of points either side of centre to search
  int search_ni_;

  //: Number of points either side of centre to search
  int search_nj_;

  //: Define default values
  void set_defaults();

  //: Add one example to the model
  void add_one_example(const vimt_image_2d_of<float>& image,
                       const vgl_point_2d<double>& p,
                       const vgl_vector_2d<double>& u);

 public:

  // Dflt ctor
  mfpf_norm_corr2d_builder();

  // Destructor
  virtual ~mfpf_norm_corr2d_builder();

  //: Size of step between sample points
  virtual void set_step_size(double);

  //: Define size of mask
  void set_kernel_size(unsigned ni, unsigned nj,
                       double ref_x, double ref_y);

  //: Define size of mask
  // Ref point is centre
  void set_kernel_size(unsigned ni, unsigned nj);

  //: Kernel mask is ni x nj
  unsigned ni() const { return ni_; }

  //: Kernel mask is ni x nj
  unsigned nj() const { return nj_; }

  int search_ni() const { return search_ni_; }
  int search_nj() const { return search_nj_; }

  //: Create new mfpf_norm_corr2d on heap
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

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#endif
