// This is core/vbl/vbl_bool_ostream.h
#ifndef vbl_bool_ostream_h_
#define vbl_bool_ostream_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
//
// \verbatim
//  Modifications
//   PDA (Manchester) 21/03/2001: Tidied up the documentation
// \endverbatim

#include <vcl_iosfwd.h>

class vbl_bool_ostream
{
 public:
  class on_off
  {
   public:
    on_off(const bool &val) : truth(&val) {}
    const bool* truth;
  };

  class high_low
  {
   public:
    high_low(const bool &val) : truth(&val) {}
    const bool* truth;
  };

  class true_false
  {
   public:
    true_false(const bool &val) : truth(&val) {}
    const bool* truth;
  };
};

vcl_ostream& operator<<(vcl_ostream& s,
                        const vbl_bool_ostream::on_off& proxy);
vcl_ostream& operator<<(vcl_ostream& s,
                        const vbl_bool_ostream::high_low& proxy);
vcl_ostream& operator<<(vcl_ostream& s,
                        const vbl_bool_ostream::true_false& proxy);

#endif // vbl_bool_ostream_h_
