#ifndef vbl_bool_ostream_h_
#define vbl_bool_ostream_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vbl_bool_ostream
// .INCLUDE vbl/vbl_bool_ostream.h
// .FILE vbl_bool_ostream.cxx

#include <vcl/vcl_iostream.h>

class vbl_bool_ostream {
public:
  class on_off {
  public:
    on_off(const bool &val) : truth(&val) {}
    const bool* truth; 
  };

  class high_low {
  public:
    high_low(const bool &val) : truth(&val) {}
    const bool* truth; 
  };

  class true_false {
  public:
    true_false(const bool &val) : truth(&val) {}
    const bool* truth; 
  };
};

ostream& operator<<(ostream& s, const vbl_bool_ostream::on_off& proxy);
ostream& operator<<(ostream& s, const vbl_bool_ostream::high_low& proxy);
ostream& operator<<(ostream& s, const vbl_bool_ostream::true_false& proxy);


#endif // vbl_bool_ostream_h_
