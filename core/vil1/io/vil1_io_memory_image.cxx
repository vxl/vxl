// This is core/vil1/io/vil1_io_memory_image.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief Perform serialised binary IO for vil1_memory_image
// \author Ian Scott (Manchester)
// \date 26-Mar-2001

#include "vil1_io_memory_image.h"
#include <vil1/io/vil1_io_image.h>
#include <vil1/vil1_memory_image.h>
#include <vsl/vsl_clipon_binary_loader.txx>
#include <vil1/io/vil1_io_image_impl.h>

//: Binary write to stream.
void vsl_b_write(vsl_b_ostream &os, const vil1_memory_image & p)
{
  // write version number
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, (const vil1_image &) p);
}

//========================================================================
//: Binary load from stream.
void vsl_b_read(vsl_b_istream &is, vil1_memory_image & p)
{
  if (!is) return;

  short v;
  vsl_b_read(is, v);
  switch(v)
  {
   case 1:
    vsl_b_read(is, (vil1_image &) p);
    p.recache_from_impl();
    break;

   default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vil1_memory_image&)\n"
             << "           Unknown version number "<< v << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//========================================================================
//: Output a human readable summary to the stream
void vsl_print_summary(vcl_ostream& os,const vil1_memory_image & p)
{
    vsl_print_summary(os, p.impl());
}
