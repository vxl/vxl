// fsm: sunpro needs template instantiations to go into Templates
#include "vbl_smart_ptr_example.h"
#include <vcl_iostream.h>
#include <vcl_list.h>

void main1()
{
  vcl_list<example_sp_ref> l;

  vcl_cout << "example_sp starts" << vcl_endl;
  example_sp* ptr;
  {
    example_sp_ref sp; // refcount not incremented: no assignment yet
    vcl_cout << "example_sp_ref created" << vcl_endl;
    {
      ptr = new example_sp; // refcount not incremented: no smart pointer
      vcl_cout << *ptr << " created" << vcl_endl;

      sp = ptr; // refcount incremented: assignment to smart pointer
      vcl_cout << *sp << " assigned" << vcl_endl;

      l.push_back(sp); // refcount incremented (assignment to list entry)
      vcl_cout << *sp << " put on list" << vcl_endl;

      example_sp_ref sp2 = sp; // copy constructor: refcount incremented
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

//--------------------------------------------------------------------------------

void main2() {
  bigmatrix A, B, C;

  vcl_cerr << "one million swaps..." << vcl_flush;
  for (unsigned i=0; i<1000000; ++i) {
    C = A;
    A = B;
    B = C;
  }
  vcl_cerr << "done" << vcl_endl;
}

//--------------------------------------------------------------------------------

int main() { main1(); main2(); }
