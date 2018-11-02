#ifndef vbl_io_test_classes_h_
#define vbl_io_test_classes_h_

#include <iosfwd>
#include <vbl/vbl_ref_count.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class impl : public vbl_ref_count
{
 public:
  static int reftotal;
  int n;

  explicit impl(int nn);
  impl(impl const& x) : vbl_ref_count(x), n(x.n) {}
  impl();
  ~impl() override;
  void Print (std::ostream &str);
  static void checkcount ();
};

void vsl_b_read(vsl_b_istream&, impl &);
void vsl_b_write(vsl_b_ostream&, const impl &);
void vsl_print_summary(std::ostream&, const impl &);
void vsl_b_read(vsl_b_istream& is, impl * &);
void vsl_b_write(vsl_b_ostream& os, const impl *);
void vsl_print_summary(std::ostream&, const impl *);

#endif // vbl_io_test_classes_h_
