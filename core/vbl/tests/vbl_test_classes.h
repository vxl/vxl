#ifndef vbl_test_smart_ptr_h_
#define vbl_test_smart_ptr_h_

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vcl_iosfwd.h>

class base_impl : public vbl_ref_count
{
 public:
  static int reftotal;
  int n;

  base_impl(int nn);
  base_impl();
  base_impl(base_impl const& x) : vbl_ref_count(), n(x.n) {}
  ~base_impl();
  void Print (vcl_ostream &str);
  static bool checkcount ( int count = 0 );
};

//-------------------------------------------------------

class derived_impl : public base_impl
{
 public:
  int k;
};

//======================================================================

typedef vbl_smart_ptr<base_impl> base_sptr;
typedef vbl_smart_ptr<derived_impl> derived_sptr;

#endif // vbl_test_smart_ptr_h_
