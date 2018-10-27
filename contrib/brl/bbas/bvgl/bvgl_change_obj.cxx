//:
// \file


#include "bvgl_change_obj.h"
#include <vgl/io/vgl_io_polygon.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d.h>

bvgl_change_obj::bvgl_change_obj(const bvgl_change_obj& other)
: vbl_ref_count()
{
  class_ = other.class_;
  poly_ = other.poly_;
}

//: Return IO version number;
unsigned char
bvgl_change_obj::version(  ) const
{
  return 1;
}

//: return the centroid of the polygon
void bvgl_change_obj::centroid(double &x_cent, double &y_cent) const
{
  //: find the centre of this polygon
  x_cent = 0.0, y_cent = 0.0;
  int tot_size = 0;
  for (unsigned s = 0; s < poly_.num_sheets(); s++) {
    for (auto si : poly_[s]) {
      x_cent += si.x();
      y_cent += si.y();
    }
    tot_size += poly_[s].size();
  }
  x_cent /= tot_size;
  y_cent /= tot_size;
}

//: binary IO write
void bvgl_change_obj::b_write(vsl_b_ostream& os)
{
  // first write the version number;
  unsigned char ver = version();
  vsl_b_write(os, ver);

  vsl_b_write(os, class_);
  vsl_b_write(os, poly_);
}


//: binary IO read
void bvgl_change_obj::b_read(vsl_b_istream& is)
{
  // first read the version number;
  unsigned char ver;
  vsl_b_read(is, ver);

  switch (ver)
  {
   case 1:
    vsl_b_read(is, class_);
    vsl_b_read(is, poly_);
    break;
   default:
    std::cout << "In bvgl_change_obj::b_read() -- Unrecognized version number\n";
    break;
  }

  return;
}

#if 0
void bvgl_change_obj::xml_read()
{
}

void bvgl_change_obj::xml_write()
{
}
#endif // 0
