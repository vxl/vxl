// This is core/vil1/io/vil1_io_memory_image_impl.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Ian Scott and Nick Costen (Manchester)

#include "vil1_io_memory_image_impl.h"
#include <vil1/vil1_memory_image_impl.h>
#include <vil1/io/vil1_io_memory_image_format.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_binary_explicit_io.h>

//: Constructor
vil1_io_memory_image_impl::vil1_io_memory_image_impl()
{
}

//: Destructor
vil1_io_memory_image_impl::~vil1_io_memory_image_impl()
{
}

//: Create new object of type vil1_memory_image_impl on heap
vil1_image_impl* vil1_io_memory_image_impl::new_object() const
{
  return new vil1_memory_image_impl(0,0,0,VIL1_BYTE );
}

//: Write derived class to os using vil1_image_impl reference
void vil1_io_memory_image_impl::b_write_by_base(vsl_b_ostream& os,
                                                const vil1_image_impl& base)
                                                const
{
  vsl_b_write(os,(vil1_memory_image_impl&) base);
}

//: Write derived class to os using vil1_image_impl reference
void vil1_io_memory_image_impl::b_read_by_base(vsl_b_istream& is,
                                               vil1_image_impl& base) const
{
  vsl_b_read(is,(vil1_memory_image_impl&) base);
}


//: Copy this object onto the heap and return a pointer
vil1_io_image_impl* vil1_io_memory_image_impl::clone() const
{
  return new vil1_io_memory_image_impl(*this);
}


//========================================================================
//: Binary save self to stream.
void vsl_b_write(vsl_b_ostream &os, const vil1_memory_image_impl & p)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, p.planes());
  vsl_b_write(os, p.height());
  vsl_b_write(os, p.width());
  vsl_b_write(os,p.components());
  vsl_b_write(os,p.bits_per_component());
  vsl_b_write(os,(int)(p.component_format()));
  int nelems = p.planes() * p.height() * p.width() * p.components();
  int size = nelems * p.bits_per_component() / CHAR_BIT;
  unsigned char* buf = new unsigned char[size];
  p.get_section(buf,0,0,p.width(),p.height());
  vsl_swap_bytes((char*) buf,p.bits_per_component() / CHAR_BIT,nelems);
  os.os().write((const char*) buf,size);
  delete[] buf;
}

//========================================================================
//: Binary load self from stream.
void vsl_b_read(vsl_b_istream &is, vil1_memory_image_impl & p)
{
  if (!is) return;

  short v;
  vsl_b_read(is, v);
  switch (v)
  {
   case 1:
   {
    int planes, height, width;
    int components, bits_per_component, component_format;
    vsl_b_read(is, planes);
    vsl_b_read(is, height);
    vsl_b_read(is, width);
    vsl_b_read(is,components);
    vsl_b_read(is,bits_per_component);
    vsl_b_read(is,component_format);
    p.resize(planes,width,height,components,
             bits_per_component,vil1_component_format(component_format));
    int nelems = p.planes() * p.height() * p.width() * p.components();
    int size = nelems * p.bits_per_component() / CHAR_BIT;
    unsigned char* buf = new unsigned char[size];
    is.is().read((char*) buf,size);
    vsl_swap_bytes((char *) buf,p.bits_per_component() / CHAR_BIT, nelems);
    p.put_section(buf,0,0,width,height);
    delete[] buf;
    break;
   }
   default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vil1_memory_image_impl&)\n"
             << "           Unknown version number "<< v << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
  }
}

//========================================================================
//: Output a human readable summary to the stream
void vsl_print_summary(vcl_ostream& os,const vil1_memory_image_impl & p)
{
  os<<"vil1_memory_image_impl :"
    <<"\nNum planes : "<<p.planes()
    <<" Num rows : "<<p.height()
    <<" Num cols "<<p.width()
    <<" Num components : "<<p.components()
    <<"\nNum bits per comp : "<<p.bits_per_component()
    <<" Component format ";
  switch (p.component_format())
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
}

