#ifndef vbl_smart_ptr_example_h_
#define vbl_smart_ptr_example_h_

#include <vcl_iostream.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

// The code parts below are normally put into several files, as indicated by
// the "start of" and "end of" lines.  Here, for this example, everything is
// put in a single file, which is a perfectly valid (but less standard) way.

//: An example of how to make a smart pointer class.
// A smart pointer can be made from any class that defines the methods ref()
// and unref(), e.g., because it is derived from vbl_ref_count.
class example_sp : public vbl_ref_count {
public:
  example_sp() { vcl_cout << "example_sp constructor, refcount=" 
    << get_references() << vcl_endl; }

  ~example_sp() { vcl_cout << "example_sp destructor, refcount=" 
    << get_references() << vcl_endl; }

  example_sp(example_sp const&) { vcl_cout << "example_sp copy constructor, "
    "refcount=" << get_references() << vcl_endl; }

  friend vcl_ostream& operator<<(vcl_ostream& os, example_sp const& e) {
    int p = e.get_references(); 
    if (p < 1000) os << "example_sp, refcount=" << p;
    else          os << "example_sp, invalid";
    return os;
  }
};
typedef vbl_smart_ptr<example_sp> example_sp_sptr;

// Here is another example.

// class bigmatrix
class bigmatrix_impl : public vbl_ref_count {
public:
  double data[256][256];
  bigmatrix_impl() { vcl_cerr << "bigmatrix_impl ctor" << vcl_endl; }
  ~bigmatrix_impl() { vcl_cerr << "bigmatrix_impl dtor" << vcl_endl; }
};

class bigmatrix {
public:
  double * operator[](unsigned i) { return impl->data[i]; }
private:
  vbl_smart_ptr<bigmatrix_impl> impl;
};

#endif // vbl_smart_ptr_example_h_
