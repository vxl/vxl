// This is core/vil/io/vil_io_image_impl.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vil_io_image_impl.h"
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_clipon_binary_loader.txx>

//: Constructor
vil_io_image_impl::vil_io_image_impl()
{
}

//: Destructor
vil_io_image_impl::~vil_io_image_impl()
{
}


//: Write derived class to os using vil_image_impl reference
void vil_io_image_impl::b_write_by_base(vsl_b_ostream& os,
                                        const vil_image_impl& base) const
{
  vsl_b_write(os,base);
}

//: Write derived class to os using vil_image_impl reference
void vil_io_image_impl::b_read_by_base(vsl_b_istream& is,
                                       vil_image_impl& base) const
{
  vsl_b_read(is,base);
}

//: Print summary of derived class to os using vil_image_impl reference
void vil_io_image_impl::print_summary_by_base(vcl_ostream& os,
                                              const vil_image_impl& base) const
{
  vsl_print_summary(os,base);
}

//: Return name of class for which this object provides IO
vcl_string vil_io_image_impl::target_classname() const
{
  return vcl_string("vil_image_impl");
}

//: Return true if b is of class target_classname()
bool vil_io_image_impl::is_io_for(const vil_image_impl& b) const
{
  return b.is_a()==target_classname();
}

//=========================================================================
//: Binary save self to stream.
void vsl_b_write(vsl_b_ostream & os, const vil_image_impl & p)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  // Nothing to save.
}

//=========================================================================
//: Binary load self from stream.
void vsl_b_read(vsl_b_istream &is, vil_image_impl & p)
{
  if (!is) return;

  short ver;

  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
    // Nothing to load.
  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vil_image_impl&) \n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//=========================================================================
//: Output a human readable summary to the stream
void vsl_print_summary(vcl_ostream & /*os*/,const vil_image_impl & /*p*/)
{
  vcl_cerr << "vsl_print_summary() NYI\n";
}

//: Add example object to list of those that can be loaded
//  The vsl_binary_loader must see an example of each derived class
//  before it knows how to deal with them.
//  A clone is taken of b
void vsl_add_to_binary_loader(const vil_io_image_impl& b)
{
    vsl_clipon_binary_loader<vil_image_impl,vil_io_image_impl>::
      instance().add(b);
}


//: Binary save to stream by vil_image_impl pointer
void vsl_b_write(vsl_b_ostream &os, const vil_image_impl * b)
{
    vsl_clipon_binary_loader<vil_image_impl,vil_io_image_impl>::
      instance().write_object(os,b);
}

//: Binary read from stream by vil_image_impl pointer
void vsl_b_read(vsl_b_istream &is, vil_image_impl* &b)
{
    vsl_clipon_binary_loader<vil_image_impl,vil_io_image_impl>::
      instance().read_object(is,b);
}

//: Print summary to stream by vil_image_impl pointer
void vsl_print_summary(vcl_ostream &os, const vil_image_impl * b)
{
    vsl_clipon_binary_loader<vil_image_impl,vil_io_image_impl>::
      instance().print_object_summary(os,b);
}

// Explicitly instantiate loader
VSL_CLIPON_BINARY_LOADER_INSTANTIATE(vil_image_impl, vil_io_image_impl);
