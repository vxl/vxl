#ifndef vbl_test_smart_ptr_h_
#define vbl_test_smart_ptr_h_

// .NAME vbl_test_classes
// .LIBRARY vbl-test
// .INCLUDE vbl/tests/vbl_test_classes.h
// .FILE vbl_test_classes.cxx

#include <vcl_iostream.h>
#include <vcl_list.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

class base_impl : public vbl_ref_count  {
public :
  static int reftotal;
  int n;
 
  base_impl(int nn);
  base_impl();
  ~base_impl();
  void Print (vcl_ostream &str);
  static void checkcount ();
};

//-------------------------------------------------------
 
class derived_impl : public base_impl
{
   public : int k;
};

//======================================================================

typedef vbl_smart_ptr<base_impl> base_sptr;
typedef vbl_smart_ptr<derived_impl> derived_sptr;

#endif // vbl_test_smart_ptr_h_
