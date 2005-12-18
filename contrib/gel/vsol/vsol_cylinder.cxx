#include "vsol_cylinder.h"
//:
// \file
#include <vgl/io/vgl_io_cylinder.h>

//----------------------------------------------------------------
// ================   Binary I/O Methods ========================
//----------------------------------------------------------------

//: Binary save self to stream.
void vsol_cylinder::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, cyl_);
}

//: Binary load self from stream
void vsol_cylinder::b_read(vsl_b_istream &is)
{
  if (!is)
    return;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1:
    vsl_b_read(is, cyl_);
    break;
   default:
    vcl_cerr << "vsol_cylinder: unknown I/O version " << ver << '\n';
  }
}

//: Return IO version number;
short vsol_cylinder::version() const
{
  return 1;
}

//: Print an ascii summary to the stream
void vsol_cylinder::print_summary(vcl_ostream &os) const
{
  os << *this;
}

//: describe to the output stream
void vsol_cylinder::describe(vcl_ostream &strm, int blanking) const
{
  if (blanking < 0) blanking = 0;
  while (blanking--) { strm << ' '; }
  strm << "[vsol_cylinder center=" << cyl_.center()
       << " radius=" << cyl_.radius() << " length=" << cyl_.length()
       << " direction=" << cyl_.orientation()
       << ']' << vcl_endl;
}

//: Binary save vsol_cylinder* to stream.
void
vsl_b_write(vsl_b_ostream &os, const vsol_cylinder* p)
{
  if (p==0) {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null pointer stored
    p->b_write(os);
  }
}

//: Binary save vsol_cylinder_sptr to stream.
void
vsl_b_write(vsl_b_ostream &os, const vsol_cylinder_sptr &p)
{
  if (p==0) {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null pointer stored
    p->b_write(os);
  }
}

//: Binary load vsol_cylinder* from stream.
void
vsl_b_read(vsl_b_istream &is, vsol_cylinder* &cyl)
{
  delete cyl; cyl=0;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    cyl = new vsol_cylinder();
    cyl->b_read(is);
  }
}

//: Binary load vsol_cylinder_sptr from stream.
void
vsl_b_read(vsl_b_istream &is, vsol_cylinder_sptr &cyl)
{
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    cyl = new vsol_cylinder();
    cyl->b_read(is);
  }
}

