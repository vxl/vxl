// This is core/vil1/io/vil1_io_image.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief Perform serialised binary IO for vil1_image
// \author Ian Scott (Manchester)
// \date 23-Mar-2001

#include "vil1_io_image.h"
#include <vil1/io/vil1_io_image_impl.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_image_impl.h>

//: Binary write to stream.
void vsl_b_write(vsl_b_ostream &os, const vil1_image & p)
{
  // write version number
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);

  // Get a serial_number for object being pointed to
  unsigned long id = os.get_serial_number(p.impl());

  // Find out if this is the first time the object being pointed to
  // is being saved
  if (id == 0)
  {
    id = os.add_serialisation_record(p.impl());

      // Say that this is the first time
      // that this object is being saved.
      // This isn't really necessary but
      // it is useful as an error check
    vsl_b_write(os, true);
    vsl_b_write(os, id);     // Save the serial number
// If you get an error in the next line, it could be because your type T
// has no vsl_b_write(vsl_b_ostream &,const T*)  defined on it.
// See the documentation in the .h file to see how to add it.
    vsl_b_write(os, p.impl());   // Only save the actual object if it
                                 // hasn't been saved before to this stream
  }
  else
  {
      // Say that this is not the first time
      // that this object is being saved.
      // This isn't really necessary but
      // it is useful as an error check

    vsl_b_write(os, false);
    vsl_b_write(os, id);         // Save the serial number
  }
}

//============================================================================
//: Binary load from stream.
void vsl_b_read(vsl_b_istream &is,  vil1_image& p)
{
  if (!is) return;

  short v;
  vsl_b_read(is, v);
  switch (v)
  {
   case 1:
   {
    bool first_time;
    vsl_b_read(is, first_time);

    unsigned long id;
    vsl_b_read(is, id);


    vil1_image_impl * impl = (vil1_image_impl *)
                             is.get_serialisation_pointer(id);
    if (first_time != (impl == 0))
    {
      // This checks that the saving stream and reading stream
      // both agree on whether or not this is the first time they
      // have seen this object.
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vil1_image&)\n"
               << "           De-serialisation failure\n";
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
    }

    if (impl == 0)
    {
      vsl_b_read(is, impl);
      is.add_serialisation_record(id, impl);
    }

    p = impl; // This operator method will set the internal pointer in
              // vil1_image to the impl.
    break;
   }

   default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vil1_image&)\n"
             << "           Unknown version number "<< v << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//=========================================================================
//: Output a human readable summary to the stream
void vsl_print_summary(vcl_ostream& os,const vil1_image & p)
{
  vsl_print_summary(os, p.impl());
}
