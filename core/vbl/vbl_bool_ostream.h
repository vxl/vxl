// This is core/vbl/vbl_bool_ostream.h
#ifndef vbl_bool_ostream_h_
#define vbl_bool_ostream_h_
//:
// \file
//
// \verbatim
//  Modifications
//   PDA (Manchester) 21/03/2001: Tidied up the documentation
// \endverbatim

#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

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

std::ostream& operator<<(std::ostream& s,
                        const vbl_bool_ostream::on_off& proxy);
std::ostream& operator<<(std::ostream& s,
                        const vbl_bool_ostream::high_low& proxy);
std::ostream& operator<<(std::ostream& s,
                        const vbl_bool_ostream::true_false& proxy);

#endif // vbl_bool_ostream_h_
