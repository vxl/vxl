// This is mul/mbl/mbl_data_collector_base.h
#ifndef mbl_data_collector_base_h_
#define mbl_data_collector_base_h_
//:
// \file
// \author Tim Cootes
// \brief Non-templated base class for mbl_data_collector<T>

#include <string>
#include <iostream>
#include <iosfwd>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Non-templated base class for mbl_data_collector<T>
// Provided mainly because the Visual C++ compiler can't cope with
// the double template instantiation for the binary IO
class mbl_data_collector_base
{
 public:

  //: Dflt ctor
  mbl_data_collector_base();

  //: Destructor
  virtual ~mbl_data_collector_base();

  //: Name of the class
  virtual std::string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(std::string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual mbl_data_collector_base* clone() const = 0;

  //: Print class to os
  virtual void print_summary(std::ostream& os) const = 0;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const = 0;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs) = 0;
};

  //: Allows derived class to be loaded by base-class pointer
  //  A loader object exists which is invoked by calls
  //  of the form "vsl_b_read(bfs,base_ptr);".  This loads derived class
  //  objects from the disk, places them on the heap and
  //  returns a base class pointer.
  //  In order to work the loader object requires
  //  an instance of each derived class that might be
  //  found.  This function gives the model class to
  //  the appropriate loader.
void vsl_add_to_binary_loader(const mbl_data_collector_base& b);

  //: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mbl_data_collector_base& b);

  //: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mbl_data_collector_base& b);

  //: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const mbl_data_collector_base& b);

  //: Stream output operator for class pointer
std::ostream& operator<<(std::ostream& os,const mbl_data_collector_base* b);

#endif // mbl_data_collector_base_h_
