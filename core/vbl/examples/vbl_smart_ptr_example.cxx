// This is core/vbl/examples/vbl_smart_ptr_example.cxx

//:
// \file

#include <vcl_iostream.h>
#include <vcl_list.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

// The code parts below are normally put into several files, as indicated by
// the "start of" and "end of" lines.  Here, for this example, everything is
// put in a single file, which is a perfectly valid (but less standard) way.

//== start of example_sp.h ==//

//: An example of how to make a smart pointer class.
// A smart pointer can be made from any class that defines the methods ref()
// and unref(), e.g., because it is derived from vbl_ref_count.
class example_sp : public vbl_ref_count
{
 public:
  example_sp() { vcl_cout << "example_sp constructor, refcount=" << get_references() << '\n'; }

 ~example_sp() { vcl_cout << "example_sp destructor, refcount=" << get_references() << '\n'; }

  example_sp(example_sp const&) : vbl_ref_count()
  {
    vcl_cout << "example_sp copy constructor, refcount=" << get_references() << '\n';
  }

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

  vcl_cout << "example_sp starts\n";
  example_sp* ptr;
  {
    example_sp_sptr sp; // refcount not incremented: no assignment yet
    vcl_cout << "example_sp_sptr created\n";
    {
      ptr = new example_sp; // refcount not incremented: no smart pointer
      vcl_cout << *ptr << " created\n";

      sp = ptr; // refcount incremented: assignment to smart pointer
      vcl_cout << *sp << " assigned\n";

      l.push_back(sp); // refcount incremented (assignment to list entry)
      vcl_cout << *sp << " put on list\n";

      example_sp_sptr sp2 = sp; // copy constructor: refcount incremented
      vcl_cout << *sp << " copied to sp2" << *sp2 << '\n';

    } // sp2 goes out of scope: refcount goes down
    vcl_cout << "Copy of " << *sp << " is now out of scope\n";

  } // sp goes out of scope: refcount goes down
  vcl_cout << "Smart pointer of " << *ptr << " is now out of scope\n";

  vcl_cout << "Clearing list\n";
  l.pop_back(); // sp removed from list: refcount goes down to 0, destructor called

  // So at this point, ptr points to deallocated memory...
  vcl_cout << "List copy of " << *ptr << " has been removed, ptr freed\n";
}

//== end of first main program ==//

//--------------------------------------------------------------------------------

//== start of bigmatrix.h ==//

class bigmatrix_impl : public vbl_ref_count
{
 public:
  double data[256][256];
  bigmatrix_impl() { vcl_cout << "bigmatrix_impl ctor\n"; }
  ~bigmatrix_impl() { vcl_cout << "bigmatrix_impl dtor\n"; }
};

class bigmatrix
{
  vbl_smart_ptr<bigmatrix_impl> impl;
 public:
  double * operator[](unsigned i) { return impl->data[i]; }
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
  vcl_cout << "done\n";
}

//== end of second main program ==//

//--------------------------------------------------------------------------------

int main() { main1(); main2(); return 0; }

#include <vbl/vbl_smart_ptr.txx>
#include <vcl_list.txx>
#include <vcl_algorithm.txx>

VBL_SMART_PTR_INSTANTIATE(example_sp);
VBL_SMART_PTR_INSTANTIATE(bigmatrix_impl);

VCL_OPERATOR_NE_INSTANTIATE(vbl_smart_ptr<example_sp>);
VCL_CONTAINABLE_INSTANTIATE(vbl_smart_ptr<example_sp>);
VCL_LIST_INSTANTIATE(vbl_smart_ptr<example_sp>);
