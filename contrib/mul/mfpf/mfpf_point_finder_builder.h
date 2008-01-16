#ifndef  mfpf_point_finder_builder_h_
#define mfpf_point_finder_builder_h_

//:
// \file
// \brief Base for classes which build mfpf_point_finder objects.
// \author Tim Cootes

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vsl/vsl_binary_io.h>
#include <vcl_cassert.h>
#include <vnl/vnl_vector.h>
#include <vcl_memory.h>

#include <vimt/vimt_image_2d_of.h>
#include <vgl/vgl_vector_2d.h>

class mfpf_point_finder;

//: Base for classes which build mfpf_point_finder objects.
class mfpf_point_finder_builder
{
protected:
public:

    //: Dflt ctor
  mfpf_point_finder_builder();

    //: Destructor
  virtual ~mfpf_point_finder_builder();

  //: Size of step between sample points
  virtual void set_step_size(double)=0;

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
  virtual void print_summary(vcl_ostream& os) const =0;

    //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const =0;

    //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs) =0;

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

#endif


