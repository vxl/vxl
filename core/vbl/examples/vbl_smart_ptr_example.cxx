// This is ./vxl/vbl/examples/vbl_smart_ptr_example.cxx
#include <vcl_iostream.h>
#include <vcl_list.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

//:
// \file
//
// The code parts below are normally put into several files, as indicated by
// the "start of" and "end of" lines.  Here, for this example, everything is
// put in a single file, which is a perfectly valid (but less standard) way.

//== start of example_sp.h ==//

//: An example of how to make a smart pointer class.
// A smart pointer can be made from any class that defines the methods ref()
// and unref(), e.g., because it is derived from vbl_ref_count.
class example_sp : public vbl_ref_count {
public:
  example_sp() { vcl_cout << "example_sp constructor, refcount=" 
                          << get_references() << vcl_endl; }

 ~example_sp() { vcl_cout << "example_sp destructor, refcount=" 
                          << get_references() << vcl_endl; }

  example_sp(example_sp const&) {
                 vcl_cout << "example_sp copy constructor, refcount="
                          << get_references() << vcl_endl; }

  friend vcl_ostream& operator<<(vcl_ostream& os, example_sp const& e) {
    int p = e.get_references(); 
    if (p < 1000) os << "example_sp, refcount=" << p;
    else          os << "example_sp, invalid";
    return os;
  }
};

typedef vbl_smart_ptr<example_sp> example_sp_sptr;

//== end of example_sp.h ==//

//== start of first main program ==//

void main1()
{
  vcl_list<example_sp_sptr> l;

  vcl_cout << "example_sp starts" << vcl_endl;
  example_sp* ptr;
  {
    example_sp_sptr sp; // refcount not incremented: no assignment yet
    vcl_cout << "example_sp_sptr created" << vcl_endl;
    {
      ptr = new example_sp; // refcount not incremented: no smart pointer
      vcl_cout << *ptr << " created" << vcl_endl;

      sp = ptr; // refcount incremented: assignment to smart pointer
      vcl_cout << *sp << " assigned" << vcl_endl;

      l.push_back(sp); // refcount incremented (assignment to list entry)
      vcl_cout << *sp << " put on list" << vcl_endl;

      example_sp_sptr sp2 = sp; // copy constructor: refcount incremented
      vcl_cout << *sp << " copied to sp2" << vcl_endl;

    } // sp2 goes out of scope: refcount goes down
    vcl_cout << "Copy of " << *sp << " is now out of scope" << vcl_endl;

  } // sp goes out of scope: refcount goes down
  vcl_cout << "Smart pointer of " << *ptr << " is now out of scope" << vcl_endl;

  vcl_cout << "Clearing list" << vcl_endl;
  l.pop_back(); // sp removed from list: refcount goes down to 0, destructor called

  // So at this point, ptr points to deallocated memory...
  vcl_cout << "List copy of " << *ptr << " has been removed, ptr freed" << vcl_endl;
}

//== end of first main program ==//

//--------------------------------------------------------------------------------

//== start of bigmatrix.h ==//

class bigmatrix_impl : public vbl_ref_count {
public:
  double data[256][256];
  bigmatrix_impl() { vcl_cout << "bigmatrix_impl ctor" << vcl_endl; }
  ~bigmatrix_impl() { vcl_cout << "bigmatrix_impl dtor" << vcl_endl; }
};

class bigmatrix {
public:
  double * operator[](unsigned i) { return impl->data[i]; }
private:
  vbl_smart_ptr<bigmatrix_impl> impl;
};

//== end of bigmatrix.h ==//

//== start of second main program ==//

void main2()
{
  bigmatrix A, B, C;

  vcl_cout << "one million swaps..." << vcl_flush;
  for (unsigned i=0; i<1000000; ++i) {
    C = A;
    A = B;
    B = C;
  }
  vcl_cout << "done" << vcl_endl;
}

//== end of second main program ==//

//--------------------------------------------------------------------------------

int main() { main1(); main2(); return 0; }
// fsm: sunpro needs template instantiations to go into Templates
