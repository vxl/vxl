#include "vbl_test_classes.h"

// Remember, the template instances must be in 
// Templates, but they use these classes, so the class
// implementation must be in the library even if only
// one test uses it.

int base_impl::reftotal = 0;

base_impl::base_impl(int nn) : n(nn) {
  reftotal++;
  cout <<  "base_impl ctor : this=" << (void*)this << endl;
}

base_impl::base_impl() : n(7) {
  reftotal++;
  cout <<  "base_impl ctor : this=" << (void*)this << endl;
}

base_impl::~base_impl() {
  reftotal--;
  cout <<  "base_impl dtor : this=" << (void*)this << endl;
}

void base_impl::Print (ostream &str) {
  str << "base_impl(" << n << ") ";
}
  
void base_impl::checkcount () {
  if (reftotal == 0)
    cout << "base_impl : PASSED" << endl;
  else
    cout << "base_impl : FAILED : count = " << reftotal << endl;
} 

