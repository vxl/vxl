//-*- c++ -*-------------------------------------------------------------------
#ifndef vbl_redirector_h_
#define vbl_redirector_h_
#ifdef __GNUC__
#pragma interface "vbl_redirector"
#endif
// .SECTION Author
//    awf@robots.ox.ac.uk
// Created: 05 Jul 00
// .SECTION Modifications:
//   21-JUL-00  M.Bacic, Oxford RRG -- fixed 'pubsync' on gcc. 
//               See vbl_redirector.cxx 
#include <vcl/vcl_iostream.h>

struct vbl_redirector_data;

//: Simplified redirection of iostreams..
// To implement your own, derive a class from vbl_redirector,
// and implement `putchunk'.
class vbl_redirector {
public:

  //: Attach redirector to ostream s, 
  // so that all future output to s goes through this->putchunk
   vbl_redirector(vcl_ostream& s);

  //: Destroy redirector, restore stream to original.
   virtual ~vbl_redirector();

  //: The filter function
  // Called with `n' characters in `buf', do with as you like.
   virtual int putchunk(char const* buf, int n);

protected:
  //: Put characters to original stream.
  // Useful for derived classes which wish to filter a stream.
   int put_passthru(char const* buf, int n);

  //: Sync original stream.
   int sync_passthru();

private:
  vbl_redirector_data* p;
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vbl_redirector.

