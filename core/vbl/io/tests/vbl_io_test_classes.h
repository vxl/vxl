#ifndef vbl_io_test_classes_h_
#define vbl_io_test_classes_h_

#include <vbl/vbl_ref_count.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vsl/vsl_binary_io.h>

class impl : public vbl_ref_count
{
 public:
  static int reftotal;
  int n;

  impl(int nn);
  impl(impl const& x) : vbl_ref_count(), n(x.n) {}
  impl();
  ~impl();
  void Print (vcl_ostream &str);
  static void checkcount ();
};

void vsl_b_read(vsl_b_istream&, impl &);
void vsl_b_write(vsl_b_ostream&, const impl &);
void vsl_print_summary(vcl_ostream&, const impl &);
void vsl_b_read(vsl_b_istream& is, impl * &);
void vsl_b_write(vsl_b_ostream& os, const impl *);
void vsl_print_summary(vcl_ostream&, const impl *);

#endif // vbl_io_test_classes_h_
