#ifndef vbl_test_smart_ptr_h_
#define vbl_test_smart_ptr_h_

#include <vcl/vcl_iostream.h>
#include <vcl/vcl_list.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

class base_impl : public vbl_ref_count  {
public :
  static int reftotal;
  int n;
 
  base_impl(int nn);
  base_impl();
  ~base_impl();
  void Print (ostream &str);
  static void checkcount ();
};

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

//-------------------------------------------------------
 
class derived_impl : public base_impl
{
   public : int k;
};

//======================================================================

typedef vbl_smart_ptr<base_impl> base_ref;
typedef vbl_smart_ptr<derived_impl> derived_ref;

#endif
