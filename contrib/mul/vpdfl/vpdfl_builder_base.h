// This is mul/vpdfl/vpdfl_builder_base.h
#ifndef vpdfl_builder_base_h
#define vpdfl_builder_base_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Tim Cootes
// \date 12-Apr-2001
// \brief Base for classes to build vpdfl_pdf_base objects.


#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_memory.h>
#include <vnl/vnl_vector.h>
#include <vsl/vsl_binary_io.h>
#include <mbl/mbl_data_wrapper.h>

//=======================================================================

class vpdfl_pdf_base;


//: Base for classes to build vpdfl_pdf_base objects
//
// Design Notes:
//
// Models are passed to builder, rather than being created
// by builder, for efficiency when used in mixture models.
// (During building they'd have to be recreated at every
// iteration, which is expensive).
class vpdfl_builder_base
{
 public:
  //: Dflt ctor
  vpdfl_builder_base() {}

  //: Destructor
  virtual ~vpdfl_builder_base() {}

  //: Create empty model
  virtual vpdfl_pdf_base* new_model() const = 0;

  //: Define lower threshold on variance for built models
  virtual void set_min_var(double min_var) =0;

  //: Get lower threshold on variance for built models
  virtual double min_var() const =0;

  //: Build default model with given mean
  virtual void build(vpdfl_pdf_base& model,
                     const vnl_vector<double>& mean) const = 0;

  //: Build model from data
  virtual void build(vpdfl_pdf_base& model,
                     mbl_data_wrapper<vnl_vector<double> >& data) const = 0;

  //: Build model from weighted data
  virtual void weighted_build(vpdfl_pdf_base& model,
                              mbl_data_wrapper<vnl_vector<double> >& data,
                              const vcl_vector<double>& wts) const = 0;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual vpdfl_builder_base* clone() const = 0;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const = 0;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const = 0;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs) = 0;

  //: Create a vpdfl_builder_base object given a config steram
  // \throw mbl_exception if parse error.
  static vcl_auto_ptr<vpdfl_builder_base> new_builder_from_stream(vcl_istream &is);
};

//: Allows derived class to be loaded by base-class pointer
//  A loader object exists which is invoked by calls
//  of the form "bfs>>base_ptr;".  This loads derived class
//  objects from the disk, places them on the heap and
//  returns a base class pointer.
//  In order to work the loader object requires
//  an instance of each derived class that might be
//  found.  This function gives the model class to
//  the appropriate loader.
void vsl_add_to_binary_loader(const vpdfl_builder_base& b);

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const vpdfl_builder_base& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, vpdfl_builder_base& b);

//: Stream output operator for class reference
void vsl_print_summary(vcl_ostream& os,const vpdfl_builder_base& b);

//: Stream output operator for class pointer
void vsl_print_summary(vcl_ostream& os,const vpdfl_builder_base* b);

//: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const vpdfl_builder_base& b);

//: Stream output operator for class pointer
vcl_ostream& operator<<(vcl_ostream& os,const vpdfl_builder_base* b);

#endif // vpdfl_builder_base_h
