// fsm: sunpro needs template instantiations to go into Templates
#include "vbl_smart_ptr_example.h"
#include <vcl/vcl_list.h>

void main1()
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

//--------------------------------------------------------------------------------

void main2() {
  bigmatrix A, B, C;

  cerr << "one million swaps..." << flush;
  for (unsigned i=0; i<1000000; ++i) {
    C = A;
    A = B;
    B = C;
  }
  cerr << "done" << endl;
}

//--------------------------------------------------------------------------------

int main() { main1(); main2(); }
