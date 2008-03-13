#ifndef mfpf_point_finder_builder_h_
#define mfpf_point_finder_builder_h_
//:
// \file
// \brief Base for classes which build mfpf_point_finder objects.
// \author Tim Cootes

#include <vcl_string.h>
#include <vsl/vsl_binary_io.h>
#include <vcl_memory.h>

#include <vimt/vimt_image_2d_of.h>
#include <vgl/vgl_vector_2d.h>

class mfpf_point_finder;
class mbl_read_props_type;

//: Base for classes which build mfpf_point_finder objects.
class mfpf_point_finder_builder
{
 protected:
  //: Size of step between sample points
  double step_size_;

  //: Number of points either side of centre to search
  int search_ni_;

  //: Number of points either side of centre to search
  int search_nj_;

  //: Define N. angles (ie try at A+idA, i in [-nA,+nA]) 
  unsigned search_nA_;

  //: Angle step size (ie try at A+idA, i in [-nA,+nA])
  double search_dA_;

  //: Number of scales to try at
  unsigned search_ns_;

  //: Scaling factor (ie try at ((ds)^i), i in [-ns,+ns]
  double search_ds_;

  //: Return true if base class parameters are the same in b
  bool base_equality(const mfpf_point_finder_builder& b) const;

  //: Parse relevant parameters from props list
  void parse_base_props(mbl_read_props_type& props);

  //: Set base-class parameters of point finder
  void set_base_parameters(mfpf_point_finder& pf);

 public:

  //: Dflt ctor
  mfpf_point_finder_builder();

  //: Destructor
  virtual ~mfpf_point_finder_builder();

  //: Size of step between sample points
  virtual void set_step_size(double);

  //: Size of step between sample points
  double step_size() const { return step_size_; }

  //: Define search region size
  //  During search, samples at points on grid [-ni,ni]x[-nj,nj],
  //  with axes defined by u.
  virtual void set_search_area(unsigned ni, unsigned nj);

  //: Define angle search parameters
  void set_search_angle_range(unsigned nA, double dA);

  //: Define scale search parameters
  void set_search_scale_range(unsigned ns, double ds);

  int search_ni() const { return search_ni_; }
  int search_nj() const { return search_nj_; }

  //: Create new finder of appropriate type on heap
  virtual mfpf_point_finder* new_finder() const =0;

  //: Initialise building
  // Must be called before any calls to add_example(...)
  virtual void clear(unsigned n_egs)=0;

  //: Add one example to the model
  virtual void add_example(const vimt_image_2d_of<float>& image,
                           const vgl_point_2d<double>& p,
                           const vgl_vector_2d<double>& u)=0;

  //: Build object from the data supplied in add_example()
  virtual void build(mfpf_point_finder&)=0;

  //: Initialise from a string stream
  virtual bool set_from_stream(vcl_istream &is);

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Create a copy on the heap and return base class pointer
  virtual mfpf_point_finder_builder* clone() const = 0;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const ;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);

  //: Create a concrete object, from a text specification.
  static vcl_auto_ptr<mfpf_point_finder_builder> create_from_stream(vcl_istream &is);
};

//: Allows derived class to be loaded by base-class pointer
void vsl_add_to_binary_loader(const mfpf_point_finder_builder& b);

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mfpf_point_finder_builder& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mfpf_point_finder_builder& b);

//: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const mfpf_point_finder_builder& b);

//: Stream output operator for class pointer
vcl_ostream& operator<<(vcl_ostream& os,const mfpf_point_finder_builder* b);

#endif // mfpf_point_finder_builder_h_
