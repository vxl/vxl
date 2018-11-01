// This is core/vul/vul_redirector.h
#ifndef vul_redirector_h_
#define vul_redirector_h_
//:
// \file
// \brief Contains class for simplified redirection of iostreams
// \author awf@robots.ox.ac.uk
//
// \verbatim
//  Modifications
//   21-JUL-00  M.Bacic, Oxford RRG -- fixed 'pubsync' on gcc.
//   PDA (Manchester) 21/03/2001: Tidied up the documentation
//   Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
// \endverbatim

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

struct vul_redirector_data;

//: Simplified redirection of iostreams
// To implement your own, derive a class from vul_redirector,
// and implement `putchunk'.
class vul_redirector
{
 public:
  //: Attach redirector to std::ostream s, so that all future output to s goes through this->putchunk
  vul_redirector(std::ostream& s);

  //: Destroy redirector, restore stream to original.
  virtual ~vul_redirector();

  //: The filter function
  // Called with `n' characters in `buf', do with as you like.
  virtual std::streamsize putchunk(char const* buf, std::streamsize n);

 protected:
  //: Put characters to original stream.
  // Useful for derived classes which wish to filter a stream.
  std::streamsize put_passthru(char const* buf, std::streamsize n);

  //: Sync original stream.
  int sync_passthru();

 private:
  vul_redirector_data* p;
};

#endif // vul_redirector_h_
