#ifdef __GNUC__
#pragma implementation
#endif

// This is vxl/vil/io/vil_io_memory_image_impl.txx

#include <vil/vil_memory_image_impl.h>
#include <vil/io/vil_io_memory_image_format.h>
#include <vil/io/vil_io_memory_image_impl.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_binary_explicit_io.h>



//:
// \file
// \author Ian Scott and Nick Costen (Manchester)

//: Constructor
vil_io_memory_image_impl::vil_io_memory_image_impl()
{
}

//: Destructor
vil_io_memory_image_impl::~vil_io_memory_image_impl()
{
}

//: Create new object of type vil_memory_image_impl on heap
vil_image_impl* vil_io_memory_image_impl::new_object() const
{
  return new vil_memory_image_impl(0,0,0,VIL_BYTE );
}

//: Write derived class to os using vil_image_impl reference
void vil_io_memory_image_impl::b_write_by_base(vsl_b_ostream& os, 
                                               const vil_image_impl& base)
                                               const
{
  vsl_b_write(os,(vil_memory_image_impl&) base);
}

//: Write derived class to os using vil_image_impl reference
void vil_io_memory_image_impl::b_read_by_base(vsl_b_istream& is, 
                                              vil_image_impl& base) const
{
  vsl_b_read(is,(vil_memory_image_impl&) base);
}


//: Copy this object onto the heap and return a pointer
vil_io_image_impl* vil_io_memory_image_impl::clone() const
{
  return new vil_io_memory_image_impl(*this);
}

//: Return name of class for which this object provides IO
vcl_string vil_io_memory_image_impl::target_classname() const
{
  return vcl_string("vil_memory_image_impl");
}

//: Return true if b is of class target_classname()
bool vil_io_memory_image_impl::is_io_for(const vil_image_impl& b) const
{
  return (b.is_a()==target_classname());
}


//========================================================================
//: Binary save self to stream.
void vsl_b_write(vsl_b_ostream &os, const vil_memory_image_impl & p)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, p.planes());
  vsl_b_write(os, p.height());
  vsl_b_write(os, p.width());
  vsl_b_write(os,p.components());
  vsl_b_write(os,p.bits_per_component());
  vsl_b_write(os,p.component_format());
  int nelems = p.planes() * p.height() * p.width() * p.components(); 
  int size = nelems * p.bits_per_component() / CHAR_BIT;
  unsigned char* buf = new unsigned char[size];
  p.get_section(buf,0,0,p.width(),p.height());
  vsl_swap_bytes((char*) buf,p.bits_per_component(),nelems);
  os.os().write((const char*) buf,size);
  delete buf;
  
}

//========================================================================
//: Binary load self from stream.
void vsl_b_read(vsl_b_istream &is, vil_memory_image_impl & p)
{
 
  short v;
  vsl_b_read(is, v);
  switch(v)
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
      bits_per_component,vil_component_format(component_format));
    int nelems = p.planes() * p.height() * p.width() * p.components(); 
    int size = nelems * p.bits_per_component() / CHAR_BIT;
    unsigned char* buf = new unsigned char[size];
    is.is().read((char*) buf,size);
    vsl_swap_bytes((char *) buf,p.bits_per_component(),nelems);
    p.put_section(buf,0,0,width,height);
    delete buf;
    break;
    }
  default:
  vcl_cerr << "vsl_b_read(vil_memory_image_impl) Unknown version number ";
  vcl_cerr << v << vcl_endl;
  abort();
  }

}

//========================================================================
//: Output a human readable summary to the stream
void vsl_print_summary(vcl_ostream& os,const vil_memory_image_impl & p)
{
  os<<"vil_memory_image_impl : "<<vcl_endl;
  os<<"Num planes : "<<p.planes();
  os<<" Num rows : "<<p.height();
  os<<" Num cols "<<p.width();
  os<<" Num components : "<<p.components()<<vcl_endl;
  os<<"Num bits per comp : "<<p.bits_per_component();
  os<<" Component format ";
  switch (p.component_format())
  {
  case VIL_COMPONENT_FORMAT_UNKNOWN:
    os << "VIL_COMPONENT_FORMAT_UNKNOWN";
    break;
  case VIL_COMPONENT_FORMAT_UNSIGNED_INT:
    os << "VIL_COMPONENT_FORMAT_UNSIGNED_INT";
    break;
  case VIL_COMPONENT_FORMAT_SIGNED_INT:
    os << "VIL_COMPONENT_FORMAT_SIGNED_INT";
    break;
  case VIL_COMPONENT_FORMAT_IEEE_FLOAT:
    os << "VIL_COMPONENT_FORMAT_IEEE_FLOAT";
    break;
  case VIL_COMPONENT_FORMAT_COMPLEX:
    os << "VIL_COMPONENT_FORMAT_COMPLEX";
    break;
  default:
    os << "unknown";
    break;
  }
  
}




