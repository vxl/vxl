// This is core/vil1/io/vil1_io_memory_image_format.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vil1_io_memory_image_format.h"

//========================================================================
//: Binary save vil1_memory_image_format to stream.
void vsl_b_write(vsl_b_ostream &os, const vil1_memory_image_format& v)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, v.components);
  vsl_b_write(os, v.bits_per_component);
  vsl_b_write(os, int(v.component_format));
}

//========================================================================
//: Binary load vil1_memory_image_format from stream.
void vsl_b_read(vsl_b_istream &is, vil1_memory_image_format& v)
{
  if (!is) return;

  int cf;
  short w;
  vsl_b_read(is, w);
  switch (w)
  {
   case 1:
    vsl_b_read(is, v.components);
    vsl_b_read(is, v.bits_per_component);
    vsl_b_read(is, cf);
    v.component_format=(vil1_component_format)cf;
    break;

   default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vil1_memory_image_format&)\n"
             << "           Unknown version number "<< w << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//========================================================================
//: Output a human readable summary of a vil1_memory_image_format object
void vsl_print_summary(vcl_ostream &os, const vil1_memory_image_format& v)
{
  os<<"Memory image format : ( "<<v.components<<" , "<<v.bits_per_component<<" , ";
  switch (v.component_format)
  {
    case VIL1_COMPONENT_FORMAT_UNKNOWN:
      os << "VIL1_COMPONENT_FORMAT_UNKNOWN";
      break;
    case VIL1_COMPONENT_FORMAT_UNSIGNED_INT:
      os << "VIL1_COMPONENT_FORMAT_UNSIGNED_INT";
      break;
    case VIL1_COMPONENT_FORMAT_SIGNED_INT:
      os << "VIL1_COMPONENT_FORMAT_SIGNED_INT";
      break;
    case VIL1_COMPONENT_FORMAT_IEEE_FLOAT:
      os << "VIL1_COMPONENT_FORMAT_IEEE_FLOAT";
      break;
    case VIL1_COMPONENT_FORMAT_COMPLEX:
      os << "VIL1_COMPONENT_FORMAT_COMPLEX";
      break;
    default:
      os << "unknown";
      break;
  }
  os << " )";
}
