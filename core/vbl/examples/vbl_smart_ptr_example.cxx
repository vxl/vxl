// <begin copyright notice>
//--------------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1999 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//                               
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//       
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>

// Here is an example of how to make a smart pointer class.
// A smart pointer can be made from any class that defines the methods ref()
// and unref(), e.g., because it subclasses off vbl_ref_count.

// The code parts below are normally put into several files, as indicated by
// the "start of" and "end of" lines.  Here, for this example, everything is
// put in a single file, which is a perfectly valid (but less standard) way.

// start of example_sp.h 
#include <vcl/vcl_iostream.h>
#include <vbl/vbl_ref_count.h>

class example_sp : public vbl_ref_count {
public:
  example_sp() { cout << "example_sp constructor, refcount=" << get_references() << endl; }

  ~example_sp() { cout << "example_sp destructor, refcount=" << get_references() << endl; }

  example_sp(example_sp const&) { cout << "example_sp copy constructor, refcount=" << get_references() << endl; }

  friend ostream& operator<<(ostream& os, example_sp const& e) {
    int p = e.get_references(); 
    if (p < 1000) os << "example_sp, refcount=" << p;
    else          os << "example_sp, invalid";
    return os;
  }
};
// end of example_sp.h 

//--------------------------------------------------------------------------------
// start of list+vbl_smart_ptr+example_sp--.cxx
#include <vbl/vbl_smart_ptr.txx>
#include <vcl/vcl_compiler.h>
#include <vcl/vcl_list.txx>
#include <vcl/vcl_algorithm.txx>

VCL_OPERATOR_NE_INSTANTIATE(vbl_smart_ptr<example_sp>);
VCL_CONTAINABLE_INSTANTIATE(vbl_smart_ptr<example_sp>);
VCL_LIST_INSTANTIATE(vbl_smart_ptr<example_sp>);

// end of list+vbl_smart_ptr+example_sp-.cxx
//--------------------------------------------------------------------------------

// start of example_sp_ref.h 
#include <vbl/vbl_smart_ptr.h>
class example_sp;
typedef vbl_smart_ptr<example_sp> example_sp_ref;
// end of example_sp_ref.h 
//--------------------------------------------------------------------------------

// To instantiate create a .cxx file in the Templates directory of
// the class you want the smart pointer of.  In that file do this:

// start of vbl_smart_ptr+example_sp-.cxx

VBL_SMART_PTR_INSTANTIATE(example_sp);
// end of vbl_smart_ptr+example_sp-.cxx

#include <vcl/vcl_list.h>

int main()
{
  vcl_list<example_sp_ref> l;

  cout << "example_sp starts" << endl;
  example_sp* ptr;
  {
    example_sp_ref sp; // refcount not incremented: no assignment yet
    cout << "example_sp_ref created" << endl;
    {
      ptr = new example_sp; // refcount not incremented: no smart pointer
      cout << *ptr << " created" << endl;

      sp = ptr; // refcount incremented: assignment to smart pointer
      cout << *sp << " assigned" << endl;

      l.push_back(sp); // refcount incremented (assignment to list entry)
      cout << *sp << " put on list" << endl;

      example_sp_ref sp2 = sp; // copy constructor: refcount incremented
      cout << *sp << " copied to sp2" << endl;

    } // sp2 goes out of scope: refcount goes down
    cout << "Copy of " << *sp << " is now out of scope" << endl;

  } // sp goes out of scope: refcount goes down
  cout << "Smart pointer of " << *ptr << " is now out of scope" << endl;

  cout << "Clearing list" << endl;
  l.pop_back(); // sp removed from list: refcount goes down to 0, destructor called

  // So at this point, ptr points to deallocated memory...
  cout << "List copy of " << *ptr << " has been removed, ptr freed" << endl;
}
