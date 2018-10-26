#include "vsol_cylinder.h"
//:
// \file
#include <vgl/io/vgl_io_cylinder.h>
#include <vgl/vgl_vector_3d.h>

//: Return true iff the point p is inside (or on) this cylinder
bool vsol_cylinder::contains(vgl_point_3d<double> const& p)
{
  double x= cyl_.center().x() - ((cyl_.length()/2.0) * cyl_.orientation().x());
  double y= cyl_.center().y() - ((cyl_.length()/2.0) * cyl_.orientation().y());
  double z= cyl_.center().z() - ((cyl_.length()/2.0) * cyl_.orientation().z());
  vgl_point_3d<double> bottom_center(x, y, z);

  double x2= cyl_.center().x() + ((cyl_.length()/2.0) * cyl_.orientation().x());
  double y2= cyl_.center().y() + ((cyl_.length()/2.0) * cyl_.orientation().y());
  double z2= cyl_.center().z() + ((cyl_.length()/2.0) * cyl_.orientation().z());
  vgl_point_3d<double> top_center(x2, y2, z2);

  vgl_vector_3d<double> v = p - bottom_center;
  vgl_vector_3d<double> d = top_center - bottom_center;
  double dot_p = dot_product(v, d);
  double length_sqr = cyl_.length()*cyl_.length();

  if ((dot_p < 0.0) || (dot_p > length_sqr))
    return false;
  else {
    double dsq = v.sqr_length() - (dot_p*dot_p/length_sqr);
    if (dsq > (cyl_.radius()*cyl_.radius()))
      return false;
    else
      return true;
  }
}

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
    std::cerr << "vsol_cylinder: unknown I/O version " << ver << '\n';
  }
}

//: Return IO version number;
short vsol_cylinder::version() const
{
  return 1;
}

//: Print an ascii summary to the stream
void vsol_cylinder::print_summary(std::ostream &os) const
{
  os << *this;
}

//: describe to the output stream
void vsol_cylinder::describe(std::ostream &strm, int blanking) const
{
  if (blanking < 0) blanking = 0;
  while (blanking--) { strm << ' '; }
  strm << "[vsol_cylinder center=" << cyl_.center()
       << " radius=" << cyl_.radius() << " length=" << cyl_.length()
       << " direction=" << cyl_.orientation()
       << ']' << std::endl;
}

//: Binary save vsol_cylinder* to stream.
void
vsl_b_write(vsl_b_ostream &os, const vsol_cylinder* p)
{
  if (p==nullptr) {
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
  if (p==nullptr) {
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
  delete cyl; cyl=nullptr;
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

