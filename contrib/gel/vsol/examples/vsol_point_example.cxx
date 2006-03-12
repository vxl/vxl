// Example use of the vsol_point_* classes
//
// Author: Peter Tu sep 19 2000

#include <vcl_iostream.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>

int main()
{
  // vsol is an abstract class so we need a concrete example

  vcl_cout << "2D example " << vcl_endl;

  vsol_point_2d_sptr p2d = new vsol_point_2d(5.0,10.0);
  vcl_cout << "X and Y" << vcl_endl;
  vcl_cout << p2d->x() << " " << p2d->y() << vcl_endl;

  vsol_point_2d_sptr p2d_1 = new vsol_point_2d(25.0,35.0);
  vcl_cout << "X and Y" << vcl_endl;
  vcl_cout << p2d_1->x() << " " << p2d_1->y() << vcl_endl;

  p2d_1->set_x(20.0);
  p2d_1->set_y(30.0);

  vcl_cout << "X and Y" << vcl_endl;
  vcl_cout << p2d_1->x() << " " << p2d_1->y() << vcl_endl;

  vsol_point_2d_sptr mid_2d = p2d_1->middle(*(p2d.ptr()));
  vcl_cout << "middle" << vcl_endl;
  vcl_cout << mid_2d->x() << " " << mid_2d->y() << vcl_endl;

  double dist=p2d_1->distance(p2d);
  vcl_cout << "Distance " << vcl_endl;
  vcl_cout << dist << vcl_endl;

  vcl_cout << "3D example " << vcl_endl;

  vsol_point_3d_sptr p3d = new vsol_point_3d(5.0,10.0,15.0);
  vcl_cout << "X and Y and Z" << vcl_endl;
  vcl_cout << p3d->x() << " " << p3d->y() << " " << p3d->z() << vcl_endl;

  vsol_point_3d_sptr p3d_1 = new vsol_point_3d(25.0,35.0,40.0);
  vcl_cout << "X and Y Z " << vcl_endl;
  vcl_cout << p3d_1->x() << " " << p3d_1->y() << " " << p3d_1->z() <<  vcl_endl;

  p3d_1->set_x(20.0);
  p3d_1->set_y(30.0);
  p3d_1->set_z(35.0);


  vcl_cout << "X and Y Z" << vcl_endl;
  vcl_cout << p3d_1->x() << " " << p3d_1->y() << " " << p3d_1->z() <<  vcl_endl;

  vsol_point_3d_sptr mid_3d = p3d_1->middle(*(p3d.ptr()));
  vcl_cout << "middle" << vcl_endl;
  vcl_cout << mid_3d->x() << " " << mid_3d->y() << " " << mid_3d->z() << vcl_endl;

  dist=p3d_1->distance(p3d);
  vcl_cout << "Distance " << vcl_endl;
  vcl_cout << dist << vcl_endl;

  return 0;
}
